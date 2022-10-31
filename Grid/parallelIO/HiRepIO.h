#include <istream>
#include <fstream>
#include <cmath>
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

  bool operator==(const HiRepHeaderData &rhs) {
    return ((abs(plaquette - rhs.plaquette)<1.e-9) and (N_gauge == rhs.N_gauge) and
            (t == rhs.t) and (x == rhs.x) and (y == rhs.y) and (z == rhs.z));
  }
};

class HiRepIO {
 public:
  static HiRepHeaderData readHeader(const std::string &filename) {
    std::ifstream stream(filename, std::ios::binary);
    if (not stream.is_open()) {
      throw std::runtime_error("Danger! This file does not exist.");
    }
    return Grid::HiRepIO::readHeader(stream);
  }

  static HiRepHeaderData readHeader(std::istream &stream) {
    HiRepHeaderData header;
    header.N_gauge = read_with_correct_endianess<int>(stream);
    header.t = read_with_correct_endianess<int>(stream);
    header.x = read_with_correct_endianess<int>(stream);
    header.y = read_with_correct_endianess<int>(stream);
    header.z = read_with_correct_endianess<int>(stream);
    header.plaquette = read_with_correct_endianess<double>(stream);
    if (header.N_gauge != Config_Nc) {
      throw std::runtime_error("N_gauge is not configured as expected.");
    }
    return header;
  };

  static void writeHeader(std::ostream &stream, const HiRepHeaderData &header) {
    write_with_correct_endianess(stream, header.N_gauge);
    write_with_correct_endianess(stream, header.t);
    write_with_correct_endianess(stream, header.x);
    write_with_correct_endianess(stream, header.y);
    write_with_correct_endianess(stream, header.z);
    write_with_correct_endianess(stream, header.plaquette);
  }

 private:
  template <typename T>
  static T read_with_correct_endianess(std::istream &stream) {
    T storage;
    stream.read(reinterpret_cast<char *>(&storage), sizeof(T));
    if (system_endianess() == endianness::little) {
      endswap(&storage);
    }
    return storage;
  }

  template <typename T>
  static void write_with_correct_endianess(std::ostream &stream, T value) {
    if (system_endianess() == endianness::little) {
      endswap(&value);
    }
    stream.write(reinterpret_cast<char *>(&value), sizeof(T));
  }

  static void fill_with_defaults(FieldMetaData &metadata) {
    std::fill(metadata.boundary.begin(), metadata.boundary.end(), "PERIODIC");
    metadata.nd = 4;
    metadata.hdr_version = "";
    // TODO: What is a reasonable default here?
    // metadata.datatype = "";
    // It should indicate the theory, e.g. 4D_SU3_GAUGE_3x3
    metadata.storage_format = "";
    metadata.scidac_checksuma = 0;
    metadata.scidac_checksumb = 0;
    metadata.ensemble_id = "unknown (import from HiRep)";
    metadata.ensemble_label = "unknown (import from HiRep)";
    metadata.sequence_number = 1;
    metadata.creator = "unknown (import from HiRep)";
    metadata.creator_hardware = "unknown (import from HiRep)";
    metadata.creation_date = "unknown (import from HiRep)";
    metadata.archive_date = "unknown (import from HiRep)";
    metadata.floating_point = "IEEE64BIG";
  }

 public:
  static FieldMetaData convertHeader(HiRepHeaderData headerdata) {
    FieldMetaData metadata;
    fill_with_defaults(metadata);
    metadata.dimension[3] = headerdata.t;
    metadata.dimension[0] = headerdata.x;
    metadata.dimension[1] = headerdata.y;
    metadata.dimension[2] = headerdata.z;
    metadata.plaquette = headerdata.plaquette;
    return metadata;
  }
};
}  // namespace Grid