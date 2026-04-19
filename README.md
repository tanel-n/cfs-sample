# cFS Sample — Quick Start

Quick start for cFS.

Prerequisites
- git, cmake, make, a C compiler (gcc/clang)

## Quick start
Update submodules:

    git submodule update --init --recursive

Copy sample makefile

    cp cfe/cmake/Makefile.sample Makefile          
    cp -r cfe/cmake/sample_defs sample_defs

Build:

    make SIMULATION=native prep          
    make                                 
    make install                         
    cd build/exe/cpu1/                   
    ./core-cpu1 