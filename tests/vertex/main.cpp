#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <memory>

#include <primer/string.h>
#include <primer/vector.h>
#include <primer/io.h>

#include <prime-vulkan/vulkan.h>

#include "application.h"


int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Application app;

    fprintf(stderr, "Init Wayland...\n");
    app.init_wayland();

    fprintf(stderr, "Init Vulkan...\n");
    app.init_vulkan();

    app.run();

    return 0;
}
