#include "lib.hpp"

Value::Value(const std::string &s) : v(s) {}

Value::Value(const int i) : v(i) {}
Value::Value() : v(nullptr) {}
Value::Value(const Value &other) : v(other.v) {}
Value &Value::operator=(const Value &other)
{
    v = other.v;
    return *this;
}
bool Value::operator==(const Value &other) const
{
    return v == other.v;
}

int Value::getInt() const
{
    return std::get<int>(v);
}
std::string Value::getString() const
{
    return std::get<std::string>(v);
}
bool Value::isNull() const
{
    return std::holds_alternative<std::nullptr_t>(v);
}

Connection::Connection() : id(1) {}
Row Connection::find(const TableName &table, const RowId id)
{
    return std::vector<Value>();
}
int Connection::count(const RowId t)
{
    return 0;
}
bool Connection::deleteRow(const TableName &table, const RowId id)
{
    return true;
}