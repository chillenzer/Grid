#include <sstream>

#include "Grid/Grid.h"

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
  double plaquette = 0.;
  int N_gauge = 0;
  int t = 0;
  int x = 0;
  int y = 0;
  int z = 0;
};

class HiRepIO {
 public:
  static HiRepHeaderData readHeader(std::stringstream &stream) {
    HiRepHeaderData header;
    header.N_gauge = read_with_correct_endianess<int>(stream);
    header.t = read_with_correct_endianess<int>(stream);
    header.x = read_with_correct_endianess<int>(stream);
    header.y = read_with_correct_endianess<int>(stream);
    header.z = read_with_correct_endianess<int>(stream);
    header.plaquette = read_with_correct_endianess<double>(stream);
    return header;
  };

 private:
  template <typename T>
  static T read_with_correct_endianess(std::stringstream &stream) {
    T storage;
    stream.read(reinterpret_cast<char *>(&storage), sizeof(T));
    if (system_endianess() == endianness::little) {
      endswap(&storage);
    }
    return storage;
  }

 public:
  static Grid::FieldMetaData convertHeader(HiRepHeaderData headerdata) {
    FieldMetaData metadata;
    metadata.dimension[3] = headerdata.t;
    metadata.dimension[0] = headerdata.x;
    metadata.dimension[1] = headerdata.y;
    metadata.dimension[2] = headerdata.z;

    return metadata;
  }
};

}  // namespace Grid
