#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QMainWindow>
#include <QtWidgets>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QQuickWidget>
#include <QGamepad>
#include <QGamepadKeyNavigation>
#include "VideoReceiver.h"
#include "pingsensor.h"
#include <QMediaPlayer>
#include "SecureFileUploader.h"
namespace AS
{
#include "ardusub_api.h"

}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void resizeWindowsManual();
    void ResizeToolBar();
    void AddToolBarSpacer(QToolBar* pToolBar=nullptr,int width=-1);
    void InitPing1dUDP();
    void CheckRollOrPitchChang(bool bTimerOut=false);
    void RestartNetWork();
    void LoadInIConfig();
    void UpdateMapCenterCoordinates(QStringList coord);
    void UpdateMarkerCoordinates(QStringList coord);
    void UpdateModeLable();
    void HandleNewKey(QKeyEvent *event);
    void InitGamePad();
    void LoadMapingKey();
    void UpdateKeyControlValue(bool bPress=true);
    void UpdateMapTopLableText(QString strTip);
    void  PlayMediaFileMapText(QString strText);
signals:
    void SetQMLText();
    void UpdateLogWindow(const QString& strLog);
private slots:
    void updateVehicleData();
    void manualControl();
    void on_setQmlText();
    void on_actionVideo_triggered();

    void on_actionMenu_triggered();

    void on_modeBt_clicked();               //Changing Combobox to Button

    void modeComboBox_currentIndexChanged(int index);

    void armCheckBox_stateChanged(bool checked);

    void on_actionSonarGps_triggered();

    void on_updateConfidence();

    void  on_statusChanged(QQuickWidget::Status status);
    void on_mainStackedWidget_currentChanged(int arg1);

    void on_axisLeftXChanged(double value);
    void on_axisLeftYChanged(double value);
    void on_axisRightXChanged(double value);
    void on_axisRightYChanged(double value);
    void on_gamepadConnected(int deviceId);
    void on_gamepadDisconnected(int deviceId);




    void on_actionTakePhoto_triggered();
    void  InitArdusubMessageLog();
//qml key indicators
    void  On_QML_StatusChanged(QQuickWidget::Status status);
    void  On_QML_ArmState();
    void  On_QML_DiveMode();
    void  On_QML_Speed();
    void  Call_QML_ChangeKey(const QString& strKey);
    void  SetQMLItemOpacity(QString strObName,qreal fOpacity=1);
    void  SetQMLItemText(QString strObName,QString strButtonText);
    void  SetQMLItemVar(QString strObName,QString strVarName,const QVariant& var);
    void on_actionMessageLog_triggered();

private:
    Ui::MainWindow *ui;

    VideoReceiver *videoReceiver;
    PingSensor*   pingLink;
    QPushButton *armCheckBox;
    QPushButton *modeComboBox;               //Changing Combobox to Button
    //QComboBox *modeComboBox;
    
    // status toolbar
    QLabel *yawLabelValue;
    QLabel *pitchLabelValue;
    QLabel *rollLabelValue;
    QLabel *depthLabelValue;
    QLabel *SonarlValue;
    QLabel *SonarLabel;
    QLabel *MetersValue;
    // timer
    QTimer vehicleDataUpdateTimer;
    QTimer manualControlTimer;

    //int m_modeIndex; 
    uint IdleTime;                        //Changin Combobox to Button
	bool	firstRun;
    bool robot_status_delay;                     // first run flag 2020/02/20
    //
    float WarnDistance = 1.0;
    float MinDistance = 0.5;
    bool  SonarAlarm = false;
    int   AlarmTime = 0;                 //Unused
    int   AlarmSetting = 5;
    int   ConfidenceSetting = 90;
    QTime  PrevTime;
    QObject*    qmlTimer=nullptr;
    //check roll pitch
    QTimer       rollLPitchCheckTimer;
    QString     strRollValue;
    QString     strPitchValue;
    bool        bas_init_status=false;
    QStringList     fMapCoordinates;
    QStringList     fMarkerCoordinates;
    bool        bardusubCoordinates;// is use  ardusubapi coordinates
    uint         iIdleSetting;
    QGamepad*   _gamepad;
    QString     mapTextCache;
    bool        bmapState=false;
    QGamepadKeyNavigation*  _gameKeyNavigation;
    QMediaPlayer *   player=nullptr;
    QString         strTakPhontoName;
    QString         strRemoteDir;
    QString         strHost;
    QString         strUser;
    QString         strPass;
    int             strPhotoDelay;
    bool            bsftpIdle;
    QTime           tLastUpLoad;
    bool            bdepthhold;

    bool            bArmState;
    bool            bSpeed;
    bool            bMode;
    int             iButton;        //Stoping to many manual_control.buttons data being sent.
    int             iButtonNo; 
    bool            bBold;

    QString         strSpeed;
    QString         strDiveMode;

    bool            bConnection;

    typedef struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        int16_t r;
        uint16_t buttons;
    } manual_control_t;

    manual_control_t manual_control;

    typedef struct
    {
        bool W = false;
        bool S;
        bool A;
        bool D;
        bool Up;
        bool Down;
        bool Left;
        bool Right;
    } pressedKey_t;

    pressedKey_t pressedKey;

    typedef struct
    {
        int16_t forward;
        int16_t backward;
        int16_t leftward;
        int16_t rightward;
        int16_t upward;
        int16_t downward;
        int16_t turnLeft;
        int16_t turnRight;
        int16_t turnLeftM;
        int16_t turnRightM;

    } keyControlValue_t;

    keyControlValue_t keyControlValue;
    keyControlValue_t keyControlValue_Press;            //KEYBOARD - KEYPRESS
    keyControlValue_t keyControlValue_Hold;             //KEYBOARD - KEYHOLD
    keyControlValue_t keyControlValue_HoldWarning;      //KEYBOARD - KEYHOLD - SONAR WARNING
    keyControlValue_t keyControlValue_Warning;          //KEYBOARD - KEYPRESS - SONAR WARNING
    keyControlValue_t keyControlValue_HoldAlarm;        //KEYBOARD - KEYHOLD - SONAR ALARM
    keyControlValue_t keyControlValue_Alarm;            //KEYBOARD - KEYPRESS - SONAR ALARM


    int32_t             m_devlat=0;
    int32_t             m_devlon=0;               /*< [degE7] Longitude, expressed*/
    int32_t             m_devalt=0;
    uint8_t currentVehicle;
    AS::Vehicle_Data_t *vehicle_data;



    QMap<QString,QObject*>              m_qmlObjectMap;         //qml key indicators
    
    void setupToolBars();
    void setupTimer();

    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};
#endif // MAINWINDOW_H
