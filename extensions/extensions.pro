TEMPLATE = subdirs
lessThan(QT_MAJOR_VERSION, 6) {
	SUBDIRS = PythonQt_QtAll
}

equals(QT_MAJOR_VERSION, 6) {
	SUBDIRS = 
}
