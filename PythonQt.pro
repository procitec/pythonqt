TEMPLATE = subdirs

CONFIG += debug

CONFIG += ordered

equals(QT_MAJOR_VERSION, 6) {
    message( "Qt6 build detected")
    DEFINES += "QT6_BUILD"
}

lessThan(QT_MAJOR_VERSION, 6) {
    error("Qt6 or higher required")
}

SUBDIRS = generator src extensions tests examples gui_tests
