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
  auto svg = na::utils::io::SVG().setTextSize(11).setPointSize(3.0).draw(mt);

  for (auto leaf : mt) {
    auto stencil = mt.stencil(leaf.z_index);
    auto center = mt.cellBounds(leaf.z_index).center();
    if (leaf.z_index == 192)
      for (auto nid : stencil) {
        auto n_center = mt.cellBounds(nid).center();
        svg.link(center, n_center, hermes::colors::RGB_Color::Purple());
      }
  }

  svg.write("morton_tree.svg");

  return 0;
}
