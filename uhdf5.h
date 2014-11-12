#ifndef UHDF5_H
#define UHDF5_H

#include <hdf5.h>
#include <vector>
#include <string>

namespace h5
{
    
typedef std::vector<int>    dimensions;
    
class Dataset;
class Attribute;

// 
// File
//
    
class File
{
public:    
    File();
    ~File();

    // Fails if file doesn't exist
    bool        open(const char *fname);   // bool readonly=false
    // Fails if file exists
    bool        create(const char *fname); // bool overwrite=false
    void        close();

    // Returns NULL if failed
    Dataset*    open_dataset(const char *path);

    // Returns NULL if failed
    template <typename T>
    Dataset*    create_dataset(const char *path, const dimensions& dims);

    hid_t       get_id()    { return m_file_id; }

protected:
    Dataset*    _create_dataset(const char *path, const dimensions& dims, hid_t dtype);
    
protected:
    hid_t       m_file_id;
};

// 
// Dataset
//

class Dataset
{
public:
    Dataset(File *file, hid_t dset_id);
    ~Dataset();

    bool        get_dimensions(dimensions& dims);

    Attribute*  get_attribute(const char *name);
    template <typename T>      
    Attribute*  create_attribute(const char *name, const dimensions& dims);

    template <typename T>
    bool        read(T *values);
    template <typename T>
    bool        write(T *values);

    hid_t       get_id()    { return m_dataset_id; }
    
protected:
    
    Attribute*  _create_attribute(const char *name, const dimensions& dims, hid_t dtype);

    template <typename T>
    bool        _read(T* values, hid_t memtype);

    template <typename T>
    bool        _write(const T* values, hid_t memtype);

protected:
    File        *m_file;
    hid_t       m_dataset_id;
};

//
// Attribute
//

class Attribute
{
public:
    Attribute(Dataset *dataset, hid_t attr_id);
    ~Attribute();

    bool        get_dimensions(dimensions& dims);

    template <typename T>
    bool        read(T *values);
    template <typename T>
    bool        write(T *values);

    hid_t       get_id()    { return m_attribute_id; }
    
protected:

    template <typename T>
    bool        _read(T* values, hid_t memtype);

    template <typename T>
    bool        _write(const T* values, hid_t memtype);

protected:
    Dataset     *m_dataset;
    hid_t       m_attribute_id;
};

} // namespace h5

#endif
