#include "uhdf5.h"

void
write_file(const char *fname, int N)
{
    h5::File    file;
    h5::Dataset *dset;
    h5::Attribute *attr;

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
    h5::File    file;
    h5::Dataset *dset;
    h5::Attribute *attr;

    file.open(fname);

    dset = file.open_dataset("/doh");
    h5::dimensions dims;
    dset->get_dimensions(dims);
    printf("N=%d: %d, %d\n", dims.size(), dims[0], dims[1]);

    float *v = new float[dims[0]*dims[1]];
    dset->read<float>(v);
    for (int i = 0; i < dims[0]*dims[1]; i++)
        printf("%f ", v[i]);
    printf("\n");
    delete [] v;

    attr = dset->get_attribute("counts");
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