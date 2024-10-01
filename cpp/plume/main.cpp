#include <vector>
#include <string>

class Connection;

class User {
public:
    int id;
    __attribute__((noinline)) bool deleteUser(Connection& conn);
    User(int _id) : id(_id) {}
    bool deleteUserController(Connection& conn);
};

class Connection {
public:
    int id;
    Connection() : id(1) {}
    bool deleteUser(const User& user) { return true; }
    template<typename T> 
    std::vector<T> findAny(const int id) const {
        std::vector<T> v;
        v.emplace_back(id + this->id);
        return v;
    }

    int countAny(const int t) const {
        return t + this->id;
    }

    template<typename T> void deleteAny(const T& t) {}
};

class Blog {
public:
    int id;
    Blog(int _id) : id(_id) {}
    static std::vector<Blog> findForAuthor(const Connection& conn, const User& user) {
        return conn.findAny<Blog>(user.id);
    }
    int countAuthors(const Connection& conn) const { return conn.countAny(this->id); }
    __attribute__((noinline)) void deleteBlog(Connection& conn) { conn.deleteAny(*this); }
};

class PostAuthor {
public:
    int id;
    PostAuthor(int _id) : id(_id) {}
    static std::vector<int> getPostIdsForAuthor(const Connection& conn, int authorId) {
        return conn.findAny<int>(authorId);
    }
    static int countOtherAuthors(const Connection& conn, int postId, int authorId) { return conn.countAny(postId); }
};

class Post {
public:
    int id;
    Post(int _id) : id(_id) {}
    static Post getPost(const Connection& conn, int postId) { return Post(conn.id); }
    __attribute__((noinline)) void deletePost(Connection& conn) { conn.deleteAny(*this); }
};

class Notification {
public:
    int id;
    Notification(int _id) : id(_id) {}
    static std::vector<Notification> findFollowedBy(const Connection& conn, const User& user) {
        return conn.findAny<Notification>(user.id);
    }
    __attribute__((noinline)) void deleteNotification(Connection& conn) { conn.deleteAny(*this); }
};

class ActivityStream;

class Comment {
public:
    int id;
    Comment(int _id) : id(_id) {}
    static std::vector<Comment> listByAuthor(const Connection& conn, int authorId) {
        return conn.findAny<Comment>(authorId);
    }
    __attribute__((noinline)) void deleteComment(Connection& conn) { conn.deleteAny(*this); }
    ActivityStream buildDeleteActivity(const Connection& conn);
};

class Media {
public:
    int id;
    Media(int _id) : id(_id) {}
    static std::vector<Media> forUser(const Connection& conn, int userId) {
        return conn.findAny<Media>(userId);
    }
    __attribute__((noinline)) void deleteMedia(Connection& conn) { conn.deleteAny(*this); }
};

class ActivityStream {
public:
    int id;
    ActivityStream(int _id) : id(_id) {}
    std::string toJson() { return "{}"; }
};

__attribute__((noinline)) bool User::deleteUser(Connection& conn) {
    conn.deleteAny(*this);
    return true;
}

bool User::deleteUserController(Connection& conn) {
    // Delete blogs where the user is the only author
    std::vector<Blog> blogs = Blog::findForAuthor(conn, *this);
    for (Blog& blog : blogs) {
        if (blog.countAuthors(conn) <= 1) {
            blog.deleteBlog(conn);
        }
    }

    // Delete posts where the user is the only author
    std::vector<int> postIds = PostAuthor::getPostIdsForAuthor(conn, id);
    for (int postId : postIds) {
        int otherAuthorsCount = PostAuthor::countOtherAuthors(conn, postId, id);
        if (otherAuthorsCount == 0) {
            Post post = Post::getPost(conn, postId);
            post.deletePost(conn);
        }
    }

    // Delete notifications for the user
    std::vector<Notification> notifications = Notification::findFollowedBy(conn, *this);
    for (Notification& notification : notifications) {
        notification.deleteNotification(conn);
    }

    // // Delete comments by the user
    // std::vector<Comment> comments = Comment::listByAuthor(conn, id);
    // for (Comment& comment : comments) {
    //     comment.deleteComment(conn);
    // }

    // Delete media uploaded by the user
    std::vector<Media> media = Media::forUser(conn, id);
    for (Media& mediaItem : media) {
        mediaItem.deleteMedia(conn);
    }

    // Finally, delete the user record
    return conn.deleteUser(*this);
}

int main() {
    return 0;
}