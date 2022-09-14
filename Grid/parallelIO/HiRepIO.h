#include <sstream>

namespace Grid {
struct HiRepHeaderData {
  int N_gauge = 0;
};
class HiRepIO {
 public:
  static HiRepHeaderData readHeader(std::stringstream stream) {
    HiRepHeaderData header;
    header.N_gauge = 42;
    return header;
  };
};
}  // namespace Grid
