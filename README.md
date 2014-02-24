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

Build
=====

To build Templar run:

qmake
make

in the source folder.
After make the folder should contain an executable named "Templar".

Eventually you need to explicitly add the graphviz include path:

qmake INCLUDEPATH+=/path/to/includes/of/graphviz

Compatibility
=============

The current version has been compiled succesfully with Graphviz 2.34.0 and Qt4.8.5/Qt 5.2.0 on GCC 4.8.2 and Clang 3.5 (trunk 196048)
