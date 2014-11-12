#include <cstdlib>
#include "uhdf5.h"

void
write_file(const char *fname, int N)
{
    h5::File        file;
    h5::Dataset     *dset;
    h5::Attribute   *attr;

    file.create(fname);

    float   *v = new float[N*3];
    for (int i = 0; i < N*3; i++)
        v[i] = 1.0*i;

    h5::dimensions d;
    d.push_back(N);
    d.push_back(3);

    dset = file.create_dataset<float>("/doh", d);
    dset->write<float>(v);
    delete [] v;

    d.clear();
    d.push_back(2);
    attr = dset->create_attribute<uint32_t>("counts", d);

    uint32_t c[2] = { 123, 456 };
    attr->write<uint32_t>(c);
    delete attr;

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

    dset = file.open_dataset("/doh");

    h5::dimensions dims;
    dset->get_dimensions(dims);
    printf("N=%d: %d, %d\n", dims.size(), dims[0], dims[1]);

    type = dset->get_type();
    printf("Dataset data class = %d, order = %d, size = %d, precision = %d, signed = %d\n",
        type->get_class(), type->get_order(), type->get_size(), type->get_precision(), type->is_signed());

    if (!type->matches<float>())
    {
        printf("Type doesn't match float!\n");
        exit(-1);
    }

    delete type;

    float *v = new float[dims[0]*dims[1]];
    dset->read<float>(v);
    for (int i = 0; i < dims[0]*dims[1]; i++)
        printf("%f ", v[i]);
    printf("\n");
    delete [] v;

    attr = dset->get_attribute("counts");

    type = attr->get_type();
    printf("Attribute data class = %d, order = %d, size = %d, precision = %d, signed = %d\n",
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
        printf("%lu ", c[i]);
    printf("\n");
    delete [] c;

    delete dset;
}

int
main(int argc, char *argv[])
{
    write_file(argv[1], 13);
    read_file(argv[1]);
}