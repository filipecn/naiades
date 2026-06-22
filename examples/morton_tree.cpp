#include <naiades/spatial/morton_tree.h>
#include <naiades/utils/io.h>

#include <random>

namespace na = naiades;

int main() {
  // Seed generator with a hardware-based random device
  std::random_device rd;
  // Initialize the Mersenne Twister engine
  std::mt19937 gen(rd());

  auto mt = *na::spatial::MortonTree2::fromMaxLevel(4);
  HERMES_LOG_VARIABLE(mt);
  h_index call = 0;
  mt.refine([&](const auto &leaf) -> bool {
    // Define an even, uniform distribution between 1 and 100 inclusive
    std::uniform_int_distribution<int> distrib(1, 100);

    return distrib(gen) < 70;
  });
  HERMES_LOG_VARIABLE(mt);
  na::utils::io::SVG("morton_tree.svg")
      .setDimensions(mt.bounds())
      .setTextSize(11)
      .setPointSize(1)
      .draw(mt)
      .write();

  return 0;
}
