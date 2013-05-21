#ifndef DashboardWatch_H
#define DashboardWatch_H

#include <QSystemTrayIcon>
#include <QWidget>
#include <QMainWindow>
#include <QGridLayout>
#include <QSettings>
#include <QLocalServer>
#include <QtNetwork/QNetworkAccessManager>

class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;

class DashboardWatch : public QMainWindow
{
    Q_OBJECT

public:
    DashboardWatch();

    static const bool OS_IS_UBUNTU;
    void setVisible(bool visible);
    bool GetConnected() const;
    static bool OsIsUbuntu();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_UpdateRateSpinBoxValue_Changed( int i );
    void on_AutoStartCheckBox_clicked( bool checked );
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void replyFinished(QNetworkReply*);
    void fetch();
    void on_NewLocalSocketConnection();
    void GotoMitkDashboardAction();
    void GotoMbiDashboardAction();

private:
    void setIcon(QIcon& index);
    void createOptionsGroupBox();
    void createActions();
    void createTrayIcon();
    void loadSettings();
    void saveSettings();
    void setAutoStart(bool autostart);

    QLabel* m_StatusIconLabel;
    QLabel* m_StatusLabel;

    QLabel* m_UpdateRateLabel;
    QSpinBox* m_UpdateRateSpinBox;
    QLabel* m_AutoStartLabel;
    QCheckBox* m_AutoStartCheckBox;
    QGridLayout* m_OptionsGroupBoxLayout;
    QGroupBox* m_OptionsGroupBox;

    QAction* m_UpdateNowAction;
    QAction* m_RestoreAction;
    QAction* m_QuitAction;
    QAction* m_GotoMitkDashboardAction;
    QAction* m_GotoMbiDashboardAction;

    QSystemTrayIcon* m_TrayIcon;
    QMenu* m_TrayIconMenu;
    QNetworkAccessManager* m_Manager;

    QTimer* m_UpdateTimer;

    QIcon m_RedIcon;
    QIcon m_GreenIcon;
    QIcon m_RedGreenIcon;
    QIcon m_QuestionIcon;
    int m_PreviousResult;
    QString m_Message;
    int m_MessageTime;
    QString m_Title;
    QSettings m_Settings;
    QLocalServer* m_LocalServer;
    bool m_Connected;
};

#endif
