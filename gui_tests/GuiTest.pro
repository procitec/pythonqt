TARGET   = GuiTest
TEMPLATE = app

CONFIG += debug

mac:CONFIG -= app_bundle

DESTDIR = ../lib

contains(QT_MAJOR_VERSION, 6) {
  QT += widgets
}

include ( ../build/common.prf )  
include ( ../build/PythonQt.prf )  

SOURCES +=      \
  main.cpp	\
  pyobjects.cpp	\
  gui.cpp

HEADERS +=	\
  pyobjects.h	\
  gui.h
 
FORMS = GuiTest.ui

RESOURCES += GuiTest.qrc
