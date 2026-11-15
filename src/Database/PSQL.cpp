#include <grain/Database/PSQL.hpp>

#include <stdexcept>
#include <utility>


namespace Grain {


namespace {


//------------------------------------------------------------------------------
//  PostgreSQL type mapping
//------------------------------------------------------------------------------

[[nodiscard]]
auto _toFieldType_(pqxx::oid oid) noexcept -> PSQLFieldType {

    switch (oid) {

        case 16:   return PSQLFieldType::Boolean;
        case 17:   return PSQLFieldType::Bytea;
        case 18:   return PSQLFieldType::Character;
        case 19:   return PSQLFieldType::Name;

        case 20:   return PSQLFieldType::Int64;
        case 21:   return PSQLFieldType::Int16;
        case 23:   return PSQLFieldType::Int32;
        case 26:   return PSQLFieldType::Oid;

        case 25:   return PSQLFieldType::Text;
        case 1043: return PSQLFieldType::Varchar;

        case 114:  return PSQLFieldType::Json;
        case 3802: return PSQLFieldType::Jsonb;

        case 700:  return PSQLFieldType::Float32;
        case 701:  return PSQLFieldType::Float64;
        case 1700: return PSQLFieldType::Numeric;

        case 1082: return PSQLFieldType::Date;
        case 1083: return PSQLFieldType::Time;
        case 1114: return PSQLFieldType::Timestamp;
        case 1184: return PSQLFieldType::TimestampTz;
        case 1186: return PSQLFieldType::Interval;

        case 2950: return PSQLFieldType::Uuid;

        default:   return PSQLFieldType::Unknown;
    }
}


} // namespace


//------------------------------------------------------------------------------
//  PSQLField
//------------------------------------------------------------------------------

PSQLField::PSQLField(pqxx::field_ref field)
    : field_(field) {
}


auto PSQLField::isNull() const noexcept -> bool {
    return field_.is_null();
}


auto PSQLField::c_str() const noexcept -> const char* {
    return field_.c_str();
}


auto PSQLField::name() const -> std::string {
    return std::string(field_.name());
}


auto PSQLField::type() const noexcept -> PSQLFieldType {
    return _toFieldType_(field_.type());
}


auto PSQLField::typeOid() const noexcept -> pqxx::oid {
    return field_.type();
}


auto PSQLField::size() const noexcept -> std::size_t {
    return static_cast<std::size_t>(field_.size());
}


//------------------------------------------------------------------------------
//  PSQLRow
//------------------------------------------------------------------------------

PSQLRow::PSQLRow(pqxx::row_ref row)
    : row_(row) {
}


auto PSQLRow::size() const noexcept -> std::size_t {
    return static_cast<std::size_t>(row_.size());
}


auto PSQLRow::empty() const noexcept -> bool {
    return row_.size() == 0;
}


auto PSQLRow::operator[](std::size_t index) const -> PSQLField {
    return PSQLField(row_[index]);
}


auto PSQLRow::operator[](const char* name) const -> PSQLField {
    return PSQLField(row_[name]);
}


//------------------------------------------------------------------------------
//  PSQLTxResult
//------------------------------------------------------------------------------

PSQLTxResult::PSQLTxResult(pqxx::result&& result)
    : result_(std::move(result)) {
}


auto PSQLTxResult::size() const noexcept -> std::size_t {
    return static_cast<std::size_t>(result_.size());
}


auto PSQLTxResult::empty() const noexcept -> bool {
    return result_.empty();
}


auto PSQLTxResult::columns() const noexcept -> std::size_t {
    return static_cast<std::size_t>(result_.columns());
}


auto PSQLTxResult::columnName(std::size_t index) const -> std::string {
    return std::string(
        result_.column_name(static_cast<int>(index))
    );
}


auto PSQLTxResult::columnType(
    std::size_t index
) const noexcept -> PSQLFieldType {

    return _toFieldType_(
        result_.column_type(static_cast<int>(index))
    );
}


auto PSQLTxResult::columnTypeOid(
    std::size_t index
) const noexcept -> pqxx::oid {

    return result_.column_type(static_cast<int>(index));
}


auto PSQLTxResult::operator[](std::size_t index) const -> PSQLRow {
    return PSQLRow(result_[index]);
}


//------------------------------------------------------------------------------
//  PSQLReadTx
//------------------------------------------------------------------------------

PSQLReadTx::PSQLReadTx(pqxx::connection& connection)
    : transaction_(
        std::make_unique<pqxx::read_transaction>(connection)
    ) {
}


auto PSQLReadTx::exec(
    const std::string& query
) -> PSQLTxResult {

    return PSQLTxResult(transaction_->exec(query));
}


void PSQLReadTx::commit() {
    transaction_->commit();
}


//------------------------------------------------------------------------------
//  PSQLWriteTx
//------------------------------------------------------------------------------

PSQLWriteTx::PSQLWriteTx(pqxx::connection& connection)
    : transaction_(
        std::make_unique<pqxx::work>(connection)
    ) {
}

auto PSQLWriteTx::exec(
    const std::string& query
) -> PSQLTxResult {

    return PSQLTxResult(transaction_->exec(query));
}


void PSQLWriteTx::commit() {
    transaction_->commit();
}


//------------------------------------------------------------------------------
//  PSQL
//------------------------------------------------------------------------------

auto PSQL::connect(
    const std::string& connectionString
) -> PSQLError {

    disconnect();

    try {

        connection_ = std::make_unique<pqxx::connection>(
            connectionString
        );

        if (!connection_->is_open()) {
            connection_.reset();
            return PSQLError::ConnectionFailed;
        }

    } catch (...) {

        connection_.reset();
        return PSQLError::ConnectionFailed;
    }

    return PSQLError::None;
}


void PSQL::disconnect() noexcept {
    connection_.reset();
}


auto PSQL::isConnected() const noexcept -> bool {
    return connection_ && connection_->is_open();
}


auto PSQL::connection() -> pqxx::connection& {

    if (!isConnected()) {
        throw std::logic_error(
            "PSQL: connection is not open."
        );
    }

    return *connection_;
}


auto PSQL::read() -> PSQLReadTx {
    return PSQLReadTx(connection());
}


auto PSQL::write() -> PSQLWriteTx {
    return PSQLWriteTx(connection());
}


auto PSQL::transaction() -> PSQLReadWriteTx {
    return PSQLReadWriteTx(connection());
}


} // namespace Grain
