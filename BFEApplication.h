#ifndef BFEAPPLICATION_H
#define BFEAPPLICATION_H

#include <QtGui>
#include <QApplication>

class BFEApplication : public QApplication {
public:
    BFEApplication(int argc, char *argv[]) : QApplication(argc, argv) { }
    virtual ~BFEApplication() { }

    // reimplemented from QApplication so we can throw exceptions in slots
    virtual bool notify(QObject * receiver, QEvent * event) {
        try {
            return QApplication::notify(receiver, event);
        } catch(std::exception& e) {
            qCritical() << "Exception thrown:" << e.what();
        }
        return false;
    }
};

#endif // BFEAPPLICATION_H
