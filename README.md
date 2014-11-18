# uHDF5: minimalistic C++ API for working with HDF5 files

## Goals

1. Make working with HDF5 easier and less verbose, compared to using the direct HDF5 C or C++ API
2. Only support every-day operations well, don't try to cover the whole gamut of HDF5 possibilities
3. Expose as little of the underlying HDF5 library as possible
4. Allow to use native C++ types and idioms (like uint8_t, float, double, std::vector)

## Non-goals

1. Support complex mixes of reading and writing operations to the same file. Basic usage pattern of
   the library is open file, read, close OR create file, write close.
2. Expose all functionality of the HDF5 format and library. Use the direct HDF5 C or C++ API for that.

## Example

See included t_uhdf5.cpp


