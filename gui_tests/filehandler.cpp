#include "filehandler.h"

#include <QDate>

FileHandler::FileHandler(const QString& filename) {
    this->mFile.setFileName(filename);
}

bool FileHandler::createFile() {
    this->mFile.open(QIODevice::WriteOnly);
    auto dateString = QDate::currentDate().toString();
    this->mFile.write(dateString.toLocal8Bit());
    this->mFile.close();
    return true;
}

bool FileHandler::fileExists() const {
    return this->mFile.exists();
}
bool FileHandler::deleteFile() {
    return this->mFile.remove();
}
