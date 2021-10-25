TARGET   = PythonQtTest
TEMPLATE = app

DESTDIR    = ../lib

QT += testlib opengl

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets
}

include ( ../build/common.prf )
include ( ../build/PythonQt.prf )

HEADERS +=                    \
  PythonQtTests.h

SOURCES +=                    \
  PythonQtTestMain.cpp        \
  PythonQtTests.cpp
