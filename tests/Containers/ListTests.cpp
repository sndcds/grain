#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>

#include <grain/Containers/List.hpp>

namespace Grain::Tests {

namespace {

//------------------------------------------------------------------------------
// Test types
//------------------------------------------------------------------------------

struct NonDefaultConstructible {
    explicit NonDefaultConstructible(int value)
        : value(value) {
    }

    int value;
};

struct MoveOnly {
    explicit MoveOnly(int value)
        : value(value) {
    }

    MoveOnly(const MoveOnly&) = delete;

    MoveOnly& operator=(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&& other) noexcept
        : value(other.value) {
        other.value = -1;
    }

    MoveOnly& operator=(MoveOnly&& other) noexcept {
        if (this != &other) {
            value = other.value;
            other.value = -1;
        }

        return *this;
    }

    int value;
};

struct NonCopyable {
    explicit NonCopyable(int value)
        : value(value) {
    }

    NonCopyable(const NonCopyable&) = delete;

    NonCopyable& operator=(const NonCopyable&) = delete;

    NonCopyable(NonCopyable&&) noexcept = default;

    NonCopyable& operator=(NonCopyable&&) noexcept = default;

    int value;
};

struct ThrowingType {
    static int constructions_before_throw;

    explicit ThrowingType(int value)
        : value(value) {
        if (constructions_before_throw == 0) {
            throw std::runtime_error("construction failed");
        }

        --constructions_before_throw;
    }

    ThrowingType(const ThrowingType&) = default;

    ThrowingType(ThrowingType&&) noexcept = default;

    int value;
};

int ThrowingType::constructions_before_throw = 0;

struct LifetimeCounter {
    static int alive;
    static int constructed;
    static int destroyed;

    explicit LifetimeCounter(int value)
        : value(value) {
        ++alive;
        ++constructed;
    }

    LifetimeCounter(const LifetimeCounter& other)
        : value(other.value) {
        ++alive;
        ++constructed;
    }

    LifetimeCounter(LifetimeCounter&& other) noexcept
        : value(other.value) {
        ++alive;
        ++constructed;
        other.value = -1;
    }

    ~LifetimeCounter() {
        --alive;
        ++destroyed;
    }

    int value;

    static void reset() {
        alive = 0;
        constructed = 0;
        destroyed = 0;
    }
};

int LifetimeCounter::alive = 0;
int LifetimeCounter::constructed = 0;
int LifetimeCounter::destroyed = 0;

struct alignas(64) AlignedType {
    explicit AlignedType(int value)
        : value(value) {
    }

    int value;
};

} // namespace

//------------------------------------------------------------------------------
// int
//------------------------------------------------------------------------------

TEST_CASE("List stores integers") {
    List<int> list;

    REQUIRE(list.empty());
    REQUIRE(list.size() == 0);
    REQUIRE(list.capacity() == 0);

    list.push_back(10);
    list.push_back(20);
    list.push_back(30);

    REQUIRE(list.size() == 3);
    REQUIRE(list[0] == 10);
    REQUIRE(list[1] == 20);
    REQUIRE(list[2] == 30);
}

//------------------------------------------------------------------------------
// std::string
//------------------------------------------------------------------------------

TEST_CASE("List stores std::string") {
    List<std::string> list;

    list.emplace_back("Hello");
    list.push_back(std::string("Grain"));

    REQUIRE(list.size() == 2);
    REQUIRE(list[0] == "Hello");
    REQUIRE(list[1] == "Grain");
}

//------------------------------------------------------------------------------
// Non-default-constructible
//------------------------------------------------------------------------------

TEST_CASE("List supports non-default-constructible types") {
    List<NonDefaultConstructible> list;

    list.emplace_back(42);

    REQUIRE(list.size() == 1);
    REQUIRE(list[0].value == 42);
}

//------------------------------------------------------------------------------
// Move-only
//------------------------------------------------------------------------------

TEST_CASE("List supports move-only types") {
    List<MoveOnly> list;

    list.emplace_back(42);

    MoveOnly value(123);
    list.push_back(std::move(value));

    REQUIRE(list.size() == 2);
    REQUIRE(list[0].value == 42);
    REQUIRE(list[1].value == 123);
    REQUIRE(value.value == -1);
}

//------------------------------------------------------------------------------
// Non-copyable
//------------------------------------------------------------------------------

TEST_CASE("List supports non-copyable types") {
    List<NonCopyable> list;

    list.emplace_back(10);
    list.emplace_back(20);

    REQUIRE(list.size() == 2);
    REQUIRE(list[0].value == 10);
    REQUIRE(list[1].value == 20);

    static_assert(!std::copy_constructible<List<NonCopyable> >);
    static_assert(!std::is_copy_assignable_v<List<NonCopyable> >);
}

//------------------------------------------------------------------------------
// Throwing constructor
//------------------------------------------------------------------------------

TEST_CASE("List preserves state when emplace_back throws") {
    List<ThrowingType> list;

    ThrowingType::constructions_before_throw = 2;

    list.emplace_back(10);
    list.emplace_back(20);

    REQUIRE(list.size() == 2);
    REQUIRE(list[0].value == 10);
    REQUIRE(list[1].value == 20);

    REQUIRE_THROWS_AS(
        list.emplace_back(30),
        std::runtime_error
        );

    REQUIRE(list.size() == 2);
    REQUIRE(list[0].value == 10);
    REQUIRE(list[1].value == 20);
}

//------------------------------------------------------------------------------
// Copy construction
//------------------------------------------------------------------------------

TEST_CASE("List supports copy construction") {
    List<int> original;

    original.push_back(1);
    original.push_back(2);
    original.push_back(3);

    List<int> copy(original);

    REQUIRE(copy.size() == original.size());

    for (std::size_t i = 0; i < original.size(); ++i) {
        REQUIRE(copy[i] == original[i]);
    }

    copy[0] = 100;

    REQUIRE(original[0] == 1);
    REQUIRE(copy[0] == 100);
}

//------------------------------------------------------------------------------
// Copy assignment
//------------------------------------------------------------------------------

TEST_CASE("List supports copy assignment") {
    List<int> source;
    source.push_back(1);
    source.push_back(2);
    source.push_back(3);

    List<int> destination;
    destination.push_back(100);
    destination.push_back(200);

    destination = source;

    REQUIRE(destination.size() == 3);
    REQUIRE(destination[0] == 1);
    REQUIRE(destination[1] == 2);
    REQUIRE(destination[2] == 3);

    destination[0] = 999;

    REQUIRE(source[0] == 1);
}

//------------------------------------------------------------------------------
// Move construction
//------------------------------------------------------------------------------

TEST_CASE("List supports move construction") {
    List<std::string> source;

    source.emplace_back("one");
    source.emplace_back("two");

    const auto source_data = source.data();

    List<std::string> destination(std::move(source));

    REQUIRE(destination.size() == 2);
    REQUIRE(destination[0] == "one");
    REQUIRE(destination[1] == "two");

    REQUIRE(destination.data() == source_data);

    REQUIRE(source.empty());
    REQUIRE(source.data() == nullptr);
    REQUIRE(source.capacity() == 0);
}

//------------------------------------------------------------------------------
// Move assignment
//------------------------------------------------------------------------------

TEST_CASE("List supports move assignment") {
    List<std::string> source;

    source.emplace_back("one");
    source.emplace_back("two");

    const auto source_data = source.data();

    List<std::string> destination;

    destination.emplace_back("old");

    destination = std::move(source);

    REQUIRE(destination.size() == 2);
    REQUIRE(destination[0] == "one");
    REQUIRE(destination[1] == "two");

    REQUIRE(destination.data() == source_data);

    REQUIRE(source.empty());
    REQUIRE(source.data() == nullptr);
}

//------------------------------------------------------------------------------
// Reallocation
//------------------------------------------------------------------------------

TEST_CASE("List reallocates while preserving elements") {
    List<int> list;

    list.reserve(2);

    REQUIRE(list.capacity() == 2);

    list.push_back(1);
    list.push_back(2);

    const auto old_data = list.data();

    list.push_back(3);

    REQUIRE(list.size() == 3);
    REQUIRE(list.capacity() >= 3);

    REQUIRE(list.data() != old_data);

    REQUIRE(list[0] == 1);
    REQUIRE(list[1] == 2);
    REQUIRE(list[2] == 3);
}

//------------------------------------------------------------------------------
// clear
//------------------------------------------------------------------------------

TEST_CASE("List clear destroys all elements") {
    LifetimeCounter::reset();

    {
        List<LifetimeCounter> list;

        list.emplace_back(1);
        list.emplace_back(2);
        list.emplace_back(3);

        REQUIRE(LifetimeCounter::alive == 3);

        list.clear();

        REQUIRE(list.empty());
        REQUIRE(LifetimeCounter::alive == 0);
        REQUIRE(LifetimeCounter::destroyed == 3);
    }

    REQUIRE(LifetimeCounter::alive == 0);
}

//------------------------------------------------------------------------------
// pop_back
//------------------------------------------------------------------------------

TEST_CASE("List pop_back destroys the last element") {
    LifetimeCounter::reset();

    {
        List<LifetimeCounter> list;

        list.emplace_back(1);
        list.emplace_back(2);
        list.emplace_back(3);

        REQUIRE(LifetimeCounter::alive == 3);

        list.pop_back();

        REQUIRE(list.size() == 2);
        REQUIRE(list[0].value == 1);
        REQUIRE(list[1].value == 2);
        REQUIRE(LifetimeCounter::alive == 2);
        REQUIRE(LifetimeCounter::destroyed == 1);
    }

    REQUIRE(LifetimeCounter::alive == 0);
    REQUIRE(LifetimeCounter::destroyed == 3);
}

//------------------------------------------------------------------------------
// Range-for
//------------------------------------------------------------------------------

TEST_CASE("List supports range-for iteration") {
    List<int> list;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    int sum = 0;

    for (const int value : list) {
        sum += value;
    }

    REQUIRE(sum == 6);

    for (int& value : list) {
        value *= 2;
    }

    REQUIRE(list[0] == 2);
    REQUIRE(list[1] == 4);
    REQUIRE(list[2] == 6);
}

//------------------------------------------------------------------------------
// Alignment
//------------------------------------------------------------------------------

TEST_CASE("List correctly aligns over-aligned elements") {
    List<AlignedType> list;

    list.emplace_back(42);
    list.emplace_back(84);

    REQUIRE(
        reinterpret_cast<std::uintptr_t>(list.data()) %
        alignof(AlignedType) == 0
        );

    REQUIRE(
        reinterpret_cast<std::uintptr_t>(&list[0]) %
        alignof(AlignedType) == 0
        );

    REQUIRE(
        reinterpret_cast<std::uintptr_t>(&list[1]) %
        alignof(AlignedType) == 0
        );

    REQUIRE(list[0].value == 42);
    REQUIRE(list[1].value == 84);
}

//------------------------------------------------------------------------------
// Lifetime counting during reallocation
//------------------------------------------------------------------------------

TEST_CASE("List correctly handles lifetimes during reallocation") {
    LifetimeCounter::reset();

    {
        List<LifetimeCounter> list;

        list.reserve(1);

        list.emplace_back(1);

        REQUIRE(LifetimeCounter::alive == 1);

        list.emplace_back(2);

        REQUIRE(list.size() == 2);
        REQUIRE(LifetimeCounter::alive == 2);

        REQUIRE(list[0].value == 1);
        REQUIRE(list[1].value == 2);
    }

    REQUIRE(LifetimeCounter::alive == 0);
    REQUIRE(
        LifetimeCounter::constructed ==
        LifetimeCounter::destroyed
        );
}

} // namespace Grain::Tests