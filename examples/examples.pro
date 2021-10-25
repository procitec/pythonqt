TEMPLATE = subdirs
SUBDIRS = CPPPyWrapperExample \
          PyGettingStarted \
          PyCPPWrapperExample \
          PyCustomMetaTypeExample \
          PyLauncher \

lessThan(QT_MAJOR_VERSION, 6) {
  SUBDIRS += PyScriptingConsole \
  SUBDIRS += PyDecoratorsExample \
  SUBDIRS += PyGuiExample \
}
