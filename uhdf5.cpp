/*
The MIT License (MIT)

Copyright (c) 2014 Paul E.C. Melis (paul.melis@surfsara.nl)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

#include "uhdf5.h"

namespace h5 {

//
// Type
//

Type::Type(hid_t type_id)
{
    m_type_id = type_id;
}

Type::~Type()
{
    H5Tclose(m_type_id);
}

Type::Class
Type::get_class()
{
    const H5T_class_t& c = H5Tget_class(m_type_id);

    switch (c)
    {
    case H5T_INTEGER:
        return INTEGER;
        break;
    case H5T_FLOAT:
        return FLOAT;
        break;
    case H5T_STRING:
        return STRING;
        break;
    default:
        return NONE;
    }
}

Type::Order
Type::get_order()
{
    const H5T_order_t& o = H5Tget_order(m_type_id);

    switch (o)
    {
    case H5T_ORDER_LE:
        return ORDER_LE;
        break;
    case H5T_ORDER_BE:
        return ORDER_BE;
        break;
    case H5T_ORDER_VAX:
        return ORDER_VAX;
        break;
    case H5T_ORDER_MIXED:
        return ORDER_MIXED;
        break;
    default:
        return ORDER_NONE;
    }
}

size_t
Type::get_size()
{
    return H5Tget_size(m_type_id);
}

size_t
Type::get_precision()
{
    return H5Tget_precision(m_type_id);
}

bool
Type::is_signed()
{
    // XXX assumes only one other return value from get_sign() possible
    return H5Tget_sign(m_type_id) == H5T_SGN_2;
}

// XXX to truly check for IEEE float and double would need to do a bit more :)
template<> bool Type::matches<float>()      { return get_class() == FLOAT && get_size() == 4; }
template<> bool Type::matches<double>()     { return get_class() == FLOAT && get_size() == 8; }

template<> bool Type::matches<int8_t>()     { return get_class() == INTEGER && get_size() == 1 && is_signed(); }
template<> bool Type::matches<int16_t>()    { return get_class() == INTEGER && get_size() == 2 && is_signed(); }
template<> bool Type::matches<int32_t>()    { return get_class() == INTEGER && get_size() == 4 && is_signed(); }
template<> bool Type::matches<int64_t>()    { return get_class() == INTEGER && get_size() == 8 && is_signed(); }

template<> bool Type::matches<uint8_t>()    { return get_class() == INTEGER && get_size() == 1 && !is_signed(); }
template<> bool Type::matches<uint16_t>()   { return get_class() == INTEGER && get_size() == 2 && !is_signed(); }
template<> bool Type::matches<uint32_t>()   { return get_class() == INTEGER && get_size() == 4 && !is_signed(); }
template<> bool Type::matches<uint64_t>()   { return get_class() == INTEGER && get_size() == 8 && !is_signed(); }

//
// File
//

File::File()
{
    m_file_id = -1;
}

File::~File()
{
    close();
}

bool
File::open(const char *fname)
{
    // H5F_ACC_RDWR or H5F_ACC_RDONLY
    unsigned int flags = H5F_ACC_RDONLY;

    m_file_id = H5Fopen(fname, flags, H5P_DEFAULT);
    if (m_file_id < 0)
        return false;

    return true;
}

bool
File::create(const char *fname)
{
    /*
    The flags parameter specifies whether an existing file is to be overwritten. It should be set to either H5F_ACC_TRUNC to overwrite an existing file or H5F_ACC_EXCL, instructing the function to fail if the file already exists.
    */

    unsigned int flags = H5F_ACC_TRUNC;

    /*
    New files are always created in read-write mode, so the read-write and read-only flags,
    H5F_ACC_RDWR and H5F_ACC_RDONLY, respectively, are not relevant in this function.
    Further note that a specification of H5F_ACC_RDONLY will be ignored; the file will
    be created in read-write mode, regardless.
    */

    m_file_id = H5Fcreate(fname, flags, H5P_DEFAULT, H5P_DEFAULT);
    if (m_file_id < 0)
        return false;

    return true;
}

void
File::close()
{
    if (m_file_id >= 0)
        H5Fclose(m_file_id);
}

Dataset*
File::open_dataset(const char *path)
{
    hid_t   dataset_id;

    dataset_id = H5Dopen2(m_file_id, path, H5P_DEFAULT);

    if (dataset_id < 0)
        return NULL;

    return new Dataset(this, dataset_id);
}

template<>
Dataset*
File::create_dataset<float>(const char *path, const dimensions& dims)
{
    return _create_dataset(path, dims, H5T_IEEE_F32LE);
}

template<>
Dataset*
File::create_dataset<double>(const char *path, const dimensions& dims)
{
    return _create_dataset(path, dims, H5T_IEEE_F64LE);
}

template<>
Dataset*
File::create_dataset<uint8_t>(const char *path, const dimensions& dims)
{
    return _create_dataset(path, dims, H5T_STD_U8LE);
}

template<>
Dataset*
File::create_dataset<uint16_t>(const char *path, const dimensions& dims)
{
    return _create_dataset(path, dims, H5T_STD_U16LE);
}

template<>
Dataset*
File::create_dataset<uint32_t>(const char *path, const dimensions& dims)
{
    return _create_dataset(path, dims, H5T_STD_U32LE);
}

template<>
Dataset*
File::create_dataset<uint64_t>(const char *path, const dimensions& dims)
{
    return _create_dataset(path, dims, H5T_STD_U64LE);
}

Dataset*
File::_create_dataset(const char *path, const dimensions& dims, hid_t dtype)
{
    hid_t   dataspace_id, dataset_id;

    const int N = dims.size();
    hsize_t d[N];
    for (int i = 0; i < N; i++)
        d[i] = dims[i];
    dataspace_id = H5Screate_simple(N, d, NULL);

    dataset_id = H5Dcreate2(m_file_id, path, dtype,
        dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    H5Sclose(dataspace_id);

    if (dataset_id < 0)
        return NULL;

    return new Dataset(this, dataset_id);
}

//
// Dataset
//

Dataset::Dataset(File *file, hid_t dset_id)
{
    m_file = file;
    m_dataset_id = dset_id;
}

Dataset::~Dataset()
{
    H5Dclose(m_dataset_id);
}

bool
Dataset::get_dimensions(dimensions& dims)
{
    hid_t   dataspace_id;
    int     ndims;

    dataspace_id = H5Dget_space(m_dataset_id);
    if (dataspace_id < 0)
        return false;

    ndims = H5Sget_simple_extent_ndims(dataspace_id);

    hsize_t d[ndims];
    H5Sget_simple_extent_dims(dataspace_id, d, NULL);

    H5Sclose(dataspace_id);

    dims.clear();
    for (int i = 0; i < ndims; i++)
        dims.push_back(d[i]);

    return true;
}

Type*
Dataset::get_type()
{
    return new Type(H5Dget_type(m_dataset_id));
}

// Dataset::read

template <typename T>
bool
Dataset::_read(T* values, hid_t memtype)
{
    herr_t  status;

    status = H5Dread(m_dataset_id, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, values);

    return status >= 0;
}

template<>
bool
Dataset::read<float>(float *values)
{
    return _read<float>(values, H5T_NATIVE_FLOAT);
}

template<>
bool
Dataset::read<double>(double *values)
{
    return _read<double>(values, H5T_NATIVE_DOUBLE);
}

template<>
bool
Dataset::read<uint8_t>(uint8_t *values)
{
    return _read<uint8_t>(values, H5T_NATIVE_UINT8);
}

template<>
bool
Dataset::read<uint16_t>(uint16_t *values)
{
    return _read<uint16_t>(values, H5T_NATIVE_UINT16);
}

template<>
bool
Dataset::read<uint32_t>(uint32_t *values)
{
    return _read<uint32_t>(values, H5T_NATIVE_UINT32);
}

template<>
bool
Dataset::read<uint64_t>(uint64_t *values)
{
    return _read<uint64_t>(values, H5T_NATIVE_UINT64);
}

// Dataset::write

template <typename T>
bool
Dataset::_write(const T* values, hid_t memtype)
{
    herr_t  status;

    status = H5Dwrite(m_dataset_id, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, values);

    return status >= 0;
}

template<>
bool
Dataset::write<float>(float *values)
{
    return _write<float>(values, H5T_NATIVE_FLOAT);
}

template<>
bool
Dataset::write<double>(double *values)
{
    return _write<double>(values, H5T_NATIVE_DOUBLE);
}

template<>
bool
Dataset::write<uint8_t>(uint8_t *values)
{
    return _write<uint8_t>(values, H5T_NATIVE_UINT8);
}

template<>
bool
Dataset::write<uint16_t>(uint16_t *values)
{
    return _write<uint16_t>(values, H5T_NATIVE_UINT16);
}

template<>
bool
Dataset::write<uint32_t>(uint32_t *values)
{
    return _write<uint32_t>(values, H5T_NATIVE_UINT32);
}

template<>
bool
Dataset::write<uint64_t>(uint64_t *values)
{
    return _write<uint64_t>(values, H5T_NATIVE_UINT64);
}

// Dataset attributes

Attribute*
Dataset::get_attribute(const char *name)
{
    hid_t   attribute_id;

    attribute_id = H5Aopen(m_dataset_id, name, H5P_DEFAULT);

    if (attribute_id < 0)
        return NULL;

    return new Attribute(this, attribute_id);
}

template<>
Attribute*
Dataset::create_attribute<float>(const char *name, const dimensions& dims)
{
    return _create_attribute(name, dims, H5T_NATIVE_FLOAT);
}

template<>
Attribute*
Dataset::create_attribute<double>(const char *name, const dimensions& dims)
{
    return _create_attribute(name, dims, H5T_NATIVE_DOUBLE);
}

template<>
Attribute*
Dataset::create_attribute<uint8_t>(const char *name, const dimensions& dims)
{
    return _create_attribute(name, dims, H5T_NATIVE_UINT8);
}

template<>
Attribute*
Dataset::create_attribute<uint16_t>(const char *name, const dimensions& dims)
{
    return _create_attribute(name, dims, H5T_NATIVE_UINT16);
}

template<>
Attribute*
Dataset::create_attribute<uint32_t>(const char *name, const dimensions& dims)
{
    return _create_attribute(name, dims, H5T_NATIVE_UINT32);
}

template<>
Attribute*
Dataset::create_attribute<uint64_t>(const char *name, const dimensions& dims)
{
    return _create_attribute(name, dims, H5T_NATIVE_UINT64);
}

Attribute*
Dataset::_create_attribute(const char *name, const dimensions& dims, hid_t dtype)
{
    hid_t   attr_id, dataspace_id;

    const int N = dims.size();
    hsize_t d[N];
    for (int i = 0; i < N; i++)
        d[i] = dims[i];
    dataspace_id = H5Screate_simple(N, d, NULL);

    attr_id = H5Acreate(m_dataset_id, name, dtype, dataspace_id, H5P_DEFAULT, H5P_DEFAULT);

    H5Sclose(dataspace_id);

    if (attr_id < 0)
        return NULL;

    return new Attribute(this, attr_id);
}

//
// Attribute
//

Attribute::Attribute(Dataset *dataset, hid_t attr_id)
{
    m_dataset = dataset;
    m_attribute_id = attr_id;
}

Attribute::~Attribute()
{
    H5Aclose(m_attribute_id);
}

bool
Attribute::get_dimensions(dimensions& dims)
{
    hid_t   dataspace_id;
    int     ndims;

    dataspace_id = H5Aget_space(m_attribute_id);
    if (dataspace_id < 0)
        return false;

    ndims = H5Sget_simple_extent_ndims(dataspace_id);

    hsize_t d[ndims];
    H5Sget_simple_extent_dims(dataspace_id, d, NULL);

    H5Sclose(dataspace_id);

    dims.clear();
    for (int i = 0; i < ndims; i++)
        dims.push_back(d[i]);

    return true;
}

Type*
Attribute::get_type()
{
    return new Type(H5Aget_type(m_attribute_id));
}


// Attribute::read

template <typename T>
bool
Attribute::_read(T* values, hid_t memtype)
{
    herr_t  status;

    status = H5Aread(m_attribute_id, memtype, values);

    return status >= 0;
}

template<> bool Attribute::read<float>(float *values)       { return _read(values, H5T_NATIVE_FLOAT); }
template<> bool Attribute::read<double>(double *values)     { return _read(values, H5T_NATIVE_DOUBLE); }
template<> bool Attribute::read<uint8_t>(uint8_t *values)   { return _read(values, H5T_NATIVE_UINT8); }
template<> bool Attribute::read<uint16_t>(uint16_t *values) { return _read(values, H5T_NATIVE_UINT16); }
template<> bool Attribute::read<uint32_t>(uint32_t *values) { return _read(values, H5T_NATIVE_UINT32); }
template<> bool Attribute::read<uint64_t>(uint64_t *values) { return _read(values, H5T_NATIVE_UINT64); }

// Attribute::write

template <typename T>
bool
Attribute::_write(const T* values, hid_t memtype)
{
    herr_t  status;

    status = H5Awrite(m_attribute_id, memtype, values);

    return status >= 0;
}

template<> bool Attribute::write<float>(float *values)       { return _write(values, H5T_NATIVE_FLOAT); }
template<> bool Attribute::write<double>(double *values)     { return _write(values, H5T_NATIVE_DOUBLE); }
template<> bool Attribute::write<uint8_t>(uint8_t *values)   { return _write(values, H5T_NATIVE_UINT8); }
template<> bool Attribute::write<uint16_t>(uint16_t *values) { return _write(values, H5T_NATIVE_UINT16); }
template<> bool Attribute::write<uint32_t>(uint32_t *values) { return _write(values, H5T_NATIVE_UINT32); }
template<> bool Attribute::write<uint64_t>(uint64_t *values) { return _write(values, H5T_NATIVE_UINT64); }

} // namespace h5

