TARGET = big-file-editor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    texteditor.cpp

HEADERS  += mainwindow.h \
    version.h \
    texteditor.h

FORMS    += mainwindow.ui

RESOURCES +=

OTHER_FILES += \
    big-file-editor.rc


RC_FILE = big-file-editor.rc
