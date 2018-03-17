TARGET = big-file-editor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    texteditor.cpp

HEADERS  += mainwindow.h \
    version.h \
    texteditor.h \
    BFEApplication.h

FORMS    += mainwindow.ui

RESOURCES +=

OTHER_FILES += \
    big-file-editor.rc \
    license.txt \
    beforeinstall.txt \
    afterinstall.txt


RC_FILE = big-file-editor.rc

QT += widgets network gui
