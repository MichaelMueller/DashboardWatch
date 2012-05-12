#include <QtGui>
#include <QSharedMemory>
#include <string>
#include "DashboardWatchLogger.h"
#include "DashboardWatch.h"

int main(int argc, char *argv[])
{
  // int qapp
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("DashboardWatch");
    QCoreApplication::setOrganizationName("DKFZ");

    // assert only one application
    QString UniqueID("DashboardWatch");
    QSharedMemory sharedMemory;
    sharedMemory.setKey(UniqueID);

    if (!sharedMemory.create(1)) {
      QMessageBox::critical(0, QObject::tr("DashboardWatch"),
                            QObject::tr("Another DashboardWatch is already running. Will exit now."));
	    return 1; // Exit already a process running 
    }

    // parse cmd args
    QString logFile;
    bool logfileFound = false;
    for( int i = 0; i < argc; i++ )
    {
      if( logfileFound )
      {
        logFile = argv[i]; 
        logfileFound = false;
      }
      else if( std::string(argv[i]) == "--logfile" )
        logfileFound = true; 
    }

    // init logging
    DashboardWatchLogger::Initialize(logFile);
    qDebug() << "Application started";

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
