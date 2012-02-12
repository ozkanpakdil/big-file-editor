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
#include <QTextStream>
#include <QFile>

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
    currentFileLineNumber=0;

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
    ui->textEdit->clear();
    QFontMetrics fm(ui->textEdit->font());
    fileName = QFileDialog::getOpenFileName(this,
         tr("Open File"), "~", tr("Any File (*.*)"));

    pageSize=this->height()/(fm.height());

    file= new QFile(fileName);
    qDebug()<<"file opened:"<<file->open(QIODevice::ReadOnly | QIODevice::Text);
    currentFileLineNumber=0;

    for(int i=0;i<pageSize;i++){
        ui->textEdit->appendPlainText(file->readLine().trimmed());
        currentFileLineNumber++;
    }

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
    if(ui->textEdit->textCursor().atEnd())
        if(sign==1){//key arrow down
            if(!file->atEnd()){
                ui->textEdit->appendPlainText(file->readLine().trimmed());
                currentFileLineNumber++;
            }
        }
    if(sign==-1){//key arrow up
    }
}
void MainWindow::goPage(int sign){
    qDebug()<<"goPage:"<<sign;
    if(!isFileOpened())
        return;
    if(ui->textEdit->textCursor().atEnd())
        if(sign==1){//page down
            for(int i=0;i<pageSize;i++)
                if(!file->atEnd()){
                    ui->textEdit->appendPlainText(file->readLine().trimmed());
                    currentFileLineNumber++;
                }
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
        if(ui->textEdit->textCursor().atEnd())
            for(int i=0;i<pageSize;i++)
                 if(!file->atEnd()){
                     ui->textEdit->appendPlainText(file->readLine().trimmed());
                     currentFileLineNumber++;
                 }
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
            if(!file->atEnd()){
                ui->textEdit->appendPlainText(file->readLine().trimmed());
                currentFileLineNumber++;
            }
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
    while(!file->atEnd()){
        bufferim.append( file->readLine() );
        i++;
        if(i%10000==0){
            ui->textEdit->clear();
            ui->textEdit->appendPlainText(bufferim);
            bufferim.clear();
            qApp->processEvents();
            qDebug()<<"date:"<<QTime::currentTime()<<"file line:"<<i;
        }
    }
    currentFileLineNumber=i;
    if(!bufferim.isEmpty())
        ui->textEdit->appendPlainText(bufferim);

    qDebug()<<"file line:"<<i;
    ui->textEdit->setVisible(true);
}

void MainWindow::cursorPositionChanged(){
    long currentLineInTheEditor=ui->textEdit->textCursor().blockNumber();

    ui->statusBar->showMessage("current line in editor:" + QString::number( currentLineInTheEditor ) +" in file:" + QString::number(currentFileLineNumber));
}

void MainWindow::on_actionSave_triggered()
{
    if(!isFileOpened()){
        QMessageBox::warning(this,"Open File First","You need to open a file first.");
        return;
    }
    ui->textEdit->setVisible(false);
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save File"), fileName+".out", tr("Any File (*.*)"));
    if (saveFileName.isEmpty())
             return;
    qDebug()<<"saveFileName:"<<saveFileName<< "currentFileLineNumber:"<<currentFileLineNumber
           <<"ui->textEdit->textCursor().blockNumber()"<<ui->textEdit->textCursor().blockNumber()
              ;
    QString plainTextEditContents = ui->textEdit->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");
    qDebug()<<"lines:"<<lines.size();

    if(currentFileLineNumber != lines.size()){
        //NOTE:user is at the end of file and we need to do some preprocess
        //for(QTextBlock block = document()->begin(); block.isValid(); block = block.next()) {}
        QMessageBox::information(this, tr("not implemented"),"not implemented yet.");
        return;
    }else{
        //NOTE:editor and file is sync so we just need to save all editor to the file and add if there is anymore line from opened file.
        QFile fileSave(saveFileName);
        if (!fileSave.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),fileSave.errorString());
            return;
        }
        qDebug()<<"file opened.";
        QTextStream out(&fileSave);
        ui->statusBar->showMessage("please wait until save done...");

        file->seek(0);
        //file->close();
        //file->open(QIODevice::ReadOnly | QIODevice::Text);

        int sayac=0;
        foreach(QString line,lines){
            out<<line<<'\n';
            file->readLine();
            sayac++;
            if(sayac%1000){
                qApp->processEvents();
            }
        }

        while(!file->atEnd()){
            out<<file->readLine();
            sayac++;
            if(sayac%1000){
                qApp->processEvents();
            }
        }
        qDebug()<<"file closed.";
        fileSave.close();
    }
    ui->textEdit->setVisible(true);
}
