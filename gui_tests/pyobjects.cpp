#include "pyobjects.h"

PyGUI::PyGUI(Gui* pGUI)
: mp_GUI(pGUI) {
}

void PyGUI::red() const {
    QMetaObject::invokeMethod(mp_GUI, "on_red_clicked");
}

void PyGUI::green() const {
    QMetaObject::invokeMethod(mp_GUI, "on_green_clicked");
}

void PyGUI::blue() const {
    QMetaObject::invokeMethod(mp_GUI, "on_blue_clicked");
}

PyMeta::PyMeta(Gui* pGUI, QPlainTextEdit* pTextEdit)
        : mp_GUI(pGUI), mp_TextEdit(pTextEdit) {
}

void PyMeta::clear_output() {
    QMetaObject::invokeMethod(mp_TextEdit, "clear");
}

void PyMeta::reset_color() {
    mp_GUI->setStyleSheet("QMainWindow {}");
}

FileHandler *PyMeta::create_filehandle(const QString &filename) {
    return new FileHandler(filename);
}
