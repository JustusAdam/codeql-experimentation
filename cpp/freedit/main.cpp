#include <optional>
#include <string>
#include <vector>
#include <set>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <thread>
#include <iostream>

#include "flib/flib.hpp"

// Struct for PostContent
struct PostContent
{
    struct Markdown
    {
        std::string content;
        Markdown(const std::string &c) : content(c) {}
        Markdown() = default;
    };
    // Other potential variants...
};

// Forward declarations for classes/structs
template <typename C>
class TypedHeader
{
public:
    std::optional<std::string> get(const std::string &key) const
    {
        return "";
    }
};
class Cookie
{
};
class FormPost
{
public:
    std::string title;
    uint32_t iid;
    bool delete_draft;
    std::string tags;
    std::string content;
    bool is_draft;
};
class AppError
{
private:
    std::string message;

public:
    AppError(const std::string &msg) : message(msg) {}
};
class SiteConfig;
class Claim;
class Inn
{
public:
    std::string inn_type;
};
enum class PostStatus
{
    Normal
};
class Post
{
public:
    uint32_t pid;
    uint32_t uid;
    uint32_t iid;
    std::string title;
    std::vector<std::string> tags;
    PostContent::Markdown content;
    int64_t created_at;
    PostStatus status;

    Post() {}

    Post(uint32_t pid, uint32_t uid, uint32_t iid, const std::string &title, const std::vector<std::string> &tags, const PostContent::Markdown &content, int64_t created_at, PostStatus status)
        : pid(pid), uid(uid), iid(iid), title(title), tags(tags), content(content), created_at(created_at), status(status) {}
};
class User;
class Database;
// Method declarations for InnRole
enum class InnRole
{
    Limited
};

// Enums
enum class NtType
{
    PostMention,
};

std::string COOKIE_NAME = "";

// Forward declarations for functions

std::string trim(const std::string &str)
{
    std::string result(str);
    result.erase(0, result.find_first_not_of(" \t\n\r\f\v"));
    result.erase(result.find_last_not_of(" \t\n\r\f\v") + 1);
    return result;
}

template <typename T, typename K>
T get_one_by_key(Database &db, const std::string &tree, const K &key)
{
    auto res = db.open_tree(tree)->get(key);
    if (!res.has_value())
    {
        throw AppError("not found");
    }
    std::optional<T> deser = deserialize<T>(res.value());
    if (deser.has_value())
    {
        return deser.value();
    }
    else
    {
        throw AppError("deserialize failed");
    }
}

template <typename T>
T get_one(Database &db, const std::string &tree, uint32_t id)
{
    return get_one_by_key<T, std::vector<uint8_t>>(db, tree, u32_to_ivec(id));
}

template <typename T>
void set_one(Database &db, const std::string &tree, uint32_t id, const T &value)
{
    set_one_with_key(db, tree, u32_to_ivec(id), value);
}

template <typename T>
void set_one_with_key(Database &db, const std::string &tree, const std::vector<uint8_t> &key, const T value)
{
    db.open_tree(tree)->set(key, serialize(value));
}

std::optional<std::vector<uint8_t>> increment(std::optional<std::vector<uint8_t>> value)
{
    if (value.has_value())
    {
        std::optional<uint32_t> deser = deserialize<uint32_t>(value.value());
        if (deser.has_value())
        {
            uint32_t v = deser.value() + 1;
            return serialize(v);
        }
    }
    return std::nullopt;
}

uint32_t incr_id(Database::Tree &tree, const std::string &counter)
{
    auto ser = tree.update_and_fetch(std::vector<uint8_t>(counter.begin(), counter.end()), increment);
    if (ser.has_value())
    {
        std::optional<uint32_t> deser = deserialize<uint32_t>(ser.value());
        if (deser.has_value())
        {
            return deser.value();
        }
    }
    throw AppError("failed to increment id");
}

std::optional<uint32_t> get_id_by_name(Database &db, const std::string &tree, const std::string &name)
{
    std::string name_2(name);
    name_2.replace(name_2.begin(), name_2.end(), " ", "_");
    std::transform(name_2.begin(), name_2.end(), name_2.begin(), tolower);
    auto res = db.open_tree(tree)->get(std::vector<uint8_t>(name_2.begin(), name_2.end()));
    if (res.has_value())
    {
        return deserialize<uint32_t>(res.value());
    }
    else
    {
        return std::nullopt;
    }
}

void add_notification(Database &db, uint32_t uid, NtType type, uint32_t pid, uint32_t cid)
{
    auto nid = incr_id(*db.open_tree("default"), "notifications_count");
    std::vector<uint8_t> k = u32_to_ivec(uid);
    auto nid_v = u32_to_ivec(nid);
    k.insert(k.end(), nid_v.begin(), nid_v.end());
    auto t_v = u32_to_ivec(static_cast<uint8_t>(type));
    k.insert(t_v.end(), t_v.begin(), t_v.end());

    auto v = u32_to_ivec(pid);
    auto cid_v = u32_to_ivec(cid);
    v.insert(v.end(), cid_v.begin(), cid_v.end());
    std::vector<uint8_t> zero = u32_to_ivec(0);
    v.insert(v.end(), zero.begin(), zero.end());
    db.open_tree("notifications")->insert(k, v);
}

std::vector<std::string> extract_element(const std::string &input, size_t max_len, char delimiter)
{
    std::vector<std::string> result;
    std::istringstream iss(input);
    std::string token;

    std::getline(iss, token, delimiter);

    while (std::getline(iss, token, delimiter))
    {
        if (token.empty())
            continue;

        size_t space_pos = token.find(' ');
        if (space_pos != std::string::npos)
        {
            std::string tag = token.substr(0, space_pos);
            if (!tag.empty() && tag.length() <= 25)
            {
                result.push_back(tag);
                if (result.size() >= max_len)
                {
                    break;
                }
            }
        }
    }

    return result;
}

Database DB;

// Additional method declarations
class User
{
public:
    static void update_stats(Database &db, uint32_t uid, const std::string &action)
    {
        const auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::ostringstream key;
        key << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << "_" << uid << "_" << action;
        incr_id(*db.open_tree("user_stats"), key.str());
    }
    static bool is_mod(const Database &db, uint32_t uid, uint32_t iid);
    static bool has_unread(const Database &db, uint32_t uid);
    std::string username;
};

// Method declarations for Claim
class Claim
{
public:
    static std::optional<Claim> get(Database &db, const TypedHeader<Cookie> &cookie, const SiteConfig &config);
    void update_last_write(Database &db);

    uint32_t uid;
    int64_t last_write;
    uint32_t role;
    uint32_t session_id;
};

// Method declarations for SiteConfig
class SiteConfig
{
public:
    static SiteConfig get(Database &db)
    {
        std::string key = "site_config";
        std::optional<std::vector<uint8_t>> v = db.open_tree("default")->get(std::vector<uint8_t>(key.begin(), key.end()));
        if (v.has_value())
        {
            auto deser = deserialize<SiteConfig>(v.value());
            if (deser.has_value())
            {
                return deser.value();
            }
        }
        return SiteConfig();
    }
    int64_t post_interval;
    size_t per_page;
    bool read_only;

    SiteConfig() = default;
};

static std::optional<InnRole> get_inn_role(Database &db, uint32_t iid, uint32_t uid)
{
    auto tree = db.open_tree("inn_users");
    auto v = u32_to_ivec(iid);
    auto uid_v = u32_to_ivec(uid);
    v.insert(v.end(), uid_v.begin(), uid_v.end());
    auto val = tree->get(v);
    if (val.has_value())
    {
        return deserialize<InnRole>(val.value());
    }
    return std::nullopt;
}

std::optional<Claim> Claim::get(Database &db, const TypedHeader<Cookie> &cookie, const SiteConfig &site_config)
{
    auto o_session = cookie.get(COOKIE_NAME);
    if (!o_session.has_value())
    {
        return std::nullopt;
    }

    auto session = o_session.value();

    auto end = session.find('_');
    if (end == std::string::npos)
    {
        return std::nullopt;
    }
    std::string timestamp_s(session.begin(), session.begin() + end);
    auto tree = db.open_tree("session");

    int64_t timestamp = std::stoll(timestamp_s);

    auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    if (now > timestamp)
    {
        return std::nullopt;
    }

    auto v = tree->get(std::vector<uint8_t>(session.begin(), session.end()));
    if (!v.has_value())
    {
        return std::nullopt;
    }

    auto claim = deserialize<Claim>(v.value());

    if (site_config.read_only && claim->role != 0)
    {
        return std::nullopt;
    }
    if (claim->role == 3)
    {
        return std::nullopt;
    }
    return claim;
}

void Claim::update_last_write(Database &db)
{
    this->last_write = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    set_one_with_key(db, "sessions", u32_to_ivec(this->session_id), *this);
}

// inn_add_index function
void inn_add_index(Database &DB, uint32_t iid, uint32_t pid, uint32_t created_at, uint32_t visibility)
{
    std::vector<uint8_t> iid_ivec = u32_to_ivec(iid);
    std::vector<uint8_t> pid_ivec = u32_to_ivec(pid);
    std::vector<uint8_t> created_at_ivec = u32_to_ivec(created_at);
    std::vector<uint8_t> visibility_ivec = u32_to_ivec(visibility);

    std::vector<uint8_t> k(iid_ivec.begin(), iid_ivec.end());
    k.insert(k.end(), pid_ivec.begin(), pid_ivec.end());
    std::vector<uint8_t> v(created_at_ivec.begin(), created_at_ivec.end());
    v.insert(v.end(), visibility_ivec.begin(), visibility_ivec.end());

    DB.open_tree("inn_index")->insert(k, v);
}

// inn_rm_index function
std::optional<AppError> inn_rm_index(Database &DB, uint32_t iid, uint32_t pid)
{
    std::vector<uint8_t> iid_ivec = u32_to_ivec(iid);
    std::vector<uint8_t> pid_ivec = u32_to_ivec(pid);

    std::vector<uint8_t> k(iid_ivec.begin(), iid_ivec.end());
    k.insert(k.end(), pid_ivec.begin(), pid_ivec.end());

    try
    {
        DB.open_tree("inn_index")->remove(k);
        return std::nullopt;
    }
    catch (const std::exception &e)
    {
        return AppError("inn_rm_index error: " + std::string(e.what()));
    }
}

void replace_all(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

// Function declaration
std::optional<std::string> edit_post_post(
    std::optional<TypedHeader<Cookie>> cookie,
    uint32_t old_pid,
    FormPost input)
{
    if (!cookie.has_value())
    {
        throw AppError("non login");
    }

    SiteConfig site_config = SiteConfig::get(DB);
    std::optional<Claim> claim = Claim::get(DB, *cookie, site_config);
    if (!claim.has_value())
    {
        throw AppError("non login");
    }

    bool is_draft = input.is_draft;
    bool delete_draft = input.delete_draft;

    std::vector<uint8_t> k;
    k.insert(k.end(), u32_to_ivec(claim->uid).begin(), u32_to_ivec(claim->uid).end());
    k.insert(k.end(), input.title.begin(), input.title.end());

    if (delete_draft)
    {
        DB.open_tree("drafts")->remove(k);
        return "Redirect::to(\"/post/edit/0\")";
    }
    if (is_draft)
    {
        set_one_with_key(DB, "drafts", k, &input);
        return "Redirect::to(\"/post/edit/0\")";
    }

    uint32_t iid = input.iid;
    std::optional<InnRole> inn_role = get_inn_role(DB, iid, claim->uid);
    if (!inn_role.has_value() || *inn_role <= InnRole::Limited)
    {
        throw AppError("unauthorized");
    }

    int64_t created_at = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (created_at - claim->last_write < site_config.post_interval)
    {
        throw AppError("write interval");
    }

    Inn inn = get_one<Inn>(DB, "inns", iid);

    uint32_t pid = (old_pid == 0) ? incr_id(*DB.open_tree("default"), "posts_count") : old_pid;
    std::vector<uint8_t> pid_ivec = u32_to_ivec(pid);

    std::vector<std::string> tags;
    uint32_t visibility = 0;
    if (inn.inn_type == "Private")
    {
        visibility = 10;
    }
    else
    {
        std::set<std::string> tags_set;
        std::istringstream iss(input.tags);
        std::string tag;
        while (std::getline(iss, tag, '#'))
        {
            tag = trim(tag);
            if (!tag.empty())
            {
                tags_set.insert(tag);
            }
        }

        tags.assign(tags_set.begin(), tags_set.end());
        if (tags.size() > 5)
        {
            tags.resize(5);
        }

        Batch batch;
        if (old_pid > 0)
        {
            Post post = get_one<Post>(DB, "posts", old_pid);
            if (post.uid != claim->uid)
            {
                throw AppError("unauthorized");
            }

            if (post.status != PostStatus::Normal)
            {
                throw AppError("locked or hidden");
            }

            if (post.iid != iid)
            {
                throw AppError("not found");
            }

            created_at = post.created_at;
            for (const auto &old_tag : post.tags)
            {
                std::vector<uint8_t> k(old_tag.begin(), old_tag.end());
                k.insert(k.end(), pid_ivec.begin(), pid_ivec.end());
                batch.remove(k);
            }
        }

        for (const auto &tag : tags)
        {
            std::vector<uint8_t> k(tag.begin(), tag.end());
            k.insert(k.end(), pid_ivec.begin(), pid_ivec.end());
            batch.insert(k, std::vector<uint8_t>());
        }
        DB.open_tree("tags")->apply_batch(batch);
    }

    std::string content = input.content;
    std::vector<std::string> notifications = extract_element(content, 5, '@');
    for (const auto &notification : notifications)
    {
        uint32_t uid;
        std::string username;
        try
        {
            uid = std::stoul(notification);
            User user = get_one<User>(DB, "users", uid);
            username = user.username;
        }
        catch (const std::invalid_argument &)
        {
            std::optional<uint32_t> uid_opt = get_id_by_name(DB, "usernames", notification);
            if (!uid_opt.has_value())
            {
                continue;
            }
            uid = *uid_opt;
            username = notification;
        }

        std::string notification_link = "<span class='replytag'>[![](/static/avatars/" + std::to_string(uid) + ".png)" + username + "](/user/" + std::to_string(uid) + ")";
        std::string from = "@" + notification;
        std::string to = "@" + notification_link;
        replace_all(content, from, to);

        if (uid != claim->uid)
        {
            add_notification(DB, uid, NtType::PostMention, pid, 0);
        }
    }

    Post post{
        pid,
        claim->uid,
        iid,
        input.title,
        tags,
        PostContent::Markdown(content),
        created_at,
        PostStatus::Normal};

    set_one(DB, "posts", pid, &post);

    std::vector<uint8_t> iid_ivec = u32_to_ivec(iid);
    std::vector<uint8_t> visibility_ivec = u32_to_ivec(visibility);
    if (old_pid == 0)
    {
        std::vector<uint8_t> k(iid_ivec.begin(), iid_ivec.end());
        k.insert(k.end(), pid_ivec.begin(), pid_ivec.end());
        DB.open_tree("inn_posts")->insert(k, std::vector<uint8_t>());

        std::vector<uint8_t> k2(u32_to_ivec(claim->uid).begin(), u32_to_ivec(claim->uid).end());
        k2.insert(k2.end(), pid_ivec.begin(), pid_ivec.end());
        std::vector<uint8_t> v(iid_ivec.begin(), iid_ivec.end());
        v.insert(v.end(), visibility_ivec.begin(), visibility_ivec.end());
        DB.open_tree("user_posts")->insert(k2, v);
    }

    if (old_pid > 0)
    {
        inn_rm_index(DB, iid, pid);
    }

    inn_add_index(DB, iid, pid, static_cast<uint32_t>(created_at), visibility);
    User::update_stats(DB, claim->uid, "post");
    claim->update_last_write(DB);

    if (inn.inn_type != "Private")
    {
        std::string key = "post" + std::to_string(pid);
        std::vector<uint8_t> value;
        if (old_pid == 0)
        {
            value = {};
        }
        else
        {
            value = {0};
        }
        DB.open_tree("tan")->insert(key, value);
    }

    std::string target = "/post/" + std::to_string(iid) + "/" + std::to_string(pid);
    return "Redirect::to(" + target + ")";
}

void clear_invalid(Database &db)
{
    std::shared_ptr<Database::Tree> tree = db.open_tree("user_stats");
    for (auto [key, value] : *tree)
    {
        std::string k_str = std::string(key.begin(), key.end());
        auto time_stamp = std::stoll(k_str);
        auto since_epoch = std::chrono::system_clock::now().time_since_epoch();
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
        auto now = secs.count();
        if (now - time_stamp > 0)
        {
            tree->remove(key);
        }
    }
}

void user_cron_job()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(600));
        try
        {
            clear_invalid(DB);
        }
        catch (const std::exception &e)
        {
            std::cout << "clear_invalid error: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(3600 * 4));
    }
}