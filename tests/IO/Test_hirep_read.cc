#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "Grid/parallelIO/HiRepIO.h"
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

    if (Grid::system_endianess() == Grid::endianness::little) {
      Grid::endswap(&n_gauge_tmp);
      Grid::endswap(&n_t_tmp);
      Grid::endswap(&n_x_tmp);
      Grid::endswap(&n_y_tmp);
      Grid::endswap(&n_z_tmp);
      Grid::endswap(&plaquette_tmp);
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

void test_reads_another_n_gauge_from_header() {
  HeaderStreamGenerator stream_gen;
  int N_gauge = 7;
  stream_gen.n_gauge = N_gauge;
  Grid::HiRepHeaderData header = Grid::HiRepIO::readHeader(stream_gen());
  assert(header.N_gauge == N_gauge);
}

void test_read_x_from_header() {
  HeaderStreamGenerator stream_gen;
  int x = 9;
  stream_gen.n_x = x;
  Grid::HiRepHeaderData header = Grid::HiRepIO::readHeader(stream_gen());
  assert(header.x == x);
}

int main() {
  test_reads_n_gauge_from_header();
  test_reads_another_n_gauge_from_header();
  test_read_x_from_header();
  return 0;
}
