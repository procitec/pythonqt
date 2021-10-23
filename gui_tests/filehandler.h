#pragma once

#include <QFile>

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