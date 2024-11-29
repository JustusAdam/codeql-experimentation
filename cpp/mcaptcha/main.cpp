#include <string>
#include <optional>
#include <memory>
#include <exception>
#include <future>
#include "third_party/third_party.hpp"

// Equivalent structure for validation token
struct ValidationToken
{
    std::string token;
};

template <>
void encode_json(const ValidationToken &value, std::vector<uint8_t> &response)
{
    response.insert(response.end(), value.token.begin(), value.token.end());
}

// Equivalent structure for API work
struct ApiWork
{
    std::string string;
    std::string result;
    uint64_t nonce;
    std::string key;
    std::optional<uint32_t> time;
    std::optional<std::string> worker_type;
};

// Conversion function from ApiWork to Work
Work convert_to_work(const ApiWork &value)
{
    return Work{
        .string = value.string,
        .nonce = value.nonce,
        .result = value.result,
        .key = value.key};
}

// Main verification function
HttpResponse verify_pow(
    const HttpRequest &req,
    const web::Json<ApiWork> &payload,
    const std::shared_ptr<AppData> &data)
{
    std::string ip = req.get_connection_info().get_peer_addr();

    const std::string &key = payload->key;
    ApiWork work_payload = payload.get();
    auto worker_type = work_payload.worker_type;
    auto time = work_payload.time;
    auto nonce = work_payload.nonce;

    // Verify the proof of work
    auto [res, difficulty_factor] = data->captcha.verify_pow(
        convert_to_work(work_payload),
        ip);

    data->stats.record_solve(data, key);

    // Handle analytics if time and worker_type are present
    if (time && worker_type)
    {
        CreatePerformanceAnalytics analytics{
            .difficulty_factor = difficulty_factor,
            .time = *time,
            .worker_type = *worker_type};

        if (data->db.analytics_captcha_is_published(key))
        {
            data->db.analysis_save(key, analytics);
        }
    }

    // Update max nonce
    data->db.update_max_nonce_for_level(
        key,
        difficulty_factor,
        static_cast<uint32_t>(nonce));

    // Prepare and return response
    ValidationToken response_payload{.token = res};
    return HttpResponse::Ok()
        .set_json(response_payload);
}

HttpResponse verify_pow_async(
    const HttpRequest &req,
    const web::Json<ApiWork> &payload,
    const std::shared_ptr<AppData> &data)
{
    std::string ip = req.get_connection_info().get_peer_addr();

    const std::string &key = payload->key;
    ApiWork work_payload = payload.get();
    auto worker_type = work_payload.worker_type;
    auto time = work_payload.time;
    auto nonce = work_payload.nonce;

    // Verify the proof of work
    auto [res, difficulty_factor] = std::async([&]()
                                               { return data->captcha.verify_pow(
                                                     convert_to_work(work_payload),
                                                     ip); })
                                        .get();
    std::async([&]()
               { data->stats.record_solve(data, key); })
        .get();

    // Handle analytics if time and worker_type are present
    if (time && worker_type)
    {
        CreatePerformanceAnalytics analytics{
            .difficulty_factor = difficulty_factor,
            .time = *time,
            .worker_type = *worker_type};

        if (std::async([&]()
                       { return data->db.analytics_captcha_is_published(key); })
                .get())
        {
            std::async([&]()
                       { data->db.analysis_save(key, analytics); })
                .get();
        }
    }

    // Update max nonce
    std::async([&]()
               { data->db.update_max_nonce_for_level(
                     key,
                     difficulty_factor,
                     static_cast<uint32_t>(nonce)); })
        .get();

    // Prepare and return response
    ValidationToken response_payload{.token = res};
    return HttpResponse::Ok()
        .set_json(response_payload);
}

HttpResponse delete_account(Identity &id, web::Json<Password> &payload, AppData &data)
{
    auto &username = id.identity();
    auto hash = data.db.get_password(username);

    if (Config::verify(hash, payload->password))
    {
        data.db.delete_user(username);
        id.forget();
        return HttpResponse::Ok();
    }
    else
    {
        throw std::exception();
    }
}

HttpResponse delete_account_async(Identity &id, web::Json<Password> &payload, AppData &data)
{
    auto &username = id.identity();
    auto hash = std::async(&Database::get_password, data.db, username).get();

    if (Config::verify(hash, payload->password))
    {
        auto hash = std::async([&]()
                               { return data.db.get_password(username); })
                        .get();

        id.forget();
        return HttpResponse::Ok();
    }
    else
    {
        throw std::exception();
    }
}