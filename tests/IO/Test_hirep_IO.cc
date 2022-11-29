#include <algorithm>
#include <cassert>
#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <cstdio>

#include "Grid/Grid.h"
#include "Grid/parallelIO/HiRepIO.h"

struct HeaderStreamGenerator {
  uint32_t n_gauge = Config_Nc;
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
    stream.write(reinterpret_cast<const char*>(&n_gauge_tmp),
                 sizeof(n_gauge_tmp));
    stream.write(reinterpret_cast<const char*>(&n_t_tmp), sizeof(n_t_tmp));
    stream.write(reinterpret_cast<const char*>(&n_x_tmp), sizeof(n_x_tmp));
    stream.write(reinterpret_cast<const char*>(&n_y_tmp), sizeof(n_y_tmp));
    stream.write(reinterpret_cast<const char*>(&n_z_tmp), sizeof(n_z_tmp));
    stream.write(reinterpret_cast<const char*>(&plaquette_tmp),
                 sizeof(plaquette_tmp));
    return stream;
  }
};

class ReadHeaderFixture {
  HeaderStreamGenerator stream_gen;  // must be instantiated first
  uint32_t& n_gauge;
  uint32_t& n_t;
  uint32_t& n_x;
  uint32_t& n_y;
  uint32_t& n_z;
  double& plaquette;
  Grid::HiRepHeaderData header;

  void run_readHeader() {
    std::stringstream stream = stream_gen();
    header = Grid::HiRepIO::readHeader(stream);
  }

 public:
  ReadHeaderFixture()
      : n_gauge(stream_gen.n_gauge),
        n_t(stream_gen.n_t),
        n_x(stream_gen.n_x),
        n_y(stream_gen.n_y),
        n_z(stream_gen.n_z),
        plaquette(stream_gen.plaquette) {}

  void test_read_n_gauge_from_header() {
    n_gauge = Config_Nc;
    run_readHeader();
    assert(header.N_gauge == n_gauge);
  }

  void test_read_t_from_header() {
    n_t = 13;
    run_readHeader();
    assert(header.t == n_t);
  }

  void test_read_another_t_from_header() {
    n_t = 14;
    run_readHeader();
    assert(header.t == n_t);
  }

  void test_read_x_from_header() {
    n_x = 15;
    run_readHeader();
    assert(header.x == n_x);
  }

  void test_read_another_x_from_header() {
    n_x = 16;
    run_readHeader();
    assert(header.x == n_x);
  }

  void test_read_y_from_header() {
    n_y = 17;
    run_readHeader();
    assert(header.y == n_y);
  }

  void test_read_another_y_from_header() {
    n_y = 18;
    run_readHeader();
    assert(header.y == n_y);
  }

  void test_read_z_from_header() {
    n_z = 19;
    run_readHeader();
    assert(header.z == n_z);
  }

  void test_read_another_z_from_header() {
    n_z = 20;
    run_readHeader();
    assert(header.z == n_z);
  }

  void test_read_plaquette_from_header() {
    plaquette = 21.;
    run_readHeader();
    assert(header.plaquette == plaquette);
  }

  void test_read_another_plaquette_from_header() {
    plaquette = 22.;
    run_readHeader();
    assert(header.plaquette == plaquette);
  }
};

void test_t_in_metadata() {
  int t = 5;
  Grid::HiRepHeaderData headerdata;
  headerdata.t = t;
  Grid::FieldMetaData header = Grid::HiRepIO::convertHeader(headerdata);
  assert(header.dimension[3] == t);
}

void test_x_in_metadata() {
  int x = 6;
  Grid::HiRepHeaderData headerdata;
  headerdata.x = x;
  Grid::FieldMetaData header = Grid::HiRepIO::convertHeader(headerdata);
  assert(header.dimension[0] == x);
}

void test_y_in_metadata() {
  int y = 7;
  Grid::HiRepHeaderData headerdata;
  headerdata.y = y;
  Grid::FieldMetaData header = Grid::HiRepIO::convertHeader(headerdata);
  assert(header.dimension[1] == y);
}

void test_z_in_metadata() {
  int z = 8;
  Grid::HiRepHeaderData headerdata;
  headerdata.z = z;
  Grid::FieldMetaData header = Grid::HiRepIO::convertHeader(headerdata);
  assert(header.dimension[2] == z);
}

void test_plaquette_in_metadata() {
  int plaquette = 11.;
  Grid::HiRepHeaderData headerdata;
  headerdata.plaquette = plaquette;
  Grid::FieldMetaData header = Grid::HiRepIO::convertHeader(headerdata);
  assert(header.plaquette == plaquette);
}

void test_fieldmetadata_has_correct_defaults() {
  Grid::FieldMetaData header =
      Grid::HiRepIO::convertHeader(Grid::HiRepHeaderData());
  assert(header.nd == 4);
  assert(header.hdr_version == "");
  // TODO: What is a reasonable default here?
  // assert(header.datatype == "");
  // It should indicate the theory, e.g. 4D_SU3_GAUGE_3x3
  assert(header.storage_format == "");
  assert(header.boundary[0] == "PERIODIC");
  assert(header.boundary[1] == "PERIODIC");
  assert(header.boundary[2] == "PERIODIC");
  assert(header.boundary[3] == "PERIODIC");
  assert(header.scidac_checksuma == 0);
  assert(header.scidac_checksumb == 0);
  assert(header.ensemble_id == "unknown (import from HiRep)");
  assert(header.ensemble_label == "unknown (import from HiRep)");
  assert(header.sequence_number == 1);
  assert(header.creator == "unknown (import from HiRep)");
  assert(header.creator_hardware == "unknown (import from HiRep)");
  assert(header.creation_date == "unknown (import from HiRep)");
  assert(header.archive_date == "unknown (import from HiRep)");
  assert(header.floating_point == "IEEE64BIG");
}

// TODO:
void test_checksum_gets_computed() { assert(false); }

// TODO:
void test_link_trace_gets_computed() { assert(false); }

void test_n_gauge_gets_checked() {  // Safeguard!!!
  int n_gauge = Config_Nc + 1;      // just to be different from Config_Nc
  HeaderStreamGenerator stream_gen;
  stream_gen.n_gauge = n_gauge;
  std::stringstream stream = stream_gen();
  bool found_exception = false;
  try {
    Grid::HiRepIO::readHeader(stream);
  } catch (std::exception& e) {
    found_exception = true;
  }
  assert(found_exception);
}

void test_read_header_fails_for_non_existent_file() {
  std::string filename("Non-existentFile");
  bool found_exception = false;
  try {
    Grid::HiRepIO::readHeader(filename);
  } catch (std::exception& e) {
    found_exception = true;
  }
  assert(found_exception);
}

void test_read_header_from_real_hirep_file() {
  std::string filename("run1_4x4x4x4nc2rADJnf2b2.250000m0.500000n10");
  Grid::HiRepHeaderData expected_header({0.51419654, 2, 4, 4, 4, 4});
  Grid::HiRepHeaderData header = Grid::HiRepIO::readHeader(filename);
  assert(header == expected_header);
}

void test_write_header_to_hirep() {
  Grid::HiRepHeaderData expected_header({0.51419654, 2, 4, 4, 4, 4});
  std::stringstream stream;
  Grid::HiRepIO::writeHeader(stream, expected_header);
  stream.seekp(0);
  Grid::HiRepHeaderData header = Grid::HiRepIO::readHeader(stream);
  assert(header == expected_header);
}

void test_work_with_real_files() {
  std::string filename("HiGrid");
  Grid::HiRepHeaderData expected_header({0.51419654, 2, 4, 4, 4, 4});
  Grid::HiRepIO::writeHeader(filename, expected_header);
  Grid::HiRepHeaderData header = Grid::HiRepIO::readHeader(filename);
  assert(header == expected_header);
  assert(0 == std::remove(filename.data()));
}

void test_read_gauge_configuration() { // To be done
  Grid::Coordinate simd_layout = Grid::GridDefaultSimd(4, Grid::vComplex::Nsimd());
  Grid::Coordinate mpi_layout = Grid::GridDefaultMpi();
  Grid::Coordinate latt_size({4,4,4,4});
  Grid::GridCartesian grid(latt_size, simd_layout, mpi_layout);
  Grid::LatticeGaugeField Umu(&grid);
  Grid::HiRepHeaderData header({0.0, 2, latt_size[0], latt_size[1], latt_size[2], latt_size[3]});
  std::stringstream stream;
  Grid::HiRepIO::readConfiguration(Umu, header, stream);
  Grid::LatticeGaugeField expected_Umu = Umu;
  Grid::LatticeGaugeField Umu_diff = Umu - expected_Umu;
  assert(Grid::norm2(Umu_diff));
}

int main() {
  ReadHeaderFixture().test_read_n_gauge_from_header();
  ReadHeaderFixture().test_read_t_from_header();
  ReadHeaderFixture().test_read_another_t_from_header();
  ReadHeaderFixture().test_read_x_from_header();
  ReadHeaderFixture().test_read_another_x_from_header();
  ReadHeaderFixture().test_read_y_from_header();
  ReadHeaderFixture().test_read_another_y_from_header();
  ReadHeaderFixture().test_read_z_from_header();
  ReadHeaderFixture().test_read_another_z_from_header();
  ReadHeaderFixture().test_read_plaquette_from_header();
  ReadHeaderFixture().test_read_another_plaquette_from_header();
  test_t_in_metadata();
  test_x_in_metadata();
  test_y_in_metadata();
  test_z_in_metadata();
  test_fieldmetadata_has_correct_defaults();
  test_plaquette_in_metadata();
  test_n_gauge_gets_checked();
  test_read_header_from_real_hirep_file();
  test_read_header_fails_for_non_existent_file();
  test_write_header_to_hirep();
  test_work_with_real_files();
  test_read_gauge_configuration();

  std::cout << "Success!\n";
  return 0;
}