#include <sstream>

namespace Grid {
enum class endianness {
  little = 0,
  big = 1,
};

inline endianness system_endianess() {
  const int value{0x01};
  const void *address = static_cast<const void *>(&value);
  const unsigned char *least_significant_address =
      static_cast<const unsigned char *>(address);
  return (*least_significant_address == 0x01) ? endianness::little
                                              : endianness::big;
}

template <class T>
void endswap(T *objp) {
  unsigned char *memp = reinterpret_cast<unsigned char *>(objp);
  std::reverse(memp, memp + sizeof(T));
}

struct HiRepHeaderData {
  int N_gauge = 0;
};
class HiRepIO {
 public:
  static HiRepHeaderData readHeader(std::stringstream stream) {
    int n_gauge;
    stream.read(reinterpret_cast<char *>(&n_gauge), 4);
    if (system_endianess() == endianness::little) {
      endswap(&n_gauge);
    }
    HiRepHeaderData header;
    header.N_gauge = n_gauge;
    return header;
  };
};
}  // namespace Grid
