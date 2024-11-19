#include <vector>
#include <unordered_map>
#include <mutex>
#include <cassert>
#include <memory>
#include <string>
#include <algorithm>

namespace mt
{

    template <typename T>
    struct lock_guard
    {
        std::mutex &m;
        T &t;
        bool needs_unlock;
        lock_guard(std::mutex &m, T &t) : m(m), t(t), needs_unlock(true)
        {
            m.lock();
        }
        ~lock_guard()
        {
            if (needs_unlock)
            {
                m.unlock();
            }
        }

        T &operator*()
        {
            assert(!needs_unlock);
            return t;
        }
        T *operator->()
        {
            assert(!needs_unlock);
            return &t;
        }

        void unlock()
        {
            m.unlock();
            needs_unlock = false;
        }
    };

    template <typename T>
    struct mutex
    {
        std::mutex m;
        T t;

    public:
        lock_guard<T> lock()
        {
            return lock_guard<T>(m, t);
        }
    };

} // namespace mt

namespace rocket
{
    template <typename T>
    class State
    {
    private:
        T value;

    public:
        T &operator*() { return value; }
        T *operator->() { return &value; }
        const T &operator*() const { return value; }
        const T *operator->() const { return &value; }
    };

    namespace request
    {
        template <typename T>
        class Form
        {
            T inner;

        public:
            T &operator*()
            {
                return this->inner;
            }

            T *operator->()
            {
                return &this->inner;
            }

            const T *operator->() const
            {
                return &this->inner;
            }
        };
    }
}

template <typename T>
using mutex = mt::mutex<T>;

class Value
{

public:
    enum Type
    {
        TIME,
        INT,
        STRING,
        EMPTY,
    };
    ~Value();

    const Type get_type() const;

    Value(uint64_t value);
    Value(std::string value);
    Value(std::chrono::system_clock::time_point value);
    Value(const Value &value);

    bool is_null() const;

    template <typename T>
    friend T from_value(const Value &value);

private:
    union value_t
    {
        uint64_t uint64_value;
        std::string string_value;
        value_t(uint64_t v) : uint64_value(v) {};
        value_t(std::string v) : string_value(v) {};
        ~value_t() {}
    } value;

    Type type;
};

namespace mysql
{
    class result_iterator
    {
        using elem_ty = std::vector<Value>;
        elem_ty *pos;
        result_iterator(elem_ty *pos);

    public:
        bool operator!=(const result_iterator &other);
        result_iterator &operator++();
        elem_ty &operator*();
    };

    class Result
    {
        std::vector<std::vector<Value>> values;

    public:
        result_iterator begin();
        result_iterator end();
    };
    class Connection
    {
    public:
        bool ping();
        void query_drop(const std::string &query);
        class Statement prepare(const std::string &query);
        class Result execute(const std::string &query, const std::vector<Value> &params);
    };
    class Pool
    {
    public:
        Pool();
        Pool(const std::string &connection_string);
        class Connection *get_conn();
    };
}

namespace backend
{

    class MySqlBackend
    {
        mysql::Pool pool_;

    public:
        void do_insert(const std::string &table, const std::vector<Value> &vals, bool replace)
        {
            std::string op = replace ? "REPLACE" : "INSERT";
            std::string placeholders(vals.size(), '?');
            std::replace(placeholders.begin(), placeholders.end(), '?', ',');
            placeholders.pop_back();
            std::string query = op + " INTO " + table + " VALUES (" + placeholders + ")";

            auto conn = pool_.get_conn();
            conn->execute(query, vals);
        }

        void insert(const std::string &table, const std::vector<Value> &vals)
        {
            do_insert(table, vals, false);
        }

        void replace(const std::string &table, const std::vector<Value> &vals)
        {
            do_insert(table, vals, true);
        }
    };

}

class LectureQuestionSubmission
{
public:
    std::unordered_map<int, int> answers;
};

void controller(
    const rocket::request::Form<LectureQuestionSubmission> &data,
    backend::MySqlBackend &bg)
{

    bg.replace("answers", std::vector<Value>());

    // Destructuring assignment seems to trigger an empty for loop body.
    //
    // for (const auto &[id, elem] : data->answers)
    // {
    //     auto rec = std::vector<Value>{Value(id), Value(elem)};
    //     bg.replace("answers", rec);
    // }
    for (const auto &elem : data->answers)
    {
        auto rec = std::vector<Value>{Value(elem.first), Value(elem.second)};
        bg.replace("answers", rec);
    }
}