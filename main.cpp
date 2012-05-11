#include <QtGui>
#include <QSharedMemory>
#include "DashboardWatch.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // assert only one application
    QString UniqueID("DashboardWatch");
    QSharedMemory sharedMemory;
    sharedMemory.setKey(UniqueID);

    if (!sharedMemory.create(1)) {
      QMessageBox::critical(0, QObject::tr("DashboardWatch"),
                            QObject::tr("Another DashboardWatch is already running. Will exit now."));
	    return 1; // Exit already a process running 
    }

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("DashboardWatch"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }

    DashboardWatch DashboardWatch;
    DashboardWatch.show();
    DashboardWatch.hide();
    return app.exec();
}
