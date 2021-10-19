#pragma once

#include "ui_GuiTest.h"
#include <QMainWindow>

class Gui: public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT

public:
  explicit Gui();

private slots:
  void on_red_clicked();
  void on_green_clicked();
  void on_blue_clicked();
};
