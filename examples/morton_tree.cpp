#include <naiades/spatial/morton_tree.h>

namespace na = naiades;

int main() {
  auto mt = *na::spatial::MortonTree2::fromMaxLevel(2);
  HERMES_LOG_VARIABLE(mt);
  mt.refine([](const auto &leaf) -> bool {
    HERMES_LOG_VARIABLE(leaf.bounds.lower());
    HERMES_LOG_VARIABLE(leaf.bounds.upper());
    HERMES_LOG_VARIABLE(leaf.level);

    return true;
  });
  return 0;
}
