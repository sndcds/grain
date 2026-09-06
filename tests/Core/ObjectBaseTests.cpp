#include <catch2/catch_test_macros.hpp>

#include <grain/Core/ObjectBase.hpp>

namespace Grain::Tests {

class TestObject final : public ObjectBase {
};

TEST_CASE("ObjectBase creates unique object IDs") {
    const TestObject a;
    const TestObject b;

    REQUIRE(a.id() != b.id());
}

TEST_CASE("ObjectBase can be used polymorphically") {
    const TestObject object;

    const ObjectBase* base = &object;

    REQUIRE(base->id() == object.id());
}

} // namespace Grain::Tests