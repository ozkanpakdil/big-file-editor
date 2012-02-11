#include "texteditor.h"

#include <QKeyEvent>
#include <QDebug>

TextEditor::TextEditor(QWidget *parent) :
    QPlainTextEdit(parent)
{
}

void TextEditor::keyPressEvent ( QKeyEvent * event ){
    QPlainTextEdit::keyPressEvent(event);

    switch (event->key()) {
        case Qt::Key_Down:
            emit goLine(1);
        break;
        case Qt::Key_Up:
            emit goLine(-1);
        break;
        case Qt::Key_PageUp:
            emit goPage(-1);
        break;
        case Qt::Key_PageDown:
            emit goPage(1);
        break;
    }
    if(event->key()==Qt::Key_End && event->modifiers()==Qt::ControlModifier){
        emit goToEOF();
    }

}

