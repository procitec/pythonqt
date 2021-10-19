#include "gui.h"

Gui::Gui()
{
    setupUi(this);
    QObject::connect(this->btn_red, &QPushButton::clicked, this, &Gui::on_red_clicked);
    QObject::connect(this->btn_green, &QPushButton::clicked, this, &Gui::on_green_clicked);
    QObject::connect(this->btn_blue, &QPushButton::clicked, this, &Gui::on_blue_clicked);
}

void Gui::on_red_clicked()
{
    this->setStyleSheet("QMainWindow {background: rgb(239, 51, 49);}");
}

void Gui::on_green_clicked()
{
    this->setStyleSheet("QMainWindow {background: rgb(107, 190, 68);}");
}

void Gui::on_blue_clicked()
{
    this->setStyleSheet("QMainWindow {background: rgb(30, 145, 202);}");
}
