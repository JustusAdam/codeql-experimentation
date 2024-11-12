#include <optional>
#include <string>
#include <vector>
#include <set>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include <sstream>
#include <cctype>

#include "flib.hpp"

std::vector<uint8_t> u32_to_ivec(uint32_t value)
{
    std::vector<uint8_t> result(4);
    result[0] = (value >> 24) & 0xFF;
    result[1] = (value >> 16) & 0xFF;
    result[2] = (value >> 8) & 0xFF;
    result[3] = value & 0xFF;
    return result;
}
uint32_t u8_slice_to_u32(const std::vector<uint8_t> &slice)
{
    uint32_t result = 0;
    for (int i = 0; i < 4; i++)
    {
        result = (result << 8) | slice[i];
    }
    return result;
}

void Database::Tree::remove(const std::vector<uint8_t> &key) {}

void Database::Tree::insert(const std::string &key, const std::vector<uint8_t> &value) {}
void Database::Tree::insert(const std::vector<uint8_t> &key, const std::vector<uint8_t> &value) {}
void Database::Tree::apply_batch(const Batch &batch) {}
std::optional<std::vector<uint8_t>> Database::Tree::get(const std::vector<uint8_t> &key)
{
    return std::nullopt;
}
void Database::Tree::set(const std::vector<uint8_t> &key, const std::vector<uint8_t> &value) {}

tree_iter Database::Tree::begin()
{
    return tree_iter();
}

tree_iter Database::Tree::end()
{
    return tree_iter();
}

const tree_iter Database::Tree::begin() const
{
    return tree_iter();
}

const tree_iter Database::Tree::end() const
{
    return tree_iter();
}

std::shared_ptr<Database::Tree> Database::open_tree(const std::string &name)
{
    return std::make_shared<Database::Tree>();
}

void Batch::remove(const std::vector<uint8_t> &key) {}
void Batch::insert(const std::vector<uint8_t> &key, const std::vector<uint8_t> &value) {}

tree_iter::tree_iter() {}

std::pair<std::vector<uint8_t>, std::vector<uint8_t>> tree_iter::operator*()
{
    return std::make_pair(std::vector<uint8_t>(), std::vector<uint8_t>());
}

bool tree_iter::operator!=(const tree_iter &other) const
{
    return false;
}

bool tree_iter::operator==(const tree_iter &other) const
{
    return false;
}

tree_iter &tree_iter::operator++()
{
    return *this;
}
