TARGET   = GuiTest
TEMPLATE = app

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
  filehandler.cpp	\
  filecreatorwrapper.cpp	\
  gui.cpp

HEADERS +=	\
  pyobjects.h	\
  filehandler.h	\
  filecreatorwrapper.h	\
  filedec.h	\
  gui.h
 
FORMS = GuiTest.ui

RESOURCES += GuiTest.qrc
