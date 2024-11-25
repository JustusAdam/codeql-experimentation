#pragma once

#include <string>
#include <tuple>
#include <memory>
#include <optional>
#include <cstdint>
#include <vector>

// Basic web types (similar to actix-web)
class HttpRequest
{
public:
    class ConnectionInfo
    {
    public:
        std::string get_peer_addr() const;
    };

    ConnectionInfo get_connection_info() const;
};

class HttpResponse
{
    std::vector<uint8_t> response;

public:
    static HttpResponse Ok();
    template <typename T>
    HttpResponse &set_json(const T &payload)
    {
        encode_json(payload, response);
        return *this;
    }
};

template <typename T>
void encode_json(const T &payload, std::vector<uint8_t> &response);

namespace web
{
    template <typename T>
    class Json
    {
        T inner;

    public:
        T get() const { return inner; }
        const T &operator*() const { return inner; }
        const T *operator->() const { return &inner; }
    };
}

// libmcaptcha types
struct Work
{
    std::string string;
    uint64_t nonce;
    std::string result;
    std::string key;
};

class CaptchaSystem
{
public:
    // Returns {validation_token, difficulty_factor}
    std::tuple<std::string, uint32_t> verify_pow(const Work &work, const std::string &ip);
};

// Database types
struct CreatePerformanceAnalytics
{
    uint32_t difficulty_factor;
    uint32_t time;
    std::string worker_type;
};

class Database
{
public:
    bool analytics_captcha_is_published(const std::string &key);
    void analysis_save(const std::string &key, const CreatePerformanceAnalytics &analytics);
    void update_max_nonce_for_level(const std::string &key, uint32_t difficulty_factor, uint32_t nonce);
};

class AppData;

class Stats
{
public:
    void record_solve(const std::shared_ptr<AppData> &data, const std::string &key);
};

// Main application data structure
struct AppData
{
    CaptchaSystem captcha;
    Stats stats;
    Database db;
};
