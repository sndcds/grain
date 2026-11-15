#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include <pqxx/pqxx>


namespace Grain {


//------------------------------------------------------------------------------
//  Error
//------------------------------------------------------------------------------

enum class PSQLError : uint8_t {
    None = 0,
    ConnectionFailed
};


[[nodiscard]]
constexpr auto isError(PSQLError error) noexcept -> bool {
    return error != PSQLError::None;
}


//------------------------------------------------------------------------------
//  Field types
//------------------------------------------------------------------------------

enum class PSQLFieldType : uint8_t {
    Unknown = 0,

    Boolean,
    Bytea,
    Character,
    Name,

    Int16,
    Int32,
    Int64,
    Oid,

    Text,
    Varchar,

    Json,
    Jsonb,

    Float32,
    Float64,
    Numeric,

    Date,
    Time,
    Timestamp,
    TimestampTz,
    Interval,

    Uuid
};


//------------------------------------------------------------------------------
//  PSQLField
//------------------------------------------------------------------------------

class PSQLField {
public:
    explicit PSQLField(pqxx::field_ref field);

    [[nodiscard]]
    auto isNull() const noexcept -> bool;

    [[nodiscard]]
    auto c_str() const noexcept -> const char*;

    [[nodiscard]]
    auto name() const -> std::string;

    [[nodiscard]]
    auto type() const noexcept -> PSQLFieldType;

    [[nodiscard]]
    auto typeOid() const noexcept -> pqxx::oid;

    [[nodiscard]]
    auto size() const noexcept -> std::size_t;

    template<typename T>
    [[nodiscard]]
    auto as() const -> T {
        return field_.as<T>();
    }

private:
    pqxx::field_ref field_;
};


//------------------------------------------------------------------------------
//  PSQLRow
//------------------------------------------------------------------------------

class PSQLRow {
public:
    explicit PSQLRow(pqxx::row_ref row);

    [[nodiscard]]
    auto size() const noexcept -> std::size_t;

    [[nodiscard]]
    auto empty() const noexcept -> bool;

    [[nodiscard]]
    auto operator[](std::size_t index) const -> PSQLField;

    [[nodiscard]]
    auto operator[](const char* name) const -> PSQLField;

private:
    pqxx::row_ref row_;
};


//------------------------------------------------------------------------------
//  PSQLTxResult
//------------------------------------------------------------------------------

class PSQLTxResult {
public:
    PSQLTxResult() = default;
    explicit PSQLTxResult(pqxx::result&& result);

    PSQLTxResult(const PSQLTxResult&) = delete;
    auto operator=(const PSQLTxResult&) -> PSQLTxResult& = delete;

    PSQLTxResult(PSQLTxResult&&) noexcept = default;
    auto operator=(PSQLTxResult&&) noexcept -> PSQLTxResult& = default;

    [[nodiscard]]
    auto size() const noexcept -> std::size_t;

    [[nodiscard]]
    auto empty() const noexcept -> bool;

    [[nodiscard]]
    auto columns() const noexcept -> std::size_t;

    [[nodiscard]]
    auto columnName(std::size_t index) const -> std::string;

    [[nodiscard]]
    auto columnType(std::size_t index) const noexcept -> PSQLFieldType;

    [[nodiscard]]
    auto columnTypeOid(std::size_t index) const noexcept -> pqxx::oid;

    [[nodiscard]]
    auto operator[](std::size_t index) const -> PSQLRow;

private:
    pqxx::result result_;
};


//------------------------------------------------------------------------------
//  PSQLReadTx
//------------------------------------------------------------------------------

class PSQLReadTx {
public:
    explicit PSQLReadTx(pqxx::connection& connection);

    ~PSQLReadTx() = default;

    // No copying
    PSQLReadTx(const PSQLReadTx&) = delete;
    auto operator=(const PSQLReadTx&) -> PSQLReadTx& = delete;

    // Allow moving
    PSQLReadTx(PSQLReadTx&&) noexcept = default;
    auto operator=(PSQLReadTx&&) noexcept -> PSQLReadTx& = default;


    [[nodiscard]]
    auto exec(const std::string& query) -> PSQLTxResult;

    template<typename... Args>
        requires (sizeof...(Args) > 0)
    [[nodiscard]]
    auto exec(
        const std::string& query,
        Args&&... args
    ) -> PSQLTxResult {
        return PSQLTxResult(
            transaction_->exec_params(
                query,
                std::forward<Args>(args)...
            )
        );
    }

    void commit();

private:
    std::unique_ptr<pqxx::read_transaction> transaction_;
};


//------------------------------------------------------------------------------
//  PSQLWriteTx
//------------------------------------------------------------------------------

class PSQLWriteTx {
public:
    explicit PSQLWriteTx(pqxx::connection& connection);

    ~PSQLWriteTx() = default;

    // No copying
    PSQLWriteTx(const PSQLWriteTx&) = delete;
    auto operator=(const PSQLWriteTx&) -> PSQLWriteTx& = delete;

    // Allow moving
    PSQLWriteTx(PSQLWriteTx&&) noexcept = default;
    auto operator=(PSQLWriteTx&&) noexcept -> PSQLWriteTx& = default;

    [[nodiscard]]
    auto exec(const std::string& query) -> PSQLTxResult;

    template<typename... Args>
        requires (sizeof...(Args) > 0)
    [[nodiscard]]
    auto exec(
        const std::string& query,
        Args&&... args
    ) -> PSQLTxResult {
        return PSQLTxResult(
            transaction_->exec_params(
                query,
                std::forward<Args>(args)...
            )
        );
    }

    void commit();

private:
    std::unique_ptr<pqxx::work> transaction_;
};


// Read-write transactions use the same implementation as write transactions.
using PSQLReadWriteTx = PSQLWriteTx;


//------------------------------------------------------------------------------
//  PSQL
//------------------------------------------------------------------------------

class PSQL {
public:
    PSQL() = default;
    ~PSQL() = default;

    PSQL(const PSQL&) = delete;
    auto operator=(const PSQL&) -> PSQL& = delete;

    PSQL(PSQL&&) noexcept = default;
    auto operator=(PSQL&&) noexcept -> PSQL& = default;

    [[nodiscard]]
    auto connect(const std::string& connectionString) -> PSQLError;

    void disconnect() noexcept;

    [[nodiscard]]
    auto isConnected() const noexcept -> bool;

    [[nodiscard]]
    auto connection() -> pqxx::connection&;

    [[nodiscard]]
    auto read() -> PSQLReadTx;

    [[nodiscard]]
    auto write() -> PSQLWriteTx;

    [[nodiscard]]
    auto transaction() -> PSQLReadWriteTx;

private:
    std::unique_ptr<pqxx::connection> connection_;
};


} // namespace Grain
