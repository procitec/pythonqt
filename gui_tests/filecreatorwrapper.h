#pragma once

#include "PythonQtCppWrapperFactory.h"
#include "filehandler.h"
#include <QString>


class FileHandlerWrapper: public QObject
{
    Q_OBJECT
public:
    FileHandlerWrapper(FileHandler* pFileHandler)
    : mpFileHandler(pFileHandler){
    }

public slots:
    bool create_file() {
        return mpFileHandler->createFile();
    }

    bool file_exists() const {
        return mpFileHandler->fileExists();
    }

    bool delete_file() {
        return mpFileHandler->deleteFile();
    }

private:
    FileHandler* mpFileHandler;
};

class FileHandlerFactory: public PythonQtCppWrapperFactory
{
public:
    FileHandlerFactory()
    {}

    virtual QObject* create( const QByteArray& name, void * ptr ) override
    {
        if( name == "FileHandler" )
        {
            return new FileHandlerWrapper((FileHandler*)ptr);
        }
        return nullptr;
    }
};