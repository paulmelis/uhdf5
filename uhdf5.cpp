#include "uhdf5.h"

namespace h5 {

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

} // namespace h5

