#include "third_party.hpp"

std::string HttpRequest::ConnectionInfo::get_peer_addr() const
{
    return std::string();
}

HttpRequest::ConnectionInfo HttpRequest::get_connection_info() const
{
    return ConnectionInfo();
}

HttpResponse HttpResponse::Ok()
{
    return HttpResponse();
}

std::tuple<std::string, uint32_t> CaptchaSystem::verify_pow(const Work &work, const std::string &ip)
{
    return std::make_tuple("", 0);
}

bool Database::analytics_captcha_is_published(const std::string &key)
{
    return true;
}

void Database::analysis_save(const std::string &key, const CreatePerformanceAnalytics &analytics)
{
}

void Database::update_max_nonce_for_level(const std::string &key, uint32_t difficulty_factor, uint32_t nonce) {}

void Stats::record_solve(const std::shared_ptr<AppData> &data, const std::string &key) {}
