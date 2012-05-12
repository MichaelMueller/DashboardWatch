#ifndef DashboardWatchLogger_H
#define DashboardWatchLogger_H

#include <QString>
#include <fstream>
 
class DashboardWatchLogger
{
public:
    static void Initialize( const QString& _LogFilePath="" );
    static void Log(QtMsgType type, const char *msg);
private:
    static QString m_LogFilePath;
    static std::ofstream m_LogFile;

};

#endif
