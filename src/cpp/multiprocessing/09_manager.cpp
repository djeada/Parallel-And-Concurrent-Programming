#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

const std::string shared_data_file = "shared_data.txt";

class Manager {
public:
    Manager(int num_processes) : num_processes_(num_processes), pids_(num_processes) {}

    void start() {
        for (int i = 0; i < num_processes_; ++i) {
            pids_[i] = fork();
            if (pids_[i] == 0) {
                worker(i);
                exit(0);
            }
        }

        for (const auto &pid : pids_) {
            int status;
            waitpid(pid, &status, 0);
        }
    }

    void print_results() {
        std::cout << "All processes have completed. The contents of the shared file are:" << std::endl;
        std::ifstream infile(shared_data_file);
        std::string line;
        while (std::getline(infile, line)) {
            std::cout << line << std::endl;
        }
        infile.close();
    }

private:
    int num_processes_;
    std::vector<pid_t> pids_;

    void worker(int id) {
        std::ofstream outfile(shared_data_file, std::ios_base::app);
        outfile << "Process " << id << " is writing to the shared file." << std::endl;
        outfile.close();
    }
};

int main() {
    const int num_processes = 3;

    Manager manager(num_processes);
    manager.start();
    manager.print_results();

    return 0;
}
