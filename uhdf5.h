#ifndef UHDF5_H
#define UHDF5_H

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

#include <hdf5.h>
#include <vector>
#include <string>

namespace h5
{

typedef std::vector<int>    dimensions;

class Dataset;
class Attribute;
class Type;

//
// Type
//

class Type
{
    enum Class
    {
        NONE=-1,
        INTEGER,
        FLOAT,
        STRING
    };

    enum Order
    {
        ORDER_LE,
        ORDER_BE,
        ORDER_VAX,
        ORDER_MIXED,
        ORDER_NONE
    };

public:
    Type(hid_t type_id);
    ~Type();

    Class       get_class();
    Order       get_order();

    size_t      get_size();         // In bytes
    size_t      get_precision();    // In significant bits
    bool        is_signed();        // For integer types only

    template <typename T>
    bool        matches();

    hid_t       get_id()    { return m_type_id; }

protected:
    hid_t       m_type_id;
};

//
// File
//

class File
{
public:
    File();
    ~File();

    // Fails if file doesn't exist
    bool        open(const char *fname, bool readonly=false);
    // Fails if file exists
    bool        create(const char *fname, bool overwrite=true);
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
    Type        *get_type();

    Attribute*  get_attribute(const char *name);
    template <typename T>
    Attribute*  create_attribute(const char *name, const dimensions& dims);

    template <typename T>
    bool        read(T *values);
    template <typename T>
    bool        write(T *values);

    hid_t       get_id()        { return m_dataset_id; }

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
    Type        *get_type();

    template <typename T>
    bool        read(T *values);
    template <typename T>
    bool        write(T *values);

    hid_t       get_id()        { return m_attribute_id; }

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
