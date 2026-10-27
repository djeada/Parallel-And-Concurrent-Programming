#pragma once

// Bounded asynchronous HTTP using libcurl's multi event loop. No thread per URL.
#include <curl/curl.h>

#include <algorithm>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace curl_example {

inline void check(CURLcode code) {
    if (code != CURLE_OK) throw std::runtime_error(curl_easy_strerror(code));
}
inline void check(CURLMcode code) {
    if (code != CURLM_OK) throw std::runtime_error(curl_multi_strerror(code));
}

class Environment {
public:
    Environment() { check(curl_global_init(CURL_GLOBAL_DEFAULT)); }
    ~Environment() { curl_global_cleanup(); }
    Environment(const Environment&) = delete;
    Environment& operator=(const Environment&) = delete;
};

class Multi {
public:
    Multi() : value_(curl_multi_init()) {
        if (!value_) throw std::runtime_error("curl_multi_init failed");
    }
    ~Multi() { curl_multi_cleanup(value_); }
    Multi(const Multi&) = delete;
    Multi& operator=(const Multi&) = delete;
    CURLM* get() const noexcept { return value_; }

private:
    CURLM* value_;
};

struct Response {
    std::string body;
    CURLcode outcome = CURLE_OK;
};

inline std::size_t append_body(char* data, std::size_t size, std::size_t count, void* context) noexcept {
    try {
        static_cast<Response*>(context)->body.append(data, size * count);
        return size * count;
    } catch (...) {
        return 0; // Signal failure; never let C++ exceptions cross a C callback.
    }
}

class Transfer {
public:
    Transfer(CURLM* multi, const std::string& url, Response& response)
        : multi_(multi), easy_(curl_easy_init(), curl_easy_cleanup), response_(response) {
        if (!easy_) throw std::runtime_error("curl_easy_init failed");
        check(curl_easy_setopt(easy_.get(), CURLOPT_URL, url.c_str()));
        check(curl_easy_setopt(easy_.get(), CURLOPT_WRITEFUNCTION, append_body));
        check(curl_easy_setopt(easy_.get(), CURLOPT_WRITEDATA, &response));
        check(curl_easy_setopt(easy_.get(), CURLOPT_PRIVATE, this));
        check(curl_easy_setopt(easy_.get(), CURLOPT_FOLLOWLOCATION, 1L));
        check(curl_easy_setopt(easy_.get(), CURLOPT_FAILONERROR, 1L));
        check(curl_easy_setopt(easy_.get(), CURLOPT_NOSIGNAL, 1L));
        check(curl_easy_setopt(easy_.get(), CURLOPT_CONNECTTIMEOUT_MS, 2000L));
        check(curl_easy_setopt(easy_.get(), CURLOPT_TIMEOUT_MS, 5000L));
        check(curl_easy_setopt(easy_.get(), CURLOPT_USERAGENT, "cpp20-concurrency-example/1.0"));
        check(curl_multi_add_handle(multi_, easy_.get()));
    }
    ~Transfer() { curl_multi_remove_handle(multi_, easy_.get()); }
    Transfer(const Transfer&) = delete;
    Transfer& operator=(const Transfer&) = delete;
    void complete(CURLcode outcome) noexcept { response_.outcome = outcome; }

private:
    CURLM* multi_;
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> easy_;
    Response& response_;
};

inline std::vector<Response> fetch_many(std::span<const std::string> urls, std::size_t concurrency) {
    if (concurrency == 0) throw std::invalid_argument("Concurrency must be positive");
    std::vector<Response> responses(urls.size()); // Stable callback storage.
    Multi multi;
    std::vector<std::unique_ptr<Transfer>> active; // Destroy before the multi handle.
    active.reserve(std::min(concurrency, urls.size()));
    std::size_t next = 0;
    while (next < urls.size() || !active.empty()) {
        while (next < urls.size() && active.size() < concurrency) {
            active.push_back(std::make_unique<Transfer>(multi.get(), urls[next], responses[next]));
            ++next;
        }
        int running = 0;
        check(curl_multi_perform(multi.get(), &running));
        int queued = 0;
        while (auto* message = curl_multi_info_read(multi.get(), &queued)) {
            if (message->msg != CURLMSG_DONE) continue;
            char* context = nullptr;
            check(curl_easy_getinfo(message->easy_handle, CURLINFO_PRIVATE, &context));
            auto* completed = reinterpret_cast<Transfer*>(context);
            completed->complete(message->data.result);
            std::erase_if(active, [completed](const auto& transfer) { return transfer.get() == completed; });
        }
        if (!active.empty()) {
            int ready = 0;
            check(curl_multi_poll(multi.get(), nullptr, 0, 100, &ready));
        }
    }
    return responses;
}

} // namespace curl_example
