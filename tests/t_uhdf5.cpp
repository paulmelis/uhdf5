#include <cstdlib>
#include <cmath>
#include "uhdf5.h"

void
write_file(const char *fname, int N)
{
    h5::File        file;
    h5::Dataset     *dset;
    h5::Attribute   *attr;

    file.create(fname);

    double *doubles = new double[N*3];
    doubles[0] = M_PI;
    for (int i = 1; i < N*3; i++)
        doubles[i] = 1.0*i;

    h5::dimensions dims;

    // Doubles

    dims.push_back(N);
    dims.push_back(3);

    dset = file.create_dataset<double>("/doubles", dims);
    dset->write<double>(doubles);
    delete [] doubles;

    // Integers

    dims.clear();
    dims.push_back(2);
    attr = dset->create_attribute<uint32_t>("counts", dims);

    uint32_t c[2] = { 123, 456 };
    attr->write<uint32_t>(c);
    delete attr;

    // Large number

    dims.clear();
    dims.push_back(1);
    attr = dset->create_attribute<uint64_t>("large_number", dims);

    uint64_t l = 0x1234567890ffffffULL;
    attr->write<uint64_t>(&l);
    delete attr;

    delete dset;

    // Integers again

    dims.clear();
    dims.push_back(3);
    dims.push_back(1);
    dset = file.create_dataset<int32_t>("/int32_t", dims);
    int32_t s[3] = { -1234567890, -1, 2 };
    dset->write<int32_t>(s);

    delete dset;
}

void
read_file(const char *fname)
{
    h5::File        file;
    h5::Dataset     *dset;
    h5::Attribute   *attr;
    h5::Type        *type;

    file.open(fname);

    dset = file.open_dataset("/doubles");

    h5::dimensions dims;
    dset->get_dimensions(dims);
    printf("N=%zu: %d, %d\n", dims.size(), dims[0], dims[1]);

    type = dset->get_type();
    printf("Dataset data class = %d, order = %d, size = %zu, precision = %zu, signed = %d\n",
        type->get_class(), type->get_order(), type->get_size(), type->get_precision(), type->is_signed());

    if (!type->matches<double>())
    {
        printf("Dataset type doesn't match double!\n");
        exit(-1);
    }

    delete type;

    const int N = dims[0]*dims[1];

    // Read doubles    
    double *doubles = new double[N];
    dset->read<double>(doubles);
    for (int i = 0; i < N; i++)
        printf("%.16f ", doubles[i]);
    printf("\n");
    delete [] doubles;

    // Read doubles as floats
    float *floats = new float[N];
    dset->read<float>(floats);
    for (int i = 0; i < N; i++)
        printf("%.7f ", floats[i]);
    printf("\n");
    delete [] floats;    


    attr = dset->get_attribute("counts");

    type = attr->get_type();
    printf("Attribute data class = %d, order = %d, size = %zu, precision = %zu, signed = %d\n",
        type->get_class(), type->get_order(), type->get_size(), type->get_precision(), type->is_signed());

    if (!type->matches<uint32_t>())
    {
        printf("Type doesn't match uint32_t!\n");
        exit(-1);
    }

    delete type;

    attr->get_dimensions(dims);
    uint32_t *c = new uint32_t[dims[0]];
    attr->read<uint32_t>(c);
    delete attr;
    for (int i = 0; i < dims[0]; i++)
        printf("%u ", c[i]);
    printf("\n");
    delete [] c;

    delete dset;
}

int
main(int argc, char *argv[])
{
    if (--argc != 1)
    {
        printf("usage: %s file.hdf5\n", argv[0]);
        printf("\n");
        printf("Note that file.hdf5 will be overwritten if it exists!\n");
        printf("\n");
        exit(-1);
    }
    
    write_file(argv[1], 13);
    read_file(argv[1]);
}