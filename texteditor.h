#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QPlainTextEdit>

class TextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TextEditor(QWidget *parent = 0);

protected:
    void keyPressEvent ( QKeyEvent * event );

signals:
    void goLine(int);
    void goPage(int);
    void goToEOF();

public slots:
    
};

#endif // TEXTEDITOR_H
