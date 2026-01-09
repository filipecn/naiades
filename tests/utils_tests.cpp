#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <naiades/utils/utils.h>

using namespace naiades;
using namespace naiades::utils;

TEST_CASE("IndexSet", "[core]") {
  SECTION("sanity") {
    IndexSet set({10, 11, 12, 13, 14, 15, 16, 17, 18, 19});
    REQUIRE(set.size() == 10);
    for (h_size i = 0; i < 10; ++i) {
      REQUIRE(set.contains(core::Index::local(i)));
      REQUIRE(!set.contains(core::Index::global(i)));
      REQUIRE(set.contains(core::Index::global(i + 10)));
      REQUIRE(set[i] == i + 10);
    }
  }
  SECTION("holes") {
    std::vector<h_size> seq{10, 11, 12, 15, 17, 18, 19};
    IndexSet set(seq);
    REQUIRE(set.size() == 7);
    for (h_size i = 0; i < 7; ++i) {
      REQUIRE(set.contains(core::Index::global(seq[i])));
      REQUIRE(set[i] == seq[i]);
    }
    REQUIRE(!set.contains(core::Index::global(13)));
    REQUIRE(!set.contains(core::Index::global(14)));
    REQUIRE(!set.contains(core::Index::global(16)));
  }
  SECTION("iterator") {
    std::vector<h_size> seq{10, 11, 12, 15, 17, 18, 19};
    IndexSet set(seq);
    h_size i = 0;
    for (auto it : set) {
      REQUIRE(i == it.flat_index);
      REQUIRE(seq[i] == it.index);
      i++;
    }
  }
}
