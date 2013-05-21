#include <QtGui>
#include <QFileInfo>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QDesktopServices>
#include <iostream>

#include "DashboardWatch.h"
#include "DashboardWatchLogger.h"
#include <DashboardWatchConfig.h>

const bool DashboardWatch::OS_IS_UBUNTU = DashboardWatch::OsIsUbuntu();

  DashboardWatch::DashboardWatch()
   : m_Manager( new QNetworkAccessManager(this) ),
     m_UpdateTimer(new QTimer(this)),
     m_RedIcon(":/DashboardWatch/red-dot.png"),
     m_GreenIcon(":/DashboardWatch/green-dot.png"),
     m_RedGreenIcon(":/DashboardWatch/red-green-dot.png"),
     m_QuestionIcon(":/DashboardWatch/question.png"),
     m_PreviousResult(-2),
     m_Message(""),
     m_MessageTime(6000),
     m_Title( QString(DASHBOARDWATCH_TITLE) ),
     m_Settings(QSettings::IniFormat,
      QSettings::UserScope,
      QCoreApplication::organizationName(),
      QCoreApplication::applicationName() ),
     m_LocalServer( new QLocalServer(this) ),
     m_Connected(false)
  {
    // setup internals
    //m_UpdateTimer = new QTimer(this);
    m_UpdateTimer->setInterval(2000*60);

    // setup UI
    createOptionsGroupBox();
    createActions();
    createTrayIcon();

    m_StatusIconLabel = new QLabel;
    m_StatusLabel = new QLabel;
    m_StatusLabel->setText(m_Message);

    QHBoxLayout *statuslabelLayout = new QHBoxLayout;
    statuslabelLayout->addWidget(m_StatusIconLabel);
    statuslabelLayout->addWidget(m_StatusLabel);

    QWidget* labelWidget = new QWidget;
    labelWidget->setLayout(statuslabelLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins( 4,4,4,4 );
    mainLayout->addWidget(labelWidget);
    mainLayout->addWidget(m_OptionsGroupBox);

    QWidget* mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);

    setCentralWidget(mainWidget);

    setWindowTitle( m_Title );
    this->setIcon( m_QuestionIcon );

    // connect
    connect(m_UpdateRateSpinBox, SIGNAL( valueChanged (int) ),
            this, SLOT( on_UpdateRateSpinBoxValue_Changed(int) ));

    connect(m_AutoStartCheckBox, SIGNAL( clicked (bool) ),
            this, SLOT( on_AutoStartCheckBox_clicked( bool ) ));

    if( !OS_IS_UBUNTU )
        connect(m_TrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    connect(m_Manager, SIGNAL(finished(QNetworkReply*)),
      this, SLOT(replyFinished(QNetworkReply*)));

    connect(m_UpdateTimer, SIGNAL(timeout()),
            this, SLOT(fetch()));

    connect(m_LocalServer, SIGNAL(newConnection()), this, SLOT(on_NewLocalSocketConnection()));
    m_LocalServer->listen( DASHBOARDWATCH_UUID );

    // show/start
    if( !OS_IS_UBUNTU )
        m_TrayIcon->show();
    m_UpdateTimer->start();
    this->fetch();
  }
  void DashboardWatch::iconActivated(QSystemTrayIcon::ActivationReason reason)
  {
      switch (reason) {
      case QSystemTrayIcon::DoubleClick:
        this->setVisible(!this->isVisible());
          break;
      default:
          ;
      }
  }
 void DashboardWatch::setVisible(bool visible)
 {
     m_RestoreAction->setEnabled(isMaximized() || !visible);
     QWidget::setVisible(visible);
 }

 void DashboardWatch::closeEvent(QCloseEvent *event)
 {     
     if( !OS_IS_UBUNTU  && m_TrayIcon->isVisible() )
     {
         hide();
         event->ignore();
     }
     else
     {
         /*int answer = QMessageBox::question(this, "Attention", "Really close application?", QMessageBox::Yes, QMessageBox::No);
         if( answer == QMessageBox::Yes )
         {
             close();
         }*/
         setWindowState( Qt::WindowMinimized );
         event->ignore();
     }
 }
 bool DashboardWatch::GetConnected() const
 {
   return m_Connected;
 }

 void DashboardWatch::on_NewLocalSocketConnection()
 {
   qDebug() << "new local socket connection";
   m_Connected = true;
 }

 void DashboardWatch::GotoMitkDashboardAction()
 {
     qDebug() << "GotoMitkDashboardAction";
     QDesktopServices::openUrl(QUrl("http://cdash.mitk.org/index.php?project=MITK&display=project"));
 }

 void DashboardWatch::GotoMbiDashboardAction()
 {
     qDebug() << "GotoMbiDashboardAction";
     QDesktopServices::openUrl(QUrl("http://mbits/cdash/index.php?project=MITK-MBI&display=project"));
 }

 void DashboardWatch::setIcon(QIcon& icon)
 {
     if(!OS_IS_UBUNTU)
     {
        m_TrayIcon->setIcon(icon);
        this->setWindowIcon(icon);
     }
     m_StatusIconLabel->setPixmap( icon.pixmap(32,32) );
 }

 void DashboardWatch::fetch()
 {
     m_Manager->get(QNetworkRequest(QUrl("http://mbits/git-status/getRepoStatus.php")));
 }

  void DashboardWatch::replyFinished(QNetworkReply* pReply)
  {
    QNetworkReply::NetworkError error =
    pReply->error();

    int result = -1;
    if( error == QNetworkReply::NoError )
    {
      QByteArray data=pReply->readAll();
      QString str(data);

      result = str.toInt();
    }

    if( m_PreviousResult != result )
    {
      if( result == 0 )
      {
        m_Message = "MITK and MITK-MBI Dashboard are green";
        this->setIcon( m_GreenIcon );
      }
      else if( result == 1 )
      {
        m_Message = "MITK-MBI Dashboard is red";
        this->setIcon( m_RedGreenIcon );
      }
      else if( result == 2 )
      {
        m_Message = "MITK and MITK-MBI Dashboard are red";
        this->setIcon( m_RedIcon );
      }
      else
      {
        m_Message = "The Dashboard status is unknown";
        this->setIcon( m_QuestionIcon );
      }

      if( !OS_IS_UBUNTU )
      {
        m_TrayIcon->showMessage(QString(m_Title),
            m_Message,
            QSystemTrayIcon::Information, m_MessageTime);
          m_TrayIcon->setToolTip(m_Message);
      }

      m_StatusLabel->setText(m_Message);
      m_PreviousResult = result;
    }

    pReply->deleteLater();
  }

 void DashboardWatch::createOptionsGroupBox()
 {
   this->loadSettings();
   m_UpdateRateLabel = new QLabel(tr("Update Rate:"));

   m_UpdateRateSpinBox = new QSpinBox;
   m_UpdateRateSpinBox->setRange(1, 3600);
   m_UpdateRateSpinBox->setSuffix(" minutes");
   m_UpdateRateSpinBox->setValue( m_Settings.value("UpdateRate").toInt() );

   m_AutoStartLabel = new QLabel(tr("Autostart:"));
   m_AutoStartCheckBox = new QCheckBox;
   m_AutoStartCheckBox->setChecked( m_Settings.value("AutoStart").toBool() );

   m_OptionsGroupBoxLayout = new QGridLayout;
   m_OptionsGroupBoxLayout->setContentsMargins( 4,4,4,4 );
   m_OptionsGroupBoxLayout->addWidget(m_UpdateRateLabel, 0, 0);
   m_OptionsGroupBoxLayout->addWidget(m_UpdateRateSpinBox, 0, 1);
   m_OptionsGroupBoxLayout->addWidget(m_AutoStartLabel, 1, 0);
   m_OptionsGroupBoxLayout->addWidget(m_AutoStartCheckBox, 1, 1);

   m_OptionsGroupBox = new QGroupBox(tr("Options"));
   m_OptionsGroupBox->setLayout(m_OptionsGroupBoxLayout);
 }

 void DashboardWatch::createActions()
 {
   m_UpdateNowAction = new QAction(tr("&Update Now"), this);
   connect(m_UpdateNowAction, SIGNAL(triggered()), this, SLOT(fetch()));

    m_RestoreAction = new QAction(tr("&Restore"), this);
    connect(m_RestoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    m_GotoMitkDashboardAction = new QAction(tr("Go to &Mitk Dashboard"), this);
    connect(m_GotoMitkDashboardAction, SIGNAL(triggered()), this, SLOT(GotoMitkDashboardAction()));

    m_GotoMbiDashboardAction = new QAction(tr("Go to M&bi Dashboard"), this);
    connect(m_GotoMbiDashboardAction, SIGNAL(triggered()), this, SLOT(GotoMbiDashboardAction()));

    m_QuitAction = new QAction(tr("&Quit"), this);
    connect(m_QuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
 }

 bool DashboardWatch::OsIsUbuntu()
 {
     bool OsIsUbuntu = false;
     QFile file("/etc/lsb-release");
     if( file.exists() )
     {
         //std::cout << "/etc/lsb-release exists!" << std::endl;
         QString content;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            while (!stream.atEnd())
            {
                content.append( stream.readLine() );
            }
        }

        OsIsUbuntu = content.contains( "Ubuntu" );
     }
     return OsIsUbuntu;
 }

 void DashboardWatch::createTrayIcon()
 {

     /*
     m_GotoMitkDashboard = new QLabel;
     m_GotoMitkDashboard->setText("<a href=\"http://cdash.mitk.org/index.php?project=MITK&display=project\">Go to Mitk Dashboard</a>");
     m_GotoMitkDashboard->setOpenExternalLinks(true);

     m_GotoMbiDashboard = new QLabel;
     m_GotoMbiDashboard->setText("<a href=\"http://mbits/cdash/index.php?project=MITK-MBI&display=project\">Go to Mbi Dashboard</a>");
     m_GotoMbiDashboard->setOpenExternalLinks(true);
     */


     if( !OS_IS_UBUNTU )
     {
         m_TrayIconMenu = new QMenu(this);
         m_TrayIconMenu->addAction(m_UpdateNowAction);
         m_TrayIconMenu->addAction(m_GotoMitkDashboardAction);
         m_TrayIconMenu->addAction(m_GotoMbiDashboardAction);
         m_TrayIconMenu->addAction(m_QuitAction);
         m_TrayIconMenu->addAction(m_RestoreAction);
        m_TrayIcon = new QSystemTrayIcon(this);
        m_TrayIcon->setContextMenu(m_TrayIconMenu);
     }
     else
     {
         QToolBar* fileToolBar = addToolBar(tr("File"));
         fileToolBar->addAction(m_UpdateNowAction);
         fileToolBar->addAction(m_GotoMitkDashboardAction);
         fileToolBar->addAction(m_GotoMbiDashboardAction);
         fileToolBar->addAction(m_QuitAction);
     }

 }

void DashboardWatch::on_UpdateRateSpinBoxValue_Changed( int i )
{
  m_UpdateTimer->setInterval( i * 60 * 1000 );
  this->saveSettings();
}

void DashboardWatch::on_AutoStartCheckBox_clicked( bool checked )
{
  std::cout << "on_AutoStartCheckBox_clicked" << std::endl;
  this->saveSettings();
  this->setAutoStart( checked );
}

void DashboardWatch::loadSettings()
{
  qDebug() << "loading settings";

  m_Settings.sync();
  if( !m_Settings.contains("UpdateRate") )
    m_Settings.setValue("UpdateRate", 2);
  if( !m_Settings.contains("AutoStart") )
    m_Settings.setValue("AutoStart", false);

  qDebug() << "settings loaded: UpdateRate=" << m_Settings.value("UpdateRate").toInt() <<
    ", AutoStart=" << m_Settings.value("AutoStart").toBool();
}

void DashboardWatch::saveSettings()
{
  qDebug() << "saving settings";
  m_Settings.setValue("UpdateRate", m_UpdateRateSpinBox->value() );
  m_Settings.setValue("AutoStart", m_AutoStartCheckBox->isChecked() );
  m_Settings.sync();
}

void DashboardWatch::setAutoStart(bool autostart)
{
  #ifdef Q_WS_WIN
  QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                      QSettings::NativeFormat);

  if (autostart)
  {
    // Want to start on boot up
    char** argv = qApp->argv();
    std::cout << argv[0] << std::endl;
    settings.setValue("DashboardWatch.exe", argv[0] );
  }
  else
  {
    // Do not want to start on boot up
    settings.remove("DashboardWatch.exe");
  }
  #endif

  #ifdef Q_WS_MAC
    QMessageBox::critical(0, QObject::tr("DashboardWatch"),
                          QObject::tr("Feature not implemented for the current platform") );
    m_AutoStartCheckBox->setChecked(false);
    this->saveSettings();
  #endif

  #ifdef Q_WS_X11
  if( OS_IS_UBUNTU )
  {
      QString filename = QDir::homePath() + QDir::separator() + ".config/autostart/DashBoardWatch.desktop";
      QFile file( filename );
      if(autostart)
      {
          //std::cout << filename.toStdString() << std::endl;
          if ( file.open(QIODevice::ReadWrite) )
          {
              QString executable = QFileInfo( QCoreApplication::applicationFilePath() ).absoluteFilePath();
              QTextStream stream( &file );
              stream << "[Desktop Entry]" << endl;
              stream << "Name=DashboardWatch" << endl;
              stream << "Exec=" << executable << endl;
              stream << "Type=Application" << endl;
              file.close();
          }
      }
      else
      {
          file.remove();
      }
  }
  else
  {
      QMessageBox::critical(0, QObject::tr("DashboardWatch"),
                            QObject::tr("Feature not implemented for the current platform") );
      m_AutoStartCheckBox->setChecked(false);
      this->saveSettings();
  }
  #endif

}
