#ifndef BFEAPPLICATION_H
#define BFEAPPLICATION_H

#include <QtGui>
#include <QApplication>
#include <QUrl>
#include <exception>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

class BFEApplication : public QApplication {
public:
    BFEApplication(int argc, char *argv[]) : QApplication(argc, argv) { }
    virtual ~BFEApplication() { }

    // reimplemented from QApplication so we can throw exceptions in slots
    virtual bool notify(QObject * receiver, QEvent * event) {
        //use https://github.com/AlekSi/breakpad-qt and publish first setup.
        //for setup http://www.jrsoftware.org/isdl.php
        try
        {
            return QApplication::notify(receiver,event);
        }
        catch (const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            qDebug() << exc.what();
        }
        catch(...)
        {
            QMessageBox::warning(0,
                                 tr("An unexpected error occurred"),
                                 tr("This is likely a bug."));
            QNetworkAccessManager *manager = new QNetworkAccessManager(this);
            QString winVer=(QString)QSysInfo::prettyProductName();
            manager->get(QNetworkRequest(QUrl("http://mascix.com/bigfileeditor.jsp?winVer="+winVer)));
            qDebug()<<"http://mascix.com/bigfileeditor.jsp?winVer="+winVer;
        }
        return false;
    }
};

#endif // BFEAPPLICATION_H
