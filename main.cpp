#include <QtGui>
#include <QLocalSocket>
#include <Qt>
#include <string>
#include "DashboardWatchLogger.h"
#include "DashboardWatch.h"
#include "DashboardWatchConfig.h"

QLocalServer* m_localServer;

int main(int argc, char *argv[])
{
  // int qapp
  QApplication app(argc, argv);

  // assure one application instance
  QString uniqueID( DASHBOARDWATCH_UUID );
  QLocalSocket socket;
  socket.connectToServer(uniqueID);
  if (socket.waitForConnected(500))
  {
      QMessageBox::critical(0, QObject::tr("DashboardWatch"),
                            QObject::tr("Another DashboardWatch is already running. Will exit now."));
      return EXIT_FAILURE; // Exit already a process running
  }

  // some app specific params
  QCoreApplication::setApplicationName( DASHBOARDWATCH_TITLE );
  QCoreApplication::setOrganizationName("DKFZ");

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

  if (!QSystemTrayIcon::isSystemTrayAvailable())
  {
    QMessageBox::critical(0, QObject::tr("DashboardWatch"),
    QObject::tr("I couldn't detect any system tray "
      "on this system."));
    return 1;
  }

  DashboardWatch _DashboardWatch;
  _DashboardWatch.show();
  if( !DashboardWatch::OS_IS_UBUNTU )
    _DashboardWatch.hide();
  return app.exec();
}
