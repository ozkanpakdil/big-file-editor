#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QString fileName;

private slots:
    void on_actionOpen_triggered();
    void on_actionAbout_triggered();
    void on_actionQuit_triggered();
};

#endif // MAINWINDOW_H
