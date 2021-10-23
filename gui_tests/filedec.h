#pragma once
#include "PythonQt.h"
#include "PythonQtCppWrapperFactory.h"
#include <QObject>
#include <QFile>

class FileDec {
public:
    FileDec(const QString filename): mFile(filename) {}

    bool exists() const {
        return mFile.exists();
    }

private:
    QFile mFile;
};

// add a decorator that allows to access the CustomObject from PythonQt
class FileDecWrapper : public QObject {

Q_OBJECT

public slots:

    // add a constructor
    FileDec* new_FileDec(const QString& filename)  { return new FileDec(filename); }

    void delete_FileDec(FileDec *o) { delete o; }

    // add access methods
    bool exists(FileDec* o) { return o->exists(); }
};
