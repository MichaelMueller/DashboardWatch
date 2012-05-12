#include "DashboardWatchLogger.h"
#include <fstream>
#include <iostream>
#include <QTime>
#include <QMessageBox>

// member
QString DashboardWatchLogger::m_LogFilePath("");
std::ofstream DashboardWatchLogger::m_LogFile;

// methods
void DashboardWatchLogger::Initialize( const QString& _LogFilePath ) 
{
  m_LogFilePath = _LogFilePath;
  qInstallMsgHandler(DashboardWatchLogger::Log);
}

void DashboardWatchLogger::Log(QtMsgType type, const char *msg) 
{
  std::ostream* stream = &std::cout;
  if( !m_LogFilePath.isEmpty() && !m_LogFile.is_open() )
  {
    try
    {
      m_LogFile.open( m_LogFilePath.toAscii(), std::ios::app);
      stream = &m_LogFile;
    }
    catch(std::exception& e)
    {
      QString errorMsg = QString("Error opening log file %1. Reason: %2").arg(m_LogFilePath).arg(e.what());
      QMessageBox::critical(0, QObject::tr("DashboardWatch"), errorMsg );
      m_LogFilePath = "";
    }    
  }

  switch (type) 
  {
    case QtDebugMsg:
        (*stream) << QTime::currentTime().toString().toAscii().data() << " Info: " << msg << std::endl;
        break;
    case QtCriticalMsg:
        (*stream) << QTime::currentTime().toString().toAscii().data() << " Critical: " << msg << std::endl;
        break;
    case QtWarningMsg:
        (*stream) << QTime::currentTime().toString().toAscii().data() << " Warning: " << msg << std::endl;
        break;
    case QtFatalMsg:
        (*stream) << QTime::currentTime().toString().toAscii().data() <<  " Fatal: " << msg << std::endl;
        abort();
  }
  stream->flush();
}