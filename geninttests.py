#!/usr/bin/env python2

print """
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
"""

for signed in [True, False]:
    for intsize in [8, 16, 32, 64]:

        if signed:
            typename = 'int'
        else:
            typename = 'uint'
        typename += str(intsize)
        typename += '_t'

        if signed:
            minval = - (1 << (intsize-1))
            maxval = -minval - 1
        else:
            minval = 0
            maxval = (1 << intsize) - 1

        suffix = ''
        if not signed:
            suffix = 'U'

        if intsize == 32:
            suffix += 'L'
        elif intsize == 64:
            suffix += 'LL'

        print '{'
        print '// %s' % typename
        print '%s v[2] = { %d%s, %d%s };' % (typename, minval, suffix, maxval, suffix)
        print 'd.clear();'
        print 'd.push_back(2);'
        print 'dset = file.create_dataset<%s>("%s", d);' % (typename, typename)
        print 'dset->write<%s>(v);' % typename
        print 'delete dset;'
        print '}'

print """
}

int
main(int argc, char *argv[])
{
    write_file(argv[1]);
}
"""