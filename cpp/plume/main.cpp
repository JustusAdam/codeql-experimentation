#include <vector>
#include <string>
#include "plib/lib.hpp"

class User
{
    int _id;

public:
    const static std::string tableName;
    int id() const { return _id; }
    __attribute__((noinline)) bool deleteUser(Connection &conn);
    User(int _id) : _id(_id) {}
    bool deleteUserController(Connection &conn);
};

const std::string User::tableName = "users";

template <>
User fromRow(const Row &row)
{
    return User(row[0].getInt());
}

class Blog
{
    int _id;

public:
    const static std::string tableName;
    int id() const { return _id; }
    Blog(int _id) : _id(_id) {}
    static std::vector<Blog> findForAuthor(Connection &conn, const User &user)
    {
        return std::vector<Blog>{conn.findAny<Blog>(user.id())};
    }
    int countAuthors(Connection &conn) const { return conn.count(this->id()); }
    __attribute__((noinline)) void deleteBlog(Connection &conn) { conn.deleteAny(*this); }
};

const std::string Blog::tableName = "blogs";

template <>
Blog fromRow(const Row &row)
{
    return Blog(row[0].getInt());
}

class PostAuthor
{
    int _id;

public:
    const static std::string tableName;
    int id() const { return _id; }
    PostAuthor(int _id) : _id(_id) {}
    static std::vector<PostAuthor> getPostIdsForAuthor(Connection &conn, int authorId)
    {
        return std::vector{conn.findAny<PostAuthor>(authorId)};
    }
    static int countOtherAuthors(Connection &conn, int postId, int authorId) { return conn.count(postId); }
};

const std::string PostAuthor::tableName = "post_authors";

template <>
PostAuthor fromRow(const Row &row)
{
    return PostAuthor(row[0].getInt());
}

class Post
{
    int _id;

public:
    const static std::string tableName;
    int id() const { return _id; }
    Post(int _id) : _id(_id) {}
    static Post getPost(const Connection &conn, int postId) { return Post(conn.id); }
    __attribute__((noinline)) void deletePost(Connection &conn) { conn.deleteAny(*this); }
};

const std::string Post::tableName = "posts";

class Notification
{
    int _id;

public:
    const static std::string tableName;
    int id() const { return _id; }
    Notification(int _id) : _id(_id) {}
    static std::vector<Notification> findFollowedBy(Connection &conn, const User &user)
    {
        return std::vector{conn.findAny<Notification>(user.id())};
    }
    __attribute__((noinline)) void deleteNotification(Connection &conn) { conn.deleteAny(*this); }
};

const std::string Notification::tableName = "notifications";

template <>
Notification fromRow(const Row &row)
{
    return Notification(row[0].getInt());
}

class ActivityStream;

class Comment
{
    int _id;

public:
    const static std::string tableName;
    int id() const { return _id; }
    Comment(int _id) : _id(_id) {}
    static std::vector<Comment> listByAuthor(Connection &conn, int authorId)
    {
        return std::vector{conn.findAny<Comment>(authorId)};
    }
    __attribute__((noinline)) void deleteComment(Connection &conn) { conn.deleteAny(*this); }
    ActivityStream buildDeleteActivity(const Connection &conn);
};

const std::string Comment::tableName = "comments";

class Media
{
    int _id;

public:
    const static std::string tableName;
    int id() const { return _id; }
    Media(int _id) : _id(_id) {}
    static std::vector<Media> forUser(Connection &conn, int userId)
    {
        return std::vector{conn.findAny<Media>(userId)};
    }
    __attribute__((noinline)) void deleteMedia(Connection &conn) { conn.deleteAny(*this); }
};

template <>
Media fromRow(const Row &row)
{
    return Media(row[0].getInt());
}

const std::string Media::tableName = "media";

class ActivityStream
{
    int _id;

public:
    const static std::string tableName;
    int id() const { return _id; }
    ActivityStream(int _id) : _id(_id) {}
    std::string toJson() { return "{}"; }
};

const std::string ActivityStream::tableName = "activity_streams";

__attribute__((noinline)) bool User::deleteUser(Connection &conn)
{
    conn.deleteAny(*this);
    return true;
}

bool User::deleteUserController(Connection &conn)
{
    // Delete blogs where the user is the only author
    std::vector<Blog> blogs = Blog::findForAuthor(conn, *this);
    for (Blog &blog : blogs)
    {
        if (blog.countAuthors(conn) <= 1)
        {
            blog.deleteBlog(conn);
        }
    }

    // Delete posts where the user is the only author
    std::vector<PostAuthor> postIds = PostAuthor::getPostIdsForAuthor(conn, id());
    for (auto &postAuthor : postIds)
    {
        int otherAuthorsCount = PostAuthor::countOtherAuthors(conn, postAuthor.id(), id());
        if (otherAuthorsCount == 0)
        {
            Post post = Post::getPost(conn, postAuthor.id());
            post.deletePost(conn);
        }
    }

    // Delete notifications for the user
    std::vector<Notification> notifications = Notification::findFollowedBy(conn, *this);
    for (Notification &notification : notifications)
    {
        notification.deleteNotification(conn);
    }

    // // Delete comments by the user
    // std::vector<Comment> comments = Comment::listByAuthor(conn, id);
    // for (Comment& comment : comments) {
    //     comment.deleteComment(conn);
    // }

    // Delete media uploaded by the user
    std::vector<Media> media = Media::forUser(conn, id());
    for (Media &mediaItem : media)
    {
        mediaItem.deleteMedia(conn);
    }

    // Finally, delete the user record
    return conn.deleteAny(*this);
}

int main()
{
    return 0;
}