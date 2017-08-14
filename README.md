Templar
=======

Visualization tool for Templight C++ template debugger traces

This repository contains an updated version of Templar (http://plc.inf.elte.hu/templight/templar-20130506.tar.gz) from
http://plc.inf.elte.hu/templight/

The goal of this version is to be compatible to Qt4/5 and Graphviz 2.34.0 (using CGraph)

Licensing and Copyright
=======================

There is no license yet (neither for Templight nor for Templar).
As Templight will probably be incorporated into clang the LLVM/clang license is a candidate.

Templar was created by Zoltán Borok-Nagy so he holds the copyright on it.

Dependencies
============
Templar uses following libraries:

- Qt4 or Qt5
- GraphViz (CGraph)
- YamlCpp (for YAML file support, via PkgConfig)
- protobuf (for protobuf file support)

Build
=====

Make sure to check out the templight-tools submodule as well (e.g. `git submodule update --init`).
Please be aware that this submodule might introduce further dependencies such as Boost.

To build Templar run:

```
cmake CMakeLists.txt
make
```

in the source folder.
After make the folder should contain an executable named "Templar".

Usage
=====

After having created a compilation trace with `templight` you can start Templar (e.g. `./Templar`)
and open the trace via the `File` menu. Once a trace has been loaded you can navigate it.

For information on how to navigate through traces refer to the Wiki: https://github.com/schulmar/Templar/wiki/HowTo
