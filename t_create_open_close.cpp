#include <cstdlib>
#include "uhdf5.h"

int main(int argc, char *argv[])
{
    h5::File        file;

    const char *fname = argv[1];

    //if (!file.create(fname))
    if (!file.open(fname))
    {
        printf("File creation/open failed!\n");
        exit(-1);
    }

    printf("id = %d\n", file.get_id());

    file.close();
}