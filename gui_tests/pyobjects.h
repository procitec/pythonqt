#pragma once

#include "gui.h"

class PyGUI: public QObject
{
    Q_OBJECT
public:
    PyGUI(Gui* pGUI);

public slots:
    void red() const;
    void green() const;
    void blue() const;
    void create_file(const QString& filename);
private:
    Gui* mp_GUI;
};

class PyMeta: public QObject
{
    Q_OBJECT

public:
    PyMeta(Gui* pGUI, QPlainTextEdit* pTextEdit);

public slots:
    void clear_output();
    void reset_color();
    FileHandler* create_filehandle(const QString& filename);

private:
    Gui* mp_GUI;
    QPlainTextEdit* mp_TextEdit;
};