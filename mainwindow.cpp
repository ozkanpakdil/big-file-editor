#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QWheelEvent>
#include <QInputDialog>
#include <QScrollBar>
#include <QTextCursor>
#include <QTextBlock>
#include <QStringBuilder>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect((QObject*)ui->textEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollTheFile(int)));
    connect((QObject*)ui->textEdit, SIGNAL(goLine(int)), this, SLOT(goLine(int)));
    connect((QObject*)ui->textEdit, SIGNAL(goPage(int)), this, SLOT(goPage(int)));
    connect((QObject*)ui->textEdit, SIGNAL(goToEOF()), this, SLOT(goToEOF()));

    connect((QObject*)ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

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
    QFontMetrics fm(ui->textEdit->font());
    fileName = QFileDialog::getOpenFileName(this,
         tr("Open File"), "~", tr("Any File (*.*)"));

    pageSize=this->height()/(fm.height());

    file= new QFile(fileName);
    qDebug()<<"file opened:"<<file->open(QIODevice::ReadOnly | QIODevice::Text);

    for(int i=0;i<pageSize;i++)
        ui->textEdit->appendPlainText(file->readLine().trimmed());

    qDebug()<<"file:"<<fileName;
    qDebug()<<"lineCount:"<<pageSize;
}

void MainWindow::scrollTheFile(int lineIndex){
    if(!isFileOpened())
        return;

    if(lineNumberFromWheel > lineIndex){
        //NOTE:scrolling up

    }else{
        //scrolling down
    }
    lineNumberFromWheel=lineIndex;
    //qDebug()<<"line index:"<<lineIndex;
}

void MainWindow::goLine(int sign){
    qDebug()<<"goLine:"<<sign;
    if(!isFileOpened())
        return;

    if(sign==1){//key arrow down
        if(!file->atEnd())
            ui->textEdit->appendPlainText(file->readLine().trimmed());
    }
    if(sign==-1){//key arrow up
    }
}
void MainWindow::goPage(int sign){
    qDebug()<<"goPage:"<<sign;
    if(!isFileOpened())
        return;
    if(sign==1){//page down
        for(int i=0;i<pageSize;i++)
            if(!file->atEnd())
                ui->textEdit->appendPlainText(file->readLine().trimmed());
    }
    if(sign==-1){//page up
    }
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
                 ui->textEdit->appendPlainText(file->readLine().trimmed());
    }
    //qDebug()<<"pos:"<< file->pos();
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
                                              0,999999999,10,&ok);
    if(ok && gotoLineNumber>ui->textEdit->document()->lineCount()){
        ui->textEdit->clear();
        for(int i=ui->textEdit->document()->lineCount();i<gotoLineNumber;i++){
            if(!file->atEnd())
                ui->textEdit->appendPlainText(file->readLine().trimmed());
            qApp->processEvents();
        }

    }
    if(ok && gotoLineNumber<ui->textEdit->document()->lineCount()){
        int pos = ui->textEdit->document()->findBlockByLineNumber(gotoLineNumber-1).position();
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.setPosition(pos);
        ui->textEdit->setTextCursor(cursor);
        qDebug()<<"gone:"<<gotoLineNumber;
    }
}

void MainWindow::goToEOF(){
    if(!isFileOpened())
        return;
    ui->textEdit->setVisible(false);
    long i=0;
    QString bufferim;
    try{
        while(!file->atEnd()){
            bufferim.append( file->readLine() );
            i++;
            if(i%10000==0){
                ui->textEdit->appendPlainText(bufferim);
                //ui->textEdit->document()->setPlainText(ui->textEdit->document()->toPlainText().append( bufferim));
                bufferim.clear();
                qApp->processEvents();
                qDebug()<<"date:"<<QTime::currentTime()<<"file line:"<<i;
            }
        }
        if(!bufferim.isEmpty())
            ui->textEdit->appendPlainText(bufferim);
    }catch(...){

    }
    qDebug()<<"file line:"<<i;
    ui->textEdit->setVisible(true);
}

void MainWindow::cursorPositionChanged(){
    //ui->statusBar->showMessage("current line:" + QString::number( ui->textEdit->textCursor().blockNumber()) );
}
