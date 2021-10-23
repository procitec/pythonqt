#pragma once

#include "ui_GuiTest.h"
#include "PythonQt.h"
#include "filecreatorwrapper.h"
#include <QMainWindow>

class PyGUI;
class PyMeta;

class Gui: public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT

public:
  explicit Gui();
  ~Gui();

public slots:
  void on_red_clicked();
  void on_green_clicked();
  void on_blue_clicked();

  void on_execute_clicked();
  void on_context_changed(const QString& text);

private:
    PyGUI* mp_pygui;
    PyMeta* mp_meta;
    FileHandlerFactory* mp_filehandler_factory;
};
