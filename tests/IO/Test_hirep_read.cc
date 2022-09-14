#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "Grid/parallelIO/HiRepIO.h"

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

struct HeaderStreamGenerator {
  uint32_t n_gauge = 0;
  uint32_t n_t = 0;
  uint32_t n_x = 0;
  uint32_t n_y = 0;
  uint32_t n_z = 0;
  double plaquette = 0.;

  std::stringstream operator()() {
    uint32_t n_gauge_tmp = n_gauge;
    uint32_t n_t_tmp = n_t;
    uint32_t n_x_tmp = n_x;
    uint32_t n_y_tmp = n_y;
    uint32_t n_z_tmp = n_z;
    double plaquette_tmp = plaquette;

    if (system_endianess() == endianness::little) {
      endswap(&n_gauge_tmp);
      endswap(&n_t_tmp);
      endswap(&n_x_tmp);
      endswap(&n_y_tmp);
      endswap(&n_z_tmp);
      endswap(&plaquette_tmp);
    }

    std::stringstream stream;
    stream.write(reinterpret_cast<const char *>(&n_gauge_tmp),
                 sizeof(n_gauge_tmp));
    stream.write(reinterpret_cast<const char *>(&n_t_tmp), sizeof(n_t_tmp));
    stream.write(reinterpret_cast<const char *>(&n_x_tmp), sizeof(n_x_tmp));
    stream.write(reinterpret_cast<const char *>(&n_y_tmp), sizeof(n_y_tmp));
    stream.write(reinterpret_cast<const char *>(&n_z_tmp), sizeof(n_z_tmp));
    stream.write(reinterpret_cast<const char *>(&plaquette_tmp),
                 sizeof(plaquette_tmp));
    return stream;
  }
};

void test_reads_n_gauge_from_header() {
  HeaderStreamGenerator stream_gen;
  int N_gauge = 42;
  stream_gen.n_gauge = N_gauge;
  Grid::HiRepHeaderData header = Grid::HiRepIO::readHeader(stream_gen());
  assert(header.N_gauge == N_gauge);
}

int main() {
  test_reads_n_gauge_from_header();
  return 0;
}
