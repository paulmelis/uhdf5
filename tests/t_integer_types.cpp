
#include <cstdlib>
#include "uhdf5.h"

void
write_file(const char *fname)
{
    h5::File        file;
    h5::Dataset     *dset;
    h5::Attribute   *attr;
    h5::dimensions  d;

    file.create(fname);

{
// int8_t
int8_t v[2] = { -128, 127 };
d.clear();
d.push_back(2);
dset = file.create_dataset<int8_t>("int8_t", d);
dset->write<int8_t>(v);
delete dset;
}
{
// int16_t
int16_t v[2] = { -32768, 32767 };
d.clear();
d.push_back(2);
dset = file.create_dataset<int16_t>("int16_t", d);
dset->write<int16_t>(v);
delete dset;
}
{
// int32_t
int32_t v[2] = { -2147483648L, 2147483647L };
d.clear();
d.push_back(2);
dset = file.create_dataset<int32_t>("int32_t", d);
dset->write<int32_t>(v);
delete dset;
}
{
// int64_t
int64_t v[2] = { -9223372036854775808LL, 9223372036854775807LL };
d.clear();
d.push_back(2);
dset = file.create_dataset<int64_t>("int64_t", d);
dset->write<int64_t>(v);
delete dset;
}
{
// uint8_t
uint8_t v[2] = { 0U, 255U };
d.clear();
d.push_back(2);
dset = file.create_dataset<uint8_t>("uint8_t", d);
dset->write<uint8_t>(v);
delete dset;
}
{
// uint16_t
uint16_t v[2] = { 0U, 65535U };
d.clear();
d.push_back(2);
dset = file.create_dataset<uint16_t>("uint16_t", d);
dset->write<uint16_t>(v);
delete dset;
}
{
// uint32_t
uint32_t v[2] = { 0UL, 4294967295UL };
d.clear();
d.push_back(2);
dset = file.create_dataset<uint32_t>("uint32_t", d);
dset->write<uint32_t>(v);
delete dset;
}
{
// uint64_t
uint64_t v[2] = { 0ULL, 18446744073709551615ULL };
d.clear();
d.push_back(2);
dset = file.create_dataset<uint64_t>("uint64_t", d);
dset->write<uint64_t>(v);
delete dset;
}

}

int
main(int argc, char *argv[])
{
    write_file(argv[1]);
}

