#include "gui.h"

#include <QApplication>

int main( int argc, char **argv )
{
  QApplication qapp(argc, argv);

  Gui gui;
  gui.show();

  return qapp.exec();
}

