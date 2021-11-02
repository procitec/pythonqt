#include "gui.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>

int main( int argc, char **argv )
{
    QApplication qapp(argc, argv);

    Gui gui;
    if( argc > 1) {
        QString file_name(argv[1]);
        std::cout << "Auto test enabled by passing a file " << argv[1] << std::endl;
        QFile test_file(file_name);
        if(test_file.exists() ) {
            // disable gui

            // change context
            gui.switch_to_meta_ctx();

            // read file
            if (!test_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                std::cout << "Error. File " << argv[1] << " could not be opened!" << std::endl;
                exit(1);
            }
            QTextStream s(&test_file);
            QString content(s.readAll());
            gui.execute(content);

        } else {
            std::cout << "Error. File " << argv[1] << " not found!" << std::endl;
            exit(1);
        }
    }
    gui.show();

    return qapp.exec();
}

