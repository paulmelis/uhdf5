# uHDF5: minimalistic C++ API for working with HDF5 files

uHDF5 is a header-only utility library that makes working with HDF5
from C/C++ slightly more attractive.

## Goals

1. Make working with HDF5 easier and less verbose, compared to using the direct HDF5 C or C++ API
2. Only support every-day operations well, don't try to cover the whole gamut of HDF5 possibilities
3. Expose as little of the underlying HDF5 library as possible
4. Allow to use native C++ types and idioms (like uint8_t, float, double, std::vector)
5. Allow access to the underlying HDF5 objects (by hid_t) if one needs it

## Non-goals

1. Support complex mixes of reading and writing operations to the same file. The basic usage pattern of
   the library is: open file, read one or more datasets, close. OR create file, write one or more datasets, close.
2. Expose all functionality of the HDF5 format and library. Use the direct HDF5 C or C++ API for that.
3. Support for creating files with big-endian data layout (not hard to fix though)

## Example

See included t_uhdf5.cpp


