#lessThan(QT_MAJOR_VERSION, 6): error("requires Qt 6")

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
    big-file-editor.rc \
    license.txt \
    beforeinstall.txt \
    afterinstall.txt


RC_FILE = big-file-editor.rc

QT += core widgets network gui concurrent
