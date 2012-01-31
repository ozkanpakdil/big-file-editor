#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QWheelEvent>
#include <QInputDialog>
#include <QScrollBar>

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

    pageSize=this->height()/(fm.height());

    file= new QFile(fileName);
    qDebug()<<"file opened:"<<file->open(QIODevice::ReadOnly | QIODevice::Text);

    for(int i=0;i<pageSize;i++)
        ui->textEdit->append(file->readLine().trimmed());

    qDebug()<<"file:"<<fileName;
    qDebug()<<"lineCount:"<<pageSize;
}

void MainWindow::scrollTheFile(int lineIndex){
    if(fileName.trimmed()=="")
        return;

    if(!file->isOpen())
        return;

    if(lineNumberFromWheel > lineIndex){
        //NOTE:scrolling up

    }else{
        //scrolling down
    }
    lineNumberFromWheel=lineIndex;
    //qDebug()<<"line index:"<<lineIndex;
}

void MainWindow::wheelEvent(QWheelEvent *turning){
    if(fileName.trimmed()=="")
        return;
    //qDebug()<<"wheel:"<<turning->delta();
    if(turning->delta()>0){
        //mouse wheel up
    }else{
        //mouse wheel down
        for(int i=0;i<pageSize;i++)
             if(!file->atEnd())
                 ui->textEdit->append(file->readLine().trimmed());
    }
    qDebug()<<"pos:"<< file->pos();
}

void MainWindow::on_actionGo_to_line_triggered()
{
    if(!isFileOpened()){
        QMessageBox::warning(this,"Open File First","You need to open a file first.");
        return;
    }

    bool ok;
    int gotoLineNumber = QInputDialog::getInteger(this, tr("Input Line Number"),
                                              tr("Line Number:"), QLineEdit::Normal,
                                              0,99999999999999999,10,&ok);
    if(ok && gotoLineNumber>ui->textEdit->document()->lineCount()){
        qDebug()<<"pos:"<< file->pos();
        /*file->seek(gotoLineNumber);
        for(int i=ui->textEdit->document()->lineCount();i<gotoLineNumber-pageSize;i++){
            file->readLine();
        }*/
        ui->textEdit->clear();
        for(int i=ui->textEdit->document()->lineCount();i<gotoLineNumber;i++){
            if(!file->atEnd())
                ui->textEdit->append(file->readLine().trimmed());
            qApp->processEvents();
        }
    }
    /*
    ui->textEdit->textCursor().setPosition(gotoLineNumber);
    ui->textEdit->moveCursor(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
    QScrollBar *scroll =ui->textEdit->verticalScrollBar();
    scroll->scroll(10,0);*/

}
