#include "PythonQt.h"
#include "gui.h"

#include <QApplication>


int main( int argc, char **argv )
{
  QApplication qapp(argc, argv);

  PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
  Gui gui;
  gui.show();

  // PythonQtObjectPtr  mainContext = PythonQt::self()->getMainModule();
  // PythonQtScriptingConsole console(NULL, mainContext);
// 
  // mainContext.evalFile(":example.py");
// 
  // console.show();
  return qapp.exec();
}

