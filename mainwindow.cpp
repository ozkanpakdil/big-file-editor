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

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    file = nullptr;

    connect(ui->textEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollTheFile(int)));
    connect(ui->textEdit, SIGNAL(goLine(int)), this, SLOT(goLine(int)));
    connect(ui->textEdit, SIGNAL(goPage(int)), this, SLOT(goPage(int)));
    connect(ui->textEdit, SIGNAL(goToEOF()), this, SLOT(goToEOF()));

    connect(ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    currentFileLineNumber = 0;
}

MainWindow::~MainWindow()
{
    if (file)
    {
        file->close();
        delete file;
    }
    delete ui;
}

void MainWindow::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
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
    QMessageBox::about(this, "Big File Editor",
                       "Opens big files mostly more then GBs <br />for any problem feel free to contact via <a href='http://ozkanpakdil.github.io'>my site</a>");
}

void MainWindow::on_actionOpen_triggered()
{
    QFontMetrics fm(ui->textEdit->font());
    QString tmpFileName = QFileDialog::getOpenFileName(this,
                                                       tr("Open File"), "~", tr("Any File (*.*)"));

    if (tmpFileName == "~" || tmpFileName == "")
        return;
    fileName = tmpFileName;

    ui->textEdit->clear();
    pageSize = this->height() / (fm.height());
    if (pageSize <= 0) pageSize = 20; // fallback

    if (file) {
        file->close();
        delete file;
    }
    file = new QFile(fileName);
    qDebug() << "file opened:" << file->open(QIODevice::ReadOnly | QIODevice::Text);
    currentFileLineNumber = 0;

    QString buffer;
    for (int i = 0; i < pageSize; i++)
    {
        QString line = this->readLineFromFile();
        if (line.isEmpty() && file->atEnd()) break;
        buffer.append(line);
        buffer.append('\n');
    }
    ui->textEdit->setWordWrapMode(QTextOption::NoWrap);
    ui->textEdit->setPlainText(buffer);

    qDebug() << "file:" << fileName;
    qDebug() << "lineCount:" << pageSize;
}

void MainWindow::scrollTheFile(int lineIndex)
{
    if (!isFileOpened())
        return;

    if (lineNumberFromWheel > lineIndex)
    {
        //NOTE:scrolling up
    }
    else
    {
        //scrolling down
    }
    lineNumberFromWheel = lineIndex;
    //qDebug()<<"line index:"<<lineIndex;
}

QString MainWindow::readLineFromFile()
{
    if (!file->atEnd())
    {
        QString line = file->readLine();
        if (line.endsWith('\n'))
            line.chop(1);
        if (line.endsWith('\r'))
            line.chop(1);
        currentFileLineNumber++;
        return line;
    }
    return "";
}

void MainWindow::goLine(int sign)
{
    qDebug() << "goLine:" << sign;
    if (!isFileOpened())
        return;

    if (sign == 1)
    {
        //key arrow down
        QScrollBar* vScrollBar = ui->textEdit->verticalScrollBar();
        if (vScrollBar->value() == vScrollBar->maximum())
        {
            auto chars = this->readLineFromFile();
            if (chars.length() > 0)
                ui->textEdit->appendPlainText(chars);
        }
    }
    if (sign == -1)
    {
        //key arrow up
    }
}

void MainWindow::goPage(int sign)
{
    qDebug() << "goPage:" << sign;
    if (!isFileOpened())
        return;

    if (sign == 1)
    {
        //page down
        QScrollBar* vScrollBar = ui->textEdit->verticalScrollBar();
        if (vScrollBar->value() == vScrollBar->maximum())
        {
            for (int i = 0; i < pageSize; i++)
            {
                QString s = this->readLineFromFile();
                if (s.length() > 0)
                    ui->textEdit->appendPlainText(s);
            }
        }
    }
    if (sign == -1)
    {
        //page up
    }
}

void MainWindow::wheelEvent(QWheelEvent* turning)
{
    if (!isFileOpened())
        return;
    if (turning->angleDelta().y() > 0)
    {
        //mouse wheel up
    }
    else
    {
        //mouse wheel down
        ui->textEdit->setDisabled(true);

        QScrollBar* vScrollBar = ui->textEdit->verticalScrollBar();
        if (vScrollBar->value() == vScrollBar->maximum())
        {
            for (int i = 0; i < pageSize; i++)
            {
                QString s = this->readLineFromFile();
                if (s.length() > 0)
                {
                    ui->textEdit->appendPlainText(s);
                }
            }
        }

        ui->textEdit->setDisabled(false);
        qApp->processEvents();
    }
}

void MainWindow::on_actionGo_to_line_triggered()
{
    if (!isFileOpened())
    {
        QMessageBox::warning(this, "Open File First", "You need to open a file first.");
        return;
    }

    bool ok;
    int gotoLineNumber = QInputDialog::getInt(this, tr("Input Line Number"),
                                              tr("Line Number:"), 25, 0, 100, 1, &ok);
    if (ok && gotoLineNumber > ui->textEdit->document()->lineCount())
    {
        ui->textEdit->setVisible(false);
        ui->textEdit->clear();

        QString bufferim;
        for (int i = ui->textEdit->document()->lineCount(); i < gotoLineNumber; i++)
        {
            if (!file->atEnd())
            {
                bufferim.append(file->readLine());
                currentFileLineNumber++;
            }
            if (currentFileLineNumber % 10000 == 0)
            {
                ui->textEdit->clear();
                ui->textEdit->appendPlainText(bufferim);
                bufferim.clear();
                qApp->processEvents();
            }
        }
        if (bufferim != "")
        {
            ui->textEdit->appendPlainText(bufferim);
            qApp->processEvents();
        }
        ui->textEdit->setVisible(true);
    }
    if (ok && gotoLineNumber < ui->textEdit->document()->lineCount())
    {
        int pos = ui->textEdit->document()->findBlockByLineNumber(gotoLineNumber - 1).position();
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.setPosition(pos);
        ui->textEdit->setTextCursor(cursor);
        qDebug() << "gone:" << gotoLineNumber;
    }
}

long MainWindow::findTotalFileLineNumbers()
{
    qint64 pozisyon = file->pos();
    file->seek(0);

    long sayac = 0;
    while (!file->atEnd())
    {
        file->readLine();
        sayac++;
        if (sayac % 10000)
        {
            qApp->processEvents();
        }
    }
    file->seek(pozisyon);
    return sayac;
}

void MainWindow::goToEOF()
{
    if (!isFileOpened())
        return;
    ui->textEdit->setVisible(false);
    long i = 0;
    QString bufferim;
    while (!file->atEnd())
    {
        bufferim.append(file->readLine());
        i++;
        if (i % 10000 == 0)
        {
            ui->textEdit->clear();
            ui->textEdit->appendPlainText(bufferim);
            bufferim.clear();
            qApp->processEvents();
            qDebug() << "date:" << QTime::currentTime() << "file line:" << i;
            currentFileLineNumber = i;
        }
    }
    currentFileLineNumber = i;
    if (!bufferim.isEmpty())
        ui->textEdit->appendPlainText(bufferim);

    qDebug() << "file line:" << i;
    ui->textEdit->setVisible(true);
}

void MainWindow::cursorPositionChanged()
{
    long currentLineInTheEditor = ui->textEdit->textCursor().blockNumber();

    ui->statusBar->showMessage("current line in editor:" + QString::number(currentLineInTheEditor) + " in file:" +
        QString::number(currentFileLineNumber));
}

void MainWindow::on_actionSave_triggered()
{
    if (!isFileOpened())
    {
        QMessageBox::warning(this, "Open File First", "You need to open a file first.");
        return;
    }
    ui->textEdit->setVisible(false);
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save File"), fileName + ".out", tr("Any File (*.*)"));
    if (saveFileName.isEmpty())
        return;
    qDebug() << "saveFileName:" << saveFileName << "currentFileLineNumber:" << currentFileLineNumber
        << "ui->textEdit->textCursor().blockNumber()" << ui->textEdit->textCursor().blockNumber();
    QString plainTextEditContents = ui->textEdit->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");
    qDebug() << "lines:" << lines.size();
    qint64 pozisyon = file->pos();

    if (currentFileLineNumber != lines.size())
    {
        //NOTE:user is at the end of file and we need to do some preprocess
        //for(QTextBlock block = document()->begin(); block.isValid(); block = block.next()) {}
        QFile fileSave(saveFileName);
        if (!fileSave.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"), fileSave.errorString());
            return;
        }
        qDebug() << "file opened.";
        QTextStream out(&fileSave);

        file->seek(0);
        long totalLineCount = this->findTotalFileLineNumbers();
        ui->statusBar->showMessage("please wait until save done... total lines:" + QString::number(totalLineCount));

        int sayac = 0;

        while (sayac < currentFileLineNumber - lines.size())
        {
            out << file->readLine();
            sayac++;
            if (sayac % 1000)
            {
                qApp->processEvents();
            }
        }


        foreach(QString line, lines)
        {
            out << line << '\n';
            file->readLine();
            sayac++;
            if (sayac % 1000)
            {
                qApp->processEvents();
            }
        }

        while (!file->atEnd())
        {
            out << file->readLine();
            sayac++;
            if (sayac % 1000)
            {
                qApp->processEvents();
            }
        }
        qDebug() << "file closed.";
        fileSave.close();
    }
    else
    {
        //NOTE:editor and file is sync so we just need to save all editor to the file and add if there is anymore line
        //from opened file.
        QFile fileSave(saveFileName);
        if (!fileSave.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"), fileSave.errorString());
            return;
        }
        qDebug() << "file opened.";
        QTextStream out(&fileSave);
        ui->statusBar->showMessage("please wait until save done...");

        file->seek(0);

        int sayac = 0;
        foreach(QString line, lines)
        {
            out << line << '\n';
            file->readLine();
            sayac++;
            if (sayac % 1000)
            {
                qApp->processEvents();
            }
        }

        while (!file->atEnd())
        {
            out << file->readLine();
            sayac++;
            if (sayac % 1000)
            {
                qApp->processEvents();
            }
        }
        qDebug() << "file closed.";
        fileSave.close();
    }
    file->seek(pozisyon);
    ui->textEdit->setVisible(true);
}

void MainWindow::on_actionSearch_triggered()
{
    if (!isFileOpened())
        return;
    bool ok;
    QString searchWord = QInputDialog::getText(this, tr("Input Search Phrase"),
                                               tr("Word:"), QLineEdit::Normal,
                                               "hele", &ok);

    if (ok && !searchWord.isEmpty())
    {
        ui->textEdit->setVisible(false);
        file->seek(0);

        long i = 0;
        QString bufferim;
        while (!file->atEnd())
        {
            bufferim.append(file->readLine());
            i++;
            if (i % 10000 == 0)
            {
                ui->textEdit->clear();
                ui->textEdit->appendPlainText(bufferim);
                bufferim.clear();
                qApp->processEvents();
                qDebug() << "date:" << QTime::currentTime() << "file line:" << i;
                currentFileLineNumber = i;
                if (!ui->textEdit->document()->find(searchWord, Qt::CaseInsensitive).isNull())
                {
                    qDebug() << "found the word";
                    break;
                }
            }
        }
        currentFileLineNumber = i;
        if (!bufferim.isEmpty())
        {
            ui->textEdit->appendPlainText(bufferim);
            if (!ui->textEdit->document()->find(searchWord, Qt::CaseInsensitive).isNull())
            {
                qDebug() << "found the word";
            }
        }

        ui->textEdit->setVisible(true);
        if (!ui->textEdit->document()->find(searchWord, Qt::CaseInsensitive).isNull())
        {
            QTextCursor cursor = ui->textEdit->document()->find(searchWord, Qt::CaseInsensitive);
            cursor.beginEditBlock();
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
            cursor.endEditBlock();
            ui->textEdit->setTextCursor(cursor);
        }
    }
}

void MainWindow::on_actionGo_to_end_of_file_triggered()
{
    goToEOF();
}
