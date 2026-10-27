#pragma once

// C++ ownership around MPI's supported C interface, not the removed MPI:: API.
#include <mpi.h>

#include <array>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <utility>

namespace mpi_example {

inline void check(int result) {
    if (result == MPI_SUCCESS) return;
    std::array<char, MPI_MAX_ERROR_STRING> message{};
    int length = 0;
    MPI_Error_string(result, message.data(), &length);
    std::cerr << "MPI error: " << message.data() << '\n';
    // A failed rank cannot safely leave its peers blocked in collectives.
    MPI_Abort(MPI_COMM_WORLD, result);
    std::abort();
}

class Environment {
public:
    Environment(int& argc, char**& argv) {
        check(MPI_Init(&argc, &argv));
        check(MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN));
    }
    ~Environment() { MPI_Finalize(); }
    Environment(const Environment&) = delete;
    Environment& operator=(const Environment&) = delete;
};

// Program locals (buffers, communicators, types, requests) leave scope before
// the environment finalizes. Unexpected C++ exceptions abort the whole job.
template <typename Function>
int run(int& argc, char**& argv, Function&& program) {
    Environment environment(argc, argv);
    try { return std::forward<Function>(program)(); }
    catch (const std::exception& error) {
        std::cerr << "MPI example failed: " << error.what() << '\n';
    }
    catch (...) { std::cerr << "MPI example failed with an unknown exception\n"; }
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    return EXIT_FAILURE;
}

class Communicator {
public:
    Communicator() = default;
    ~Communicator() { if (value_ != MPI_COMM_NULL) MPI_Comm_free(&value_); }
    Communicator(const Communicator&) = delete;
    Communicator& operator=(const Communicator&) = delete;
    MPI_Comm get() const noexcept { return value_; }
    MPI_Comm* out() noexcept { return &value_; }

private:
    MPI_Comm value_ = MPI_COMM_NULL;
};

class Datatype {
public:
    Datatype() = default;
    ~Datatype() { if (value_ != MPI_DATATYPE_NULL) MPI_Type_free(&value_); }
    Datatype(const Datatype&) = delete;
    Datatype& operator=(const Datatype&) = delete;
    Datatype(Datatype&& other) noexcept
        : value_(std::exchange(other.value_, MPI_DATATYPE_NULL)) {}
    MPI_Datatype get() const noexcept { return value_; }
    MPI_Datatype* out() noexcept { return &value_; }

private:
    MPI_Datatype value_ = MPI_DATATYPE_NULL;
};

} // namespace mpi_example
