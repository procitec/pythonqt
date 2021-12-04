#pragma once

#include <QFile>

/* This is created by the FileHandlerFactory in FileCreatorWrapper */

class FileHandler
{
public:
    FileHandler(const QString& filename);
    bool createFile();
    bool fileExists() const;
    bool deleteFile();
private:
    QFile mFile;
};