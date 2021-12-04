# PythonQt

[![License](https://img.shields.io/github/license/mevislab/pythonqt.svg?color=blue)](LICENSE)

PythonQt is a dynamic [Python](https://www.python.org) binding for [Qt](https://www.qt.io).
It offers an easy way to embed the Python scripting language into
your Qt applications.

# Documentation

API documentation is available at: https://mevislab.github.io/pythonqt

# Licensing

PythonQt is distributed under the `LGPL 2.1` license.

## Licensing of Generator

The build system of PythonQt makes use of a patched version of the LGPL'ed QtScript
generator, located in the `generator` directory.

See the `LICENSE.LGPL` file in the generator subdirectory for details.
Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)

See https://code.qt.io/cgit/qt-labs/qtscriptgenerator.git for the original project.
The PythonQt wrappers generated by the generator are distributed under the `LGPL`
as well.

The generated wrappers are pre-generated and checked-in, so you only
need to build and run the generator when you want to build additional wrappers
or you want to upgrade/downgrade to another Qt version, but this requires
updating the typesystems as well.

# Building
You need Qt6 to compile PythonQt. Otherwise you will get an error message while calling qmake.

## Building on Windows with MinGW

To build PythonQt, you need to set the environment variable `PYTHON_PATH` to
point to the root dir of the python installation and `PYTHON_LIB` to point to 
the directory where the python lib file is located. Then you should set the
`PYTHON_VERSION` variable to the Python version number.

When using the prebuild Python installer, this will be:

```cmd
set PYTHON_PATH = c:\Python38
set PYTHON_LIB  = c:\Python38\libs
set PYTHON_VERSION = 38
```

## Building on Unix
Set your environment variable PATH to your qt6 build directory (e.g. qt6-build/qtbase/bin).
```bash
PATH=~/qt6-build/qtbase/bin:$PATH 
qmake .
make -j
```

# Tests
Running unittests on unix:
```bash
cd lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
./PythonQtTest
```

### GuiTests
See `gui_tests/README.md`

