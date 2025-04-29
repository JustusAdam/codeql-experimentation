
#include <vector>
#include <string>
#include <variant>

typedef std::string TableName;
typedef uint32_t RowId;

class Value
{
    std::variant<std::string, int, std::nullptr_t> v;

public:
    Value(const std::string &s);
    Value(const int i);
    Value();
    Value(const Value &other);
    Value &operator=(const Value &other);
    bool operator==(const Value &other) const;
    int getInt() const;
    std::string getString() const;
    bool isNull() const;
};

typedef std::vector<Value> Row;

template <typename T>
T fromRow(const Row &row);

class Connection
{
public:
    int id;
    Connection();
    Row find(const TableName &table, const RowId id);
    int count(const RowId t);
    bool deleteRow(const TableName &table, const RowId id);

    template <typename T>
    T findAny(const RowId id)
    {
        Row row = find(T::tableName, id);
        return fromRow<T>(row);
    }

    template <typename T>
    bool deleteAny(T &t)
    {
        return deleteRow(T::tableName, t.id());
    }
};