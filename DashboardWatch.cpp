#include <QtGui>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <iostream>

#include "DashboardWatch.h"

  DashboardWatch::DashboardWatch()
   : m_Manager( new QNetworkAccessManager(this) ),
     m_UpdateTimer(new QTimer(this)),
     m_RedIcon(":/DashboardWatch/red-dot.png"),
     m_GreenIcon(":/DashboardWatch/green-dot.png"),
     m_RedGreenIcon(":/DashboardWatch/red-green-dot.png"),
     m_QuestionIcon(":/DashboardWatch/question.png"),
     m_PreviousResult(-1),
     m_MessageTime(6000)
  {
    // setup internals
    m_UpdateTimer = new QTimer(this);
    m_UpdateTimer->setInterval(2000*60);

    // setup UI
    createOptionsGroupBox();
    createActions();
    createTrayIcon();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_OptionsGroupBox);
    setLayout(mainLayout);

    setWindowTitle(tr("DashboardWatch"));
    this->setIcon( m_RedIcon );

    // connect
    connect(m_UpdateRateSpinBox, SIGNAL( valueChanged (int) ),
            this, SLOT( on_UpdateRateSpinBoxValue_Changed(int) ));

    connect(m_TrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    connect(m_Manager, SIGNAL(finished(QNetworkReply*)),
      this, SLOT(replyFinished(QNetworkReply*)));

    connect(m_UpdateTimer, SIGNAL(timeout()),
            this, SLOT(fetch()));

    // show/start
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
     if (m_TrayIcon->isVisible())
     {
         hide();
         event->ignore();
     }
 }

 void DashboardWatch::setIcon(QIcon& icon)
 {
     m_TrayIcon->setIcon(icon);
     this->setWindowIcon(icon);
 }

 void DashboardWatch::fetch()
 {
     m_Manager->get(QNetworkRequest(QUrl("http://mbits/git-status/getRepoStatus.php")));
 }

 void DashboardWatch::replyFinished(QNetworkReply* pReply)
 {

     QByteArray data=pReply->readAll();
     QString str(data);

     int result = str.toInt();
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
       m_TrayIcon->showMessage(QString("DashboardWatch"),
                               m_Message,
                               QSystemTrayIcon::Information, m_MessageTime);
       m_TrayIcon->setToolTip(m_Message);
       m_PreviousResult = result;
     }
 }

 void DashboardWatch::createOptionsGroupBox()
 {

   m_UpdateRateLabel = new QLabel(tr("Update Rate:"));

   m_UpdateRateSpinBox = new QSpinBox;
   m_UpdateRateSpinBox->setRange(1, 3600);
   m_UpdateRateSpinBox->setSuffix(" minutes");
   m_UpdateRateSpinBox->setValue(2);

   m_OptionsGroupBoxLayout = new QGridLayout;
   m_OptionsGroupBoxLayout->addWidget(m_UpdateRateLabel, 0, 0);
   m_OptionsGroupBoxLayout->addWidget(m_UpdateRateSpinBox, 0, 1);
   m_OptionsGroupBoxLayout->setContentsMargins(0,0,0,0);

   m_OptionsGroupBox = new QGroupBox(tr("Options"));
   m_OptionsGroupBox->setLayout(m_OptionsGroupBoxLayout);
 }

 void DashboardWatch::createActions()
 {
    m_RestoreAction = new QAction(tr("&Restore"), this);
    connect(m_RestoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    m_QuitAction = new QAction(tr("&Quit"), this);
    connect(m_QuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
 }

 void DashboardWatch::createTrayIcon()
 {
     m_TrayIconMenu = new QMenu(this);
     m_TrayIconMenu->addAction(m_RestoreAction);
     m_TrayIconMenu->addAction(m_QuitAction);

     m_TrayIcon = new QSystemTrayIcon(this);
     m_TrayIcon->setContextMenu(m_TrayIconMenu);

 }

 void DashboardWatch::on_UpdateRateSpinBoxValue_Changed( int i )
 {
   m_UpdateTimer->setInterval( i * 60 * 1000 );
 }
