#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect((QObject*)ui->textEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollTheFile(int)));
}

MainWindow::~MainWindow()
{
    if(!file){
        file->close();
        delete file;
    }
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit(0);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,"Big File Editor","Opens big files mostly more then GBs");
}

void MainWindow::on_actionOpen_triggered()
{
    QFontMetrics fm(ui->textEdit->currentFont());
    fileName = QFileDialog::getOpenFileName(this,
         tr("Open File"), "~", tr("Any File (*.*)"));

    int lineCount=10;
    lineCount=this->height()/(fm.height());

    file= new QFile(fileName);
    qDebug()<<"açtýmý:"<<file->open(QIODevice::ReadOnly | QIODevice::Text);

    for(int i=0;i<lineCount;i++)
        ui->textEdit->append(file->readLine().trimmed());

    qDebug()<<"file:"<<fileName;
    qDebug()<<"lineCount:"<<lineCount;
}

void MainWindow::scrollTheFile(int neki){
    qDebug()<<"gelen:"<<neki;
}
