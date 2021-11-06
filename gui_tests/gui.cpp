#include "gui.h"
#include "pyobjects.h"
#include "filedec.h"

#include <QShortcut>


/* MainWindow class.
 * Connect buttons and initializes two contexts: GUI and context GUI + meta.
 * See README.md for detailed description of available functions */
Gui::Gui()
{
    setupUi(this);
    PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
    qRegisterMetaType<FileHandler>("FileHandler");
    qRegisterMetaType<FileDec>("FileDec");
    PythonQt::self()->addWrapperFactory(new FileHandlerFactory());
    PythonQt::self()->registerCPPClass("FileHandler");
    PythonQt::self()->registerCPPClass("FileDec", "","example", PythonQtCreateObject<FileDecWrapper>);

    QObject::connect(this->btn_red, &QPushButton::clicked, this, &Gui::on_red_clicked);
    QObject::connect(this->btn_green, &QPushButton::clicked, this, &Gui::on_green_clicked);
    QObject::connect(this->btn_blue, &QPushButton::clicked, this, &Gui::on_blue_clicked);
    QObject::connect(this->btn_execute, &QPushButton::clicked, this, &Gui::on_execute_clicked);

    // connect std out and std err to log output
    QObject::connect(PythonQt::self(), &PythonQt::pythonStdOut, this->output, &QPlainTextEdit::appendPlainText);
    QObject::connect(PythonQt::self(), &PythonQt::pythonStdErr, this->output, &QPlainTextEdit::appendPlainText);

    QObject::connect(this->cb_context, &QComboBox::currentTextChanged, this, &Gui::on_context_changed);

    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(on_execute_clicked()));

    this->cb_context->addItem("Ctx GUI");
    this->cb_context->addItem("Ctx GUI + meta");

    mp_pygui = new PyGUI(this);
    mp_meta = new PyMeta(this, this->output);
}

Gui::~Gui()
{
    delete mp_pygui;
    delete mp_meta;
}


void Gui::switch_to_meta_ctx() {
    this->cb_context->setCurrentIndex(1);
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
    execute(script);
}

void Gui::execute(const QString& content) {
    auto idx = this->cb_context->currentIndex();
    this->output->clear();
    this->output->appendPlainText( "--- Start script --- ");

    // check for current selected context
    if( idx == 0 ) {
        auto ctx = PythonQt::self()->createUniqueModule();
        ctx.addObject("gui", mp_pygui );
        ctx.evalScript(content);
    }
    else if( idx == 1 ) {
        auto ctx = PythonQt::self()->createUniqueModule();
        ctx.addObject("gui", mp_pygui );
        ctx.addObject("meta", mp_meta );
        ctx.evalScript(content);
    }
    this->output->appendPlainText( "--- Finished --- ");
}

/* Change "help text" depending on selected context */
void Gui::on_context_changed(const QString& text) {
    this->ctx_description->clear();
   if( text == "Ctx GUI") {
       QString description = "Context GUI\n\n";
       description.append("gui.red()\n");
       description.append("gui.green()\n");
       description.append("gui.blue()\n\n");
       description.append("f = meta.create_filehandle(filename)\n\n");
       description.append("from PythonQt.example import FileDec\n");
       description.append("fd = FileDec(\"test.txt\")\n");
       this->ctx_description->appendPlainText(description);

   } else if( text == "Ctx GUI + meta") {
       QString description = "Context GUI + meta\n\n";
       description.append("gui.red()\n");
       description.append("gui.green()\n");
       description.append("gui.blue()\n\n");
       description.append("meta.clear_output()\n");
       description.append("meta.reset_color()\n");
       description.append("f = meta.create_filehandle(filename)\n\n");
       description.append("from PythonQt.example import FileDec\n");
       description.append("fd = FileDec(\"test.txt\")\n");
       this->ctx_description->appendPlainText(description);
   }

}
