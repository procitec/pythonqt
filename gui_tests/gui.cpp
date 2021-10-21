#include "gui.h"
#include "pyobjects.h"


Gui::Gui()
{
    setupUi(this);
    PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);

    QObject::connect(this->btn_red, &QPushButton::clicked, this, &Gui::on_red_clicked);
    QObject::connect(this->btn_green, &QPushButton::clicked, this, &Gui::on_green_clicked);
    QObject::connect(this->btn_blue, &QPushButton::clicked, this, &Gui::on_blue_clicked);
    QObject::connect(this->btn_execute, &QPushButton::clicked, this, &Gui::on_execute_clicked);

    // connect std out and std err to log output
    QObject::connect(PythonQt::self(), &PythonQt::pythonStdOut, this->output, &QPlainTextEdit::appendPlainText);
    QObject::connect(PythonQt::self(), &PythonQt::pythonStdOut, this->output, &QPlainTextEdit::appendPlainText);

    QObject::connect(this->cb_context, &QComboBox::currentTextChanged, this, &Gui::on_context_changed);
    this->cb_context->addItem("Ctx GUI");
    this->cb_context->addItem("Ctx GUI and clear");

    mp_pygui = new PyGUI(this);
    mp_meta = new PyMeta(this, this->output);
}

Gui::~Gui()
{
    delete mp_pygui;
    delete mp_meta;
}

void Gui::on_red_clicked()
{
    this->setStyleSheet("QMainWindow {background: rgb(239, 51, 49);}");
    this->output->appendPlainText("red button clicked");
}

void Gui::on_green_clicked()
{
    this->setStyleSheet("QMainWindow {background: rgb(107, 190, 68);}");
    this->output->appendPlainText("green button clicked");
}

void Gui::on_blue_clicked()
{
    this->setStyleSheet("QMainWindow {background: rgb(30, 145, 202);}");
    this->output->appendPlainText("blue button clicked");
}

void Gui::on_execute_clicked() {
    auto script = editor->toPlainText();
    auto idx = this->cb_context->currentIndex();
    if( idx == 0 ) {
        auto ctx = PythonQt::self()->createUniqueModule();
        ctx.addObject("gui", mp_pygui );
        ctx.evalScript(script);
    }
    else if( idx == 1 ) {
        auto ctx = PythonQt::self()->createUniqueModule();
        ctx.addObject("gui", mp_pygui );
        ctx.addObject("meta", mp_meta );
        ctx.evalScript(script);
    }
}

void Gui::on_context_changed(const QString& text) {
    this->ctx_description->clear();
   if( text == "Ctx GUI") {
       this->ctx_description->appendPlainText("Decription Ctx GUI");

   } else if( text == "Ctx GUI and clear") {
       this->ctx_description->appendPlainText("Decription Ctx GUI and clear");
   }

}