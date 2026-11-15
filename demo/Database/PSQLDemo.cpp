#include "../../include/Grain.hpp"

using namespace Grain;

int main() {

    PSQL database;

    const auto error = database.connect(
        "host=localhost "
        "port=5432 "
        "dbname=oklab "
        "user=oklab "
        "password="
    );

    if (isError(error)) {
        return 1;
    }

    try {

        {
            auto tx = database.read();

            auto result = tx.exec(
                "SELECT uuid, name "
                "FROM uranus.organization"
            );

            for (std::size_t i = 0; i < result.size(); ++i) {
                const auto row = result[i];
                const auto uuid = row["uuid"].as<std::string>();
                const auto name = row["name"].as<std::string>();

                std::printf(
                    "UUID: %s | Name: %s\n",
                    uuid.c_str(),
                    name.c_str()
                );
            }

            tx.commit();
        }

        {
            auto tx = database.read();

            auto result = tx.exec(
                "SELECT uuid, name "
                "FROM uranus.organization "
                "WHERE uuid = $1",
                "019d2eed-a25a-7caf-a762-a2a71917c6a1"
            );

            tx.commit();
        }

        {
            auto tx = database.write();

            (void) tx.exec(
                "UPDATE uranus.organization "
                "SET name = $1 "
                "WHERE uuid = $2",
                "[SoundCodes~",
                "019d2eed-a25a-7caf-a762-a2a71917c6a1"
            );

            tx.commit();
        }

    }
    catch (const pqxx::sql_error& e) {
        std::fprintf(stderr, "SQL error: %s\n", e.what());
        return 1;
    }
    catch (const std::exception& e) {
        std::fprintf(stderr, "Database error: %s\n", e.what());
        return 1;
    }

    return 0;
}