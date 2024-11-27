#include <string>

struct Identity
{
    std::string username;

public:
    std::string &identity() { return username; }
};

class Database
{
public:
    void delete_user(const std::string &username) {}
};

int start(Identity &id, Database &db)
{
    std::string username = id.identity();

    db.delete_user(username);

    return 0;
}
