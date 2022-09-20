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
  int t = 0;
  int x = 0;
  int y = 0;
  int z = 0;
  double Plaquette = 0.;
};

class HiRepIO {
 public:
  static HiRepHeaderData readHeader(std::stringstream stream) {
    int n_gauge;
    int t;
    int x;
    int y;
    int z;
    double plaquette;

    stream.read(reinterpret_cast<char *>(&n_gauge), 4);
    if (system_endianess() == endianness::little) {
      endswap(&n_gauge);
      //endswap(&n_t);
      //endswap(&n_x);
      //endswap(&n_y);
      //endswap(&n_z);
      //endswap(&plaquette);
    }
    HiRepHeaderData header;
    header.N_gauge = n_gauge;
    header.t = t;
    header.x = x;
    header.y = y;
    header.z = z;
    header.plaquette = Plaquette;
    return header;
  };
};
}  // namespace Grid
