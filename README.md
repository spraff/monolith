# Build Instructions

mkdir build
cd build
ccmake /path/to/monolith

In the ccmake setup:

* make sure the compiler is gcc-4.7 or later
* add the -std=c+11 flag.

You should also be able to use the -W -Wall -Wextra -Werror -pedantic flags.

Configure and generate the makefiles.

Then run 'make' and look in ../bin for the binaries.

# Projects

## TextureMorph

A demonstration of how functions of complex numbers can generate interesting colour fields and texture coordinate transformations.
