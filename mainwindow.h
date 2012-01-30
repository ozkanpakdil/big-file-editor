#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool isFileOpened(){
        if(fileName.trimmed()=="")
            return false;
        return true;
    }

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QString fileName;
    QFile *file;
    int lineNumberFromWheel;
    int pageSize;

private slots:
    void on_actionOpen_triggered();
    void on_actionAbout_triggered();
    void on_actionQuit_triggered();
    void scrollTheFile(int);
    void wheelEvent(QWheelEvent*);
    void on_actionGo_to_line_triggered();
};

#endif // MAINWINDOW_H
