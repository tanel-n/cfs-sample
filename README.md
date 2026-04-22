# cFS Sample — Quick Start

Quick start for cFS. For cFS v7.0.0.

Prerequisites
- git, cmake, make, a C compiler (gcc/clang)

## Quick start
Update submodules:

    git submodule update --init --recursive

Copy sample makefile and create a symbolink for the example app

    cd src/cFS
    cp cfe/cmake/Makefile.sample Makefile  
    cd apps
    ln -s ../../example_app example_app
    cd ..
    ls -s ../../sample_defs_crypto sample_defs  
    cp -r cfe/cmake/sample_defs sample_defs

Build:

    make SIMULATION=native prep          
    make                                 
    make install                         
    cd build/exe/cpu1/                   
    ./core-cpu1 