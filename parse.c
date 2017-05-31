#include <stdio.h>
#include <runt.h>
#include "rscheme.h"

static int loader(runt_vm *vm)
{
    runt_load_stdlib(vm);
    runt_load_scheme(vm);
    return runt_is_alive(vm);
}

int main(int argc, char *argv[])
{
    return irunt_begin(argc, argv, loader);
}
