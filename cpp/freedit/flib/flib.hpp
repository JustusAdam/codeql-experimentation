
#include <optional>
#include <string>
#include <vector>
#include <set>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include <sstream>
#include <cctype>

std::vector<uint8_t> u32_to_ivec(uint32_t value);
uint32_t u8_slice_to_u32(const std::vector<uint8_t> &slice);

class Batch
{
public:
    void remove(const std::vector<uint8_t> &key);
    void insert(const std::vector<uint8_t> &key, const std::vector<uint8_t> &value);
};

class tree_iter;

class Database
{
public:
    class Tree
    {
    public:
        void remove(const std::vector<uint8_t> &key);
        void insert(const std::string &key, const std::vector<uint8_t> &value);
        void insert(const std::vector<uint8_t> &key, const std::vector<uint8_t> &value);
        void apply_batch(const Batch &batch);
        void set(const std::vector<uint8_t> &key, const std::vector<uint8_t> &value);
        std::optional<std::vector<uint8_t>> get(const std::vector<uint8_t> &key);
        template <typename T>
        std::optional<std::vector<uint8_t>> update_and_fetch(const std::vector<uint8_t> &key, T predicate)
        {
            auto current_value = get(key);
            auto new_value = predicate(current_value);
            if (new_value)
            {
                set(key, new_value.value());
            }
            else
            {
                remove(key);
            }
            return new_value;
        }
        tree_iter begin();
        tree_iter end();
        const tree_iter begin() const;
        const tree_iter end() const;
    };

    std::shared_ptr<Tree> open_tree(const std::string &name);
};

class tree_iter
{
public:
    bool operator==(const tree_iter &other) const;
    bool operator!=(const tree_iter &other) const;
    tree_iter &operator++();
    std::pair<std::vector<uint8_t>, std::vector<uint8_t>> operator*();

    friend class Database::Tree;

private:
    tree_iter();
};

template <typename T>
std::optional<T> deserialize(const std::vector<uint8_t> &data)
{
    if (data.size() < sizeof(T))
    {
        return std::nullopt;
    }
    T value;
    std::memcpy(&value, data.data(), sizeof(T));
    return value;
}

template <typename T>
std::vector<uint8_t> serialize(const T &value)
{
    std::vector<uint8_t> data(sizeof(T));
    std::memcpy(data.data(), &value, sizeof(T));
    return data;
}
