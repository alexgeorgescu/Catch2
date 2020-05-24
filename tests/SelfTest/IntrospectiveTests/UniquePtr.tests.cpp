#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>

TEST_CASE("Basic compilation test", "[internals][unique-ptr]") {
    using Catch::Detail::unique_ptr;
    SECTION("Default constructed unique_ptr is empty") {
        unique_ptr<int> ptr;
        REQUIRE_FALSE(ptr);
        REQUIRE(ptr.get() == nullptr);
    }
    SECTION("Basic behaviour") {
        auto naked_ptr = new int{0};
        unique_ptr<int> ptr2(naked_ptr);
        REQUIRE(ptr2);
        REQUIRE(*ptr2 == 0);
        REQUIRE(ptr2.get() == naked_ptr);
        ptr2.reset(); // this makes naked_ptr dangling!
        REQUIRE_FALSE(ptr2);
        REQUIRE(ptr2.get() == nullptr);
        ptr2.reset(new int{ 2 });
        REQUIRE(ptr2);
        REQUIRE(ptr2.get() != nullptr);
        REQUIRE(*ptr2 == 2);
    }
    SECTION("Release releases ownership") {
        auto naked_ptr = new int{ 1 };
        unique_ptr<int> ptr(naked_ptr);
        ptr.release();
        CHECK_FALSE(ptr);
        delete naked_ptr;
    }

    // Potential todos:
    // * move ops
    // * op ->
    // * various const mutations?
    // * Actually


    //Catch::Detail::unique_ptr<int[]> arrptr;
    //(void)arrptr;
}
