#include <optional>
#include <string>
#include <vector>
#include <set>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include <sstream>


// Struct for PostContent
struct PostContent {
    struct Markdown {
        std::string content;
        Markdown(const std::string& c) : content(c) {}
    };
    // Other potential variants...
};

// Forward declarations for classes/structs
template<typename C>
class TypedHeader {};
class Cookie {};
class FormPost {
public:
    std::string title;
    uint32_t iid;
    bool delete_draft;
    std::string tags;
    std::string content;
    bool is_draft;
};
class AppError {
private:
    std::string message;
public:
    AppError(const std::string& msg) : message(msg) {}
};
class SiteConfig;
class Claim;
class Inn {
public:
    std::string inn_type;
};
enum class PostStatus {
    Normal
};
class Post {
public:
    uint32_t pid;
    uint32_t uid;
    uint32_t iid;
    std::string title;
    std::vector<std::string> tags;
    PostContent::Markdown content;
    int64_t created_at;
    PostStatus status;
};
class User;
class Database;
// Method declarations for InnRole
enum class InnRole {
    Limited
};


// Enums
enum class NtType {
    PostMention,
};

// Forward declarations for functions
std::vector<uint8_t> u32_to_ivec(uint32_t value);
uint32_t u8_slice_to_u32(const std::vector<uint8_t>& slice);

template<typename T>
T get_one(const Database& db, const std::string& tree, uint32_t id);

template<typename T>
void set_one(Database& db, const std::string& tree, uint32_t id, const T& value);

void set_one_with_key(Database& db, const std::string& tree, const std::vector<uint8_t>& key, const void* value);

uint32_t incr_id(Database& db, const std::string& counter);

std::optional<uint32_t> get_id_by_name(const Database& db, const std::string& tree, const std::string& name);

void add_notification(Database& db, uint32_t uid, NtType type, uint32_t pid, uint32_t cid);

std::vector<std::string> extract_element(const std::string& content, size_t max_count, char delimiter);

// Batch class (assuming it's similar to a transaction or a set of operations)
class Batch {
public:
    void remove(const std::vector<uint8_t>& key);
    void insert(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value);
};

// Database class methods
class Database {
public:
    class Tree {
    public:
        void remove(const std::vector<uint8_t>& key);
        void insert(const std::string& key, const std::vector<uint8_t>& value);
        void insert(const std::vector<uint8_t>& key, const std::vector<uint8_t>& value);
        void apply_batch(const Batch& batch);
    };

    std::shared_ptr<Tree> open_tree(const std::string& name);
};

Database DB;

// Additional method declarations
class User {
public: 
    static void update_stats(Database& db, uint32_t uid, const std::string& action);
    static bool is_mod(const Database& db, uint32_t uid, uint32_t iid);
    static bool has_unread(const Database& db, uint32_t uid);
    std::string username;
};

// Method declarations for Claim
class Claim {
public:
    static std::optional<Claim> get(const Database& db, const TypedHeader<Cookie>& cookie, const SiteConfig& config);
    void update_last_write(Database& db);

    uint32_t uid;
    int64_t last_write;
};

// Method declarations for SiteConfig
class SiteConfig {
public:
    static SiteConfig get(const Database& db);
    int64_t post_interval;
    size_t per_page;
};


static std::optional<InnRole> get_inn_role(const Database& db, uint32_t iid, uint32_t uid);


// inn_add_index function
void inn_add_index(Database& DB, uint32_t iid, uint32_t pid, uint32_t created_at, uint32_t visibility) {
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
std::optional<AppError> inn_rm_index(Database& DB, uint32_t iid, uint32_t pid) {
    std::vector<uint8_t> iid_ivec = u32_to_ivec(iid);
    std::vector<uint8_t> pid_ivec = u32_to_ivec(pid);

    std::vector<uint8_t> k(iid_ivec.begin(), iid_ivec.end());
    k.insert(k.end(), pid_ivec.begin(), pid_ivec.end());

    try {
        DB.open_tree("inn_index")->remove(k);
        return std::nullopt;
    } catch (const std::exception& e) {
        return AppError("inn_rm_index error: " + std::string(e.what()));
    }
}

std::string trim(std::string input);

void replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}


// Function declaration
std::optional<std::string> edit_post_post(
    std::optional<TypedHeader<Cookie>> cookie,
    uint32_t old_pid,
    FormPost input
) {
    if (!cookie.has_value()) {
        throw AppError("non login");
    }

    SiteConfig site_config = SiteConfig::get(DB);
    std::optional<Claim> claim = Claim::get(DB, *cookie, site_config);
    if (!claim.has_value()) {
        throw AppError("non login");
    }

    bool is_draft = input.is_draft;
    bool delete_draft = input.delete_draft;

    std::vector<uint8_t> k;
    k.insert(k.end(), u32_to_ivec(claim->uid).begin(), u32_to_ivec(claim->uid).end());
    k.insert(k.end(), input.title.begin(), input.title.end());

    if (delete_draft) {
        DB.open_tree("drafts")->remove(k);
        return "Redirect::to(\"/post/edit/0\")";
    }
    if (is_draft) {
        set_one_with_key(DB, "drafts", k, &input);
        return "Redirect::to(\"/post/edit/0\")";
    }

    uint32_t iid = input.iid;
    std::optional<InnRole> inn_role = get_inn_role(DB, iid, claim->uid);
    if (!inn_role.has_value() || *inn_role <= InnRole::Limited) {
        throw AppError("unauthorized");
    }

    int64_t created_at = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (created_at - claim->last_write < site_config.post_interval) {
        throw AppError("write interval");
    }

    Inn inn = get_one<Inn>(DB, "inns", iid);

    uint32_t pid = (old_pid == 0) ? incr_id(DB, "posts_count") : old_pid;
    std::vector<uint8_t> pid_ivec = u32_to_ivec(pid);

    std::vector<std::string> tags;
    uint32_t visibility = 0;
    if (inn.inn_type == "Private") {
        visibility = 10;
    } else {
        std::set<std::string> tags_set;
        std::istringstream iss(input.tags);
        std::string tag;
        while (std::getline(iss, tag, '#')) {
            tag = trim(tag);
            if (!tag.empty()) {
                tags_set.insert(tag);
            }
        }

        tags.assign(tags_set.begin(), tags_set.end());
        if (tags.size() > 5) {
            tags.resize(5);
        }

        Batch batch;
        if (old_pid > 0) {
            Post post = get_one<Post>(DB, "posts", old_pid);
            if (post.uid != claim->uid) {
                throw AppError("unauthorized");
            }

            if (post.status != PostStatus::Normal) {
                throw AppError("locked or hidden");
            }

            if (post.iid != iid) {
                throw AppError("not found");
            }

            created_at = post.created_at;
            for (const auto& old_tag : post.tags) {
                std::vector<uint8_t> k(old_tag.begin(), old_tag.end());
                k.insert(k.end(), pid_ivec.begin(), pid_ivec.end());
                batch.remove(k);
            }
        }

        for (const auto& tag : tags) {
            std::vector<uint8_t> k(tag.begin(), tag.end());
            k.insert(k.end(), pid_ivec.begin(), pid_ivec.end());
            batch.insert(k, std::vector<uint8_t>());
        }
        DB.open_tree("tags")->apply_batch(batch);
    }

    std::string content = input.content;
    std::vector<std::string> notifications = extract_element(content, 5, '@');
    for (const auto& notification : notifications) {
        uint32_t uid;
        std::string username;
        try {
            uid = std::stoul(notification);
            User user = get_one<User>(DB, "users", uid);
            username = user.username;
        } catch (const std::invalid_argument&) {
            std::optional<uint32_t> uid_opt = get_id_by_name(DB, "usernames", notification);
            if (!uid_opt.has_value()) {
                continue;
            }
            uid = *uid_opt;
            username = notification;
        }

        std::string notification_link = "<span class='replytag'>[![](/static/avatars/" + std::to_string(uid) + ".png)" + username + "](/user/" + std::to_string(uid) + ")";
        std::string from = "@" + notification;
        std::string to = "@" + notification_link;
        replace_all(content, from, to);

        if (uid != claim->uid) {
            add_notification(DB, uid, NtType::PostMention, pid, 0);
        }
    }

    Post post {
        pid,
        claim->uid,
        iid,
        input.title,
        tags,
        PostContent::Markdown(content),
        created_at,
        PostStatus::Normal
    };

    set_one(DB, "posts", pid, &post);

    std::vector<uint8_t> iid_ivec = u32_to_ivec(iid);
    std::vector<uint8_t> visibility_ivec = u32_to_ivec(visibility);
    if (old_pid == 0) {
        std::vector<uint8_t> k(iid_ivec.begin(), iid_ivec.end());
        k.insert(k.end(), pid_ivec.begin(), pid_ivec.end());
        DB.open_tree("inn_posts")->insert(k, std::vector<uint8_t>());

        std::vector<uint8_t> k2(u32_to_ivec(claim->uid).begin(), u32_to_ivec(claim->uid).end());
        k2.insert(k2.end(), pid_ivec.begin(), pid_ivec.end());
        std::vector<uint8_t> v(iid_ivec.begin(), iid_ivec.end());
        v.insert(v.end(), visibility_ivec.begin(), visibility_ivec.end());
        DB.open_tree("user_posts")->insert(k2, v);
    }

    if (old_pid > 0) {
        inn_rm_index(DB, iid, pid);
    }

    inn_add_index(DB, iid, pid, static_cast<uint32_t>(created_at), visibility);
    User::update_stats(DB, claim->uid, "post");
    claim->update_last_write(DB);

    if (inn.inn_type != "Private") {
        std::string key = "post" + std::to_string(pid);
        std::vector<uint8_t> value;
        if (old_pid == 0) {
            value = {};
        } else {
            value = {0};
        }
        DB.open_tree("tan")->insert(key, value);
    }

    std::string target = "/post/" + std::to_string(iid) + "/" + std::to_string(pid);
    return "Redirect::to(" + target + ")";

}
