#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMetaObject>
#include <QGeoCoordinate>
#include <QGamepadManager>
#include <QMediaPlayer>

// THIS APP USES ARDUSUB_API TO COMMUNICATE WITH ROBOT 192.168.2.2:14???
// THIS APP USES GSTREAMER TO PLAY LIVE VIDEO STREAM ON PORT 5600
// THIS APP USES PING TO RECEIVE SONAR DATA
// THIS APP USES QMediaPlayer TO PLAY .MP3 FILES FOR VOICE
// THIS APP HAS GAMEPAD SUPPORT BUT IS NOT CURRENTLY BEING USED
// THIS APP HAS BASIC COLLISON AI
// THIS APP HAS A CONFIGURATION FILE teleportal.ini
// THIS APP HAS A LOGFILE FILE 



MainWindow::MainWindow(QWidget *parent)		//START APPLICATION
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , videoReceiver(new VideoReceiver(this))
    , currentVehicle(1)
    , vehicle_data(new AS::Vehicle_Data_t)
    ,_gamepad(nullptr)
    ,_gameKeyNavigation(nullptr)
{
   
	// SETUP APPLICATION
    ui->setupUi(this);
    setWindowIcon(QIcon(":/assets/icons/main.svg"));
    QCoreApplication::setOrganizationName("Teleportal");
    QCoreApplication::setOrganizationDomain("");
    QCoreApplication::setApplicationName("Teleportal");

    // SETUP STARTING VALUES FOR KEYBOARD INPUT
    manual_control.x = 0;
    manual_control.y = 0;
    manual_control.z = 500;
    manual_control.r = 0;
    manual_control.buttons = 0;				//manual_control.buttons is a bitmask http://www.bradgoodman.com/bittool/
    
    pressedKey.W = false;
    pressedKey.S = false;
    pressedKey.A = false;
    pressedKey.D = false;
    pressedKey.Up = false;
    pressedKey.Down = false;
    pressedKey.Left = false;
    pressedKey.Down = false;

    keyControlValue.forward = 700;
    keyControlValue.backward = -700;
    keyControlValue.leftward = -700;
    keyControlValue.rightward = 700;
    keyControlValue.upward = 900;
    keyControlValue.downward = 100;
    keyControlValue.turnLeft = -700;
    keyControlValue.turnRight = 700;
    keyControlValue.turnLeftM = -300;
    keyControlValue.turnRightM = 300;

    // SETUP MISC VARIABLES
    IdleTime = 1;
    firstRun = false;
    bdepthhold = false;
    bArmState = false;
    bSpeed = true;
    bMode = true;
    iButton = 0;
    iButtonNo = 0;
    bBold = false;
    bsftpIdle=true;

    // DISPLAY VIDEO AND LOAD TOOLBAR
    on_actionVideo_triggered();
    setupToolBars();

    // SETUP FOCUS POLICY
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // LOAD CONFIG FILE AND INITIALISE GAMEPAD
    LoadInIConfig();
    InitGamePad();

    // INITILIZE & CONNECT TO ROBOT
    std::string ip("192.168.2.");
    AS::as_api_init(ip.c_str(), F_THREAD_ALL);
    bas_init_status=true;
    qDebug() << "ROBOT INITILIZED (86)";

    // SETUP MAIN LOOP
    connect(this,SIGNAL(SetQMLText()),this,SLOT(on_setQmlText()));
    setupTimer();
    connect(ui->quickWidget,SIGNAL(statusChanged(QQuickWidget::Status)),this,SLOT(On_QML_StatusChanged(QQuickWidget::Status)));   //qml key indicators
    videoReceiver->start(ui->quickWidget);

    // LOAD MAPS
    connect(ui->quickWidget_2,SIGNAL(statusChanged(QQuickWidget::Status)),this,SLOT(on_statusChanged(QQuickWidget::Status)));
    ui->quickWidget_2->setSource(QUrl(QStringLiteral("qrc:/assets/maps.qml")));
    pingLink=new PingSensor(this);
    PrevTime=QTime::currentTime();
    connect(pingLink,SIGNAL(distanceConfidenceChanged()),this,SLOT(on_updateConfidence()));
    rollLPitchCheckTimer.setInterval(10000);
    connect(&rollLPitchCheckTimer, &QTimer::timeout, this, [this] {
        CheckRollOrPitchChang(true);
    });
    rollLPitchCheckTimer.start();

    // SETUP KEYBOARD KEY IMAGES STARTING OPACITY
    SetQMLItemOpacity("Up",0.5); 
    SetQMLItemOpacity("Down",0.5);
    SetQMLItemOpacity("TurnLeft",0.5);
    SetQMLItemOpacity("TurnRight",0.5);
    SetQMLItemOpacity("MoveLeft",0.5);
    SetQMLItemOpacity("MoveRight",0.5);
    SetQMLItemOpacity("ForWard",0.5);
    SetQMLItemOpacity("BackWard",0.5);
    SetQMLItemOpacity("CamUp",0.5);
    SetQMLItemOpacity("CamDown",0.5);
    SetQMLItemOpacity("LightsUp",0.5);
    SetQMLItemOpacity("LightsDown",0.5);
    SetQMLItemOpacity("ArmState",0.8);
    SetQMLItemOpacity("DiveMode",0.8);
    SetQMLItemOpacity("Speed",0.8);
}


MainWindow::~MainWindow()
{
    //ON SHUTDOWN DISARM ROBOT
    qDebug() << "Program Attempting To Exited";
    armCheckBox->setChecked(false);
    armCheckBox_stateChanged(true);
    delete ui;
    qDebug() << "Program Exited Successfully?";
}


void MainWindow::setupToolBars()
{
    //SETUP TOOLBAR
    QList<QAction *> actionListDisarm;
    ui->vehicleToolBar->setFocusPolicy(Qt::NoFocus);
    ui->vehicleToolBar->addActions(actionListDisarm);

    armCheckBox = new QPushButton(this);
    armCheckBox->setText("CLICK TO START - ROBOT DISARMED");
    armCheckBox->setCheckable(true);
    armCheckBox->setChecked(false);
    armCheckBox->setVisible(false);								// Hide Top Arm/DisArm Button
   /*armCheckBox->setStyleSheet("border-style: outset;"
                              "border-width: 1px;"
                              "border-radius: 2px;"
                              "border-color: beige;"
                              "min-width: 10em;"
                              "padding: 6px;"
                              "background:rgb(0, 255, 0);"
                              "color: rgb(255, 255, 255);"
                              "font: 87 12pt \"Arial Black\";");
    armCheckBox->setFocusPolicy(Qt::NoFocus);
    */
    AddToolBarSpacer(ui->vehicleToolBar);
    ui->vehicleToolBar->addWidget(armCheckBox);
    AddToolBarSpacer(ui->vehicleToolBar);
    connect(armCheckBox,SIGNAL(clicked(bool)),
                     this,SLOT(armCheckBox_stateChanged(bool)));
    UpdateMapTopLableText("ROBOT DATA LINK FAILED");
    AddToolBarSpacer(ui->tabsToolBar);
    QLabel *modeLable = new QLabel(this);
    modeLable->setText("MODE: ");
    modeLable->setVisible(false);										// Hide Top Dive Mode Label
    modeLable->setStyleSheet("font: 87 10pt \"Arial Black\"");
    ui->tabsToolBar->addWidget(modeLable);
    modeComboBox = new QPushButton(this);
    modeComboBox->setText("Unknown");
    modeComboBox->setStyleSheet("font: 87 10pt \"Arial Black\"");
    modeComboBox->setFocusPolicy(Qt::NoFocus);
    modeComboBox->setVisible(false);								// Hide Top Dive Mode Label
    // modeComboBox->show();
    connect (modeComboBox , SIGNAL(clicked()) , this , SLOT(on_modeBt_clicked()) );
    ui->tabsToolBar->addWidget(modeComboBox);

    // SONAR DISPLAY
    SonarLabel=new QLabel("SONAR: ");
    SonarlValue = new QLabel("00.0 M (00%)   ");
    SonarlValue->setFocusPolicy(Qt::NoFocus);
    SonarLabel->setStyleSheet("font: 87 10pt \"Arial Black\"");
    SonarlValue->setStyleSheet("font: 87 10pt \"Arial Black\"");
    
    //THESE YAW PITCH ROLL VALUES ARE CURRENTLY HIDDEN
    QLabel *yawLabel = new QLabel("Compass: ", this);	
    yawLabel->setFocusPolicy(Qt::NoFocus);
    yawLabelValue = new QLabel("0.00", this);
    yawLabelValue->setFocusPolicy(Qt::NoFocus);

    QLabel *pitchLabel = new QLabel("Pitch: ", this);
    pitchLabel->setFocusPolicy(Qt::NoFocus);
    pitchLabelValue = new QLabel("0.00", this);
    pitchLabelValue->setFocusPolicy(Qt::NoFocus);

    QLabel *rollLabel = new QLabel("Roll: ", this);
    rollLabel->setFocusPolicy(Qt::NoFocus);
    rollLabelValue = new QLabel("0.00", this);
    rollLabelValue->setFocusPolicy(Qt::NoFocus);

    //HIDE UNUSED YAW, PITCH & ROLL VALUES FROM TOOLBAR

    pitchLabel->setVisible(false);
    pitchLabelValue->setVisible(false);
    rollLabel->setVisible(false);
    rollLabelValue->setVisible(false);
    yawLabel->setVisible(false);
    yawLabelValue->setVisible(false);

	//DEPTH DISPLAY 
    QLabel *depthLabel = new QLabel("DEPTH: ", this);
    depthLabel->setFocusPolicy(Qt::NoFocus);
    depthLabelValue = new QLabel("0.00 M", this);
    depthLabelValue->setFocusPolicy(Qt::NoFocus);
    depthLabelValue->setStyleSheet("font: 87 10pt \"Arial Black\"");
    depthLabel->setStyleSheet("font: 87 10pt \"Arial Black\"");

    //HIDDEN VALUES
    //yawLabelValue->setFixedWidth(50);
    //pitchLabelValue->setFixedWidth(50);
    //rollLabelValue->setFixedWidth(50);
    //depthLabelValue->setFixedWidth(100);

    AddToolBarSpacer(ui->statusToolBar);
    ui->statusToolBar->addWidget(SonarLabel);
    ui->statusToolBar->addWidget(SonarlValue);
    ui->statusToolBar->addWidget(yawLabel);
    ui->statusToolBar->addWidget(yawLabelValue);
    ui->statusToolBar->addWidget(pitchLabel);
    ui->statusToolBar->addWidget(pitchLabelValue);
    ui->statusToolBar->addWidget(rollLabel);
    ui->statusToolBar->addWidget(rollLabelValue);
    ui->statusToolBar->addWidget(depthLabel);
    ui->statusToolBar->addWidget(depthLabelValue);

	AddToolBarSpacer(ui->statusToolBar);
    QLabel *bannerLabel = new QLabel(this);								//ADD LOGO
    bannerLabel->setFixedWidth(145);
    bannerLabel->setFixedHeight(15);
    bannerLabel->setStyleSheet("border-image: url(:/assets/logo/Logo-Large.png);");
    ui->statusToolBar->addWidget(bannerLabel);

    ui->menuPage->setStyleSheet("border-image: url(:/assets/keyboardControls.png);");  //DISPLAY KEYBOARD CONTROLS HELP IMAGE

    ui->tabsToolBar->setStyleSheet("QToolBar { border-left-style: none; border-right-style: none; }");
    ui->vehicleToolBar->setStyleSheet("QToolBar { border-left-style: none; border-right-style: none; }");
    ui->statusToolBar->setStyleSheet("QToolBar { border-left-style: none; border-right-style: none; }");
    strRollValue=rollLabelValue->text();
    strPitchValue=pitchLabelValue->text();
    ResizeToolBar();

      	
}


void MainWindow::on_modeBt_clicked(){

//Depreciated...

}


void MainWindow::setupTimer()
{
     //SETUP TIMER LOOP
    QObject::connect(&vehicleDataUpdateTimer, &QTimer::timeout, this, &MainWindow::updateVehicleData);
    vehicleDataUpdateTimer.setInterval(25);
    vehicleDataUpdateTimer.start();

    QObject::connect(&manualControlTimer, &QTimer::timeout, this, &MainWindow::manualControl);
    manualControlTimer.setInterval(50);
    manualControlTimer.start();
}


void MainWindow::updateVehicleData()
{
    // UPDATES ROBOT VARIABLES & MAIN TIMING LOOP

    if (!AS::as_api_check_vehicle(currentVehicle))
    {
        // IF ROBOT IS NOT CONNECTED RETURN
        UpdateMapTopLableText("ROBOT DATA LINK FAILED");
        QObject* markerItem3=ui->quickWidget->rootObject()->findChild<QObject*>("ArmState");
    	markerItem3->setProperty("enabled",QVariant::fromValue(false));
        bConnection = false;
        qDebug() << "Robot is not connected (296)";
        return;
    }

    if (!firstRun)
    {
        // SEND COMMANDS TO ROBOT ON STARTUP (DISARM, STABILITY MODE)
        	qDebug() << "Attempting first connection to Robot";

            firstRun = true;

            QObject* markerItem3=ui->quickWidget->rootObject()->findChild<QObject*>("ArmState");
    		markerItem3->setProperty("enabled",QVariant::fromValue(true));

            UpdateMapTopLableText("CONNECTED - ARM ROBOT TO START");
            bConnection = false;

            PlayMediaFileMapText("reconnect");			//PLAY WELCOME BACK MP3
    }

    //GET ROBOT DATA
    AS::as_api_get_vehicle_data2(currentVehicle, vehicle_data);

    if (vehicle_data == nullptr)
    {
        // IF ROBOT IS NOT CONNECTED RETURN
        qDebug() << "Robot is not connected (319)";
        return;
    }

    float yaw = 0, roll = 0, pitch = 0, depth = 0;

    //CHANGE VALUES FOR DISPLAY PURPOSES
    const float degreePerRad = 180.0f / 3.1415926f;
    yaw = vehicle_data->yaw * degreePerRad;
    roll = vehicle_data->roll * degreePerRad;
    pitch = vehicle_data->pitch * degreePerRad;
    depth = vehicle_data->alt / 1000.0f;

    //SEND VALUES TO COMPASS AND ADI
    ui->qADI->setData(roll, pitch);
    ui->qCompass->setYaw(yaw);
    ui->qCompass->setAlt(depth);

    rollLabelValue->setNum(round(roll * 100) / 100.0);
    pitchLabelValue->setNum(round(pitch * 100) / 100.0);
    double yawLableCompass=round(yaw * 100) / 100.0;
    yawLabelValue->setNum(yawLableCompass);

    //DEPTH VALUES
    depthLabelValue->setNum(round(depth * 100) / 100.0);
    depthLabelValue->setText(depthLabelValue->text()+" M");

    //COMPASS VALUES
    ui->qCompass->setYaw(yawLableCompass);
    if(ui->quickWidget_2->status()==QQuickWidget::Ready)
    {
        if(bardusubCoordinates)
        {
            //use robot coord for map?
            if(m_devlat!=vehicle_data->lat||
               m_devlon!=vehicle_data->lon)
            {
                m_devlat=vehicle_data->lat;
                m_devlon=vehicle_data->lon;
                double d1e7=qPow(10,7);
                QStringList CoordList {QString::number((double)m_devlat/d1e7),QString::number((double)m_devlon/d1e7)};
                UpdateMapCenterCoordinates(CoordList);
                UpdateMarkerCoordinates(CoordList);
            }
        }
        QQuickItem* pImgItem=ui->quickWidget_2->rootObject()->findChild<QQuickItem*>("markerimg");
        if(pImgItem)
        {
            pImgItem->setRotation(yawLableCompass);				//Rotate Icon on Map with Compass
        }
    }

    	//IF USER IS IDLE FOR 180 SEC DISARM ROBOT
        LASTINPUTINFO LastInput = {};
        LastInput.cbSize = sizeof(LastInput);
        ::GetLastInputInfo(&LastInput);
        uint IdleTime = (::GetTickCount() - LastInput.dwTime)/1000;
        if ((IdleTime > iIdleSetting) == true)
        {
            armCheckBox->setChecked(false);
            armCheckBox_stateChanged(true);
            PlayMediaFileMapText("timeout");
            qDebug() << "No Human Detected - Disarming Robot (381)";
        }

    //UPDATE MODE LABEL    
    CheckRollOrPitchChang(false);			//CHECK FOR LOSS OF CONNECTION WITH ROBOT
    UpdateModeLable();						//CHECK IF ROBOT IS DISARMED AND UPDATE UI
}


void MainWindow::manualControl()
{
    //MAIN ROBOT CONTROL LOOP - ACTIVATE ONLY WHEN ROBOT IS CONNECTED AND ARMED
    if (armCheckBox->isChecked()&&
        AS::as_api_check_vehicle(currentVehicle))
    {
        AS::as_api_manual_control(manual_control.x, manual_control.y,
                                  manual_control.z, manual_control.r,
                                  manual_control.buttons, currentVehicle);
    
//Adam hack to stop constant button pressing (except cam tilt)
    if (manual_control.buttons == 1024)
    {
        return;
    }   
    else if (manual_control.buttons == 512)
    {
        return;
    }
    else if (manual_control.buttons == iButton)
    {
        iButtonNo = (iButtonNo + 1);
    }
    else if (iButtonNo == 5)     //Let 5 commands get through before clearing data
    {
        manual_control.buttons = 0;
    }
    else
        {
        iButton = manual_control.buttons;
        }
    }
}


void MainWindow::resizeWindowsManual()
{
    int m_width = ui->videoPage->width();
    ui->quickWidget->setGeometry(0, 0, m_width, ui->videoPage->height());
    ui->qCompass->setGeometry(m_width - 160, 0, 160, 160);
    ui->qADI->setGeometry(m_width - 160, 160, 160, 160);
    ResizeToolBar();
}


void MainWindow::ResizeToolBar()
{
    QSize isize=ui->statusToolBar->sizeHint();
    int setW=ui->centralwidget->width()/3;
    if(setW<isize.width())
    {
        ui->statusToolBar->setFixedWidth(isize.width());
    }
    else
    {
         ui->tabsToolBar->setFixedWidth(setW);
         ui->statusToolBar->setFixedWidth(setW);
         ui->vehicleToolBar->setFixedWidth(setW);
    }

}


void MainWindow::AddToolBarSpacer(QToolBar *pToolBar,int width)
{
    QWidget *spacer = new QWidget(this);
    spacer->setFocusPolicy(Qt::NoFocus);
    if(width<0)
    {
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    else
    {
        spacer->setFixedWidth(width);
    }
    pToolBar->setFocusPolicy(Qt::NoFocus);
    pToolBar->addWidget(spacer);
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeWindowsManual();
    QMainWindow::resizeEvent(event);
    qDebug() << "Application was resized (452)";
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //KEYBOARD CONTROLS FOR ROBOT
    if (event->isAutoRepeat())
    {
        // IF THE KEY COMMAND IS THE SAME RETURN
        return;
    }

    // KEY W - UP
    if (event->key() == Qt::Key_W)
    {
        SetQMLItemOpacity("Up",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        
        if (!armCheckBox->isChecked())		
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

        // SEND COMMAND TO ROBOT
        UpdateKeyControlValue();
        pressedKey.W = true;
        manual_control.z = keyControlValue.upward;	
        qDebug() << "You Pressed Key W - Up";
    }

    // KEY S - DOWN
    else if (event->key() == Qt::Key_S)
    {
        SetQMLItemOpacity("Down",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())		
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

        // IF PROXIMITY ALERT THEN DONT SEND COMMAND
        if(!SonarAlarm)					
        {
            // SEND COMMAND TO ROBOT
            UpdateKeyControlValue();
            pressedKey.S = true;
            manual_control.z = keyControlValue.downward;
            qDebug() << "You Pressed Key S - Down";		
        }
    }

    // KEY A - ROTATE ROBOT LEFT
    else if (event->key() == Qt::Key_A)
    {
        SetQMLItemOpacity("TurnLeft",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())		
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

        qDebug() << "You Pressed Key A";
        UpdateKeyControlValue();
        pressedKey.A = true;

         //CHECK MODE OF ROBOT AND GIVE CORRECT INPUT BASED ON MODE - MANUAL MODE VERY FAST
        if (!bMode)			
        {
            manual_control.r = keyControlValue.turnLeftM;
        }
        else if (bMode)
        {
            manual_control.r = keyControlValue.turnLeft;
        }
    }

    // KEY D - ROTATE ROBOT RIGHT
    else if (event->key() == Qt::Key_D)
    {
        SetQMLItemOpacity("TurnRight",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

        qDebug() << "You Pressed Key D";
        UpdateKeyControlValue();
        pressedKey.D = true;

        // CHECK MODE OF ROBOT AND GIVE CORRECT INPUT BASED ON MODE - MANUAL MODE VERY FAST
        if (!bMode)
        {
            manual_control.r = keyControlValue.turnRightM;
        }
        else if (bMode)
        {
           manual_control.r = keyControlValue.turnRight;
        }
    }

    // KEY UP - MOVE ROBOT FORWARD
    else if (event->key() == Qt::Key_Up)
    {
        SetQMLItemOpacity("ForWard",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

        // IF PROXIMITY ALERT THEN DONT SEND COMMAND
        else if(!SonarAlarm)
        {
            // SEND COMMAND TO ROBOT
            pressedKey.Up = true;
            UpdateKeyControlValue();
            manual_control.x = keyControlValue.forward;
            qDebug() << "You Pressed Key Up";
        }
         
    }

    // KEY DOWN - MOVE ROBOT BACKWARD
    else if (event->key() == Qt::Key_Down)
    {
        SetQMLItemOpacity("BackWard",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

        qDebug() << "You Pressed Key Down";
        UpdateKeyControlValue();

        // SEND COMMAND TO ROBOT
        pressedKey.Down = true;
        manual_control.x = keyControlValue.backward;
    }

    // KEY LEFT - MOVE ROBOT LEFT
    else if (event->key() == Qt::Key_Left)
    {
        SetQMLItemOpacity("MoveLeft",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

        qDebug() << "You Pressed Key Left";
        UpdateKeyControlValue();

        // SEND COMMAND TO ROBOT
        pressedKey.Left = true;
        manual_control.y = keyControlValue.leftward;
    }

    // KEY RIGHT - MOVE ROBOT RIGHT
    else if (event->key() == Qt::Key_Right)
    {
        SetQMLItemOpacity("MoveRight",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())		
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

        qDebug() << "You Pressed Key Right";
        UpdateKeyControlValue();

        //SEND COMMAND TO ROBOT
        pressedKey.Right = true;
        manual_control.y = keyControlValue.rightward;
    }

    // KEY R - CAMERA TILT UP
    else if (event->key() == Qt::Key_R)
     {
        SetQMLItemOpacity("CamUp",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

         qDebug() << "You Pressed Key R";
         UpdateKeyControlValue();

         // SEND COMMAND TO ROBOT
         manual_control.buttons = 1024;
     }

     // KEY F - CAMERA TILT DOWN
     else if (event->key() == Qt::Key_F)
     {
        SetQMLItemOpacity("CamDown",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

         qDebug() << "You Pressed Key F";
         UpdateKeyControlValue();
         // SEND COMMAND TO ROBOT
         manual_control.buttons = 512;
     }
   
   // KEY T - LIGHTS BRIGHTER
   else if (event->key() == Qt::Key_T)
     {
        SetQMLItemOpacity("LightsUp",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }
         qDebug() << "You Pressed Key T";
         UpdateKeyControlValue();

         //SEND COMMAND TO ROBOT
         manual_control.buttons = 16384;
     }

     // KEY G - LIGHTS DIMMER
     else if (event->key() == Qt::Key_G)
     {
        SetQMLItemOpacity("LightsDown",1.0);
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

         qDebug() << "You Pressed Key G";
         UpdateKeyControlValue();

         // SEND COMMAND TO ROBOT
         manual_control.buttons = 8192;		
     }



// KEY 1 - LOW SPEED
      else if (event->key() == Qt::Key_1)
     {
         if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
            return;
         }
            qDebug() << "You Pressed Key 1 - LOW SPEED";
            SetQMLItemText("Speed","LOW SPEED");
            bSpeed = false;
            SetQMLItemOpacity("Speed",0.8);
            manual_control.buttons = 4096;
     }

 // KEY 2 - HIGH SPEED
      else if (event->key() == Qt::Key_2)
     {
         if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
            return;
         }
            qDebug() << "You Pressed Key 2 - HIGH SPEED";
            SetQMLItemText("Speed","HIGH SPEED");
            bSpeed = true;
            SetQMLItemOpacity("Speed",1.0);
            manual_control.buttons = 2048;
     }

      // KEY 4 - STABILITY MODE
      else if (event->key() == Qt::Key_4)
     {
         if (armCheckBox->isChecked())
         {
         	qDebug() << "You Pressed Key 4 - STABILIZED MODE";
            SetQMLItemText("DiveMode","STABILIZED");
            bMode = true;
            SetQMLItemOpacity("DiveMode",1.0);
            manual_control.buttons = 8;
            UpdateKeyControlValue();
         	PlayMediaFileMapText("stability");
         }
     }
   
   // KEY 7 - DEPTH HOLD MODE
   // else if (event->key() == Qt::Key_7)
   //  {
   //  	 if (armCheckBox->isChecked())
   //      {
   //      	if (bdepthhold)
   //       	{	
   //      		UpdateKeyControlValue();
   //      		manual_control.buttons = 4;
   //      		modeComboBox->setText("DEPTH HOLD");
   //      		PlayMediaFileMapText("depth");
   //      		qDebug() << "You Pressed Key 2 - DEPTH HOLD MODE";
   //      	}
   //  	}
   //  }
   

     // KEY 3 - MANUAL MODE
     else if (event->key() == Qt::Key_3)
     {
         if (armCheckBox->isChecked())
         {
         	qDebug() << "You Pressed Key 3 - MANUAL MODE";
            SetQMLItemText("DiveMode","MANUAL");
            bMode = false;
            SetQMLItemOpacity("DiveMode",0.8);
            manual_control.buttons = 2;
            UpdateKeyControlValue();
            PlayMediaFileMapText("manual");
     	}
     }

     // KEY 5 - DISARM ROBOT
     else if (event->key() == Qt::Key_5)
     {
        	armCheckBox->setChecked(false);
            armCheckBox_stateChanged(true);
     }

     // KEY 6 - ARM ROBOT
     else if (event->key() == Qt::Key_6)
     {
         	armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true); 
     }
     
     // KEY C - TAKE PHOTO
     else if (event->key() == Qt::Key_C)
     {
         qDebug() << "You Pressed Key C - TAKE PHOTO";
         // TAKE PHOTO
         on_actionTakePhoto_triggered();
     }

      // KEY H - HELP
     else if (event->key() == Qt::Key_H)
     {
         qDebug() << "You Pressed Key H - HELP";
         // DISPLAY HELP MENU
         on_actionMenu_triggered();
     }

     // KEY M - MAP
     else if (event->key() == Qt::Key_M)
     {
         qDebug() << "You Pressed Key M - MAP";
         // DISPLAY HELP MENU
         on_actionSonarGps_triggered();
     }

     // KEY V - VIDEO
     else if (event->key() == Qt::Key_V)
     {
         qDebug() << "You Pressed Key V - VIDEO";
         // DISPLAY HELP MENU
         on_actionVideo_triggered();
     }

    else
    {
        //qDebug() << "You Pressed NOT supported Key";
    }

    // OUTPUT VALUE TO DEBUG
    // qDebug() << "x: " << manual_control.x;
    // qDebug() << "y: " << manual_control.y;
    // qDebug() << "z: " << manual_control.z;
    // qDebug() << "r: " << manual_control.r;
}


void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // ON KEY RELEASE CHANGE ROBOT COMMANDS BACK TO IDLE VALUES

    if (event->isAutoRepeat())
    {
         // IF THE KEY RELEASES TO MANY TIMES? I HAVE NO IDEA WHY THIS IS HERE.
    	//qDebug() << "Key Auto Repeat Error (858)";
        return;
    }

    if (event->key() == Qt::Key_W)
    {
        qDebug() << "You Released Key W";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("Up",0.5);

        
        // IF OPPOSITE KEY IS PRESSED THEN DONT RESET VALUE TO ZERO
        if (pressedKey.S)
        {
            manual_control.z = keyControlValue.downward;
        }
        else
        {
            manual_control.z = 500;
        }
        pressedKey.W = false;
    }

    else if (event->key() == Qt::Key_S)
    {
        qDebug() << "You Released Key S";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("Down",0.5);
		// IF OPPOSITE KEY IS PRESSED THEN DONT RESET VALUE TO ZERO
        if (pressedKey.W)
        {
            manual_control.z = keyControlValue.upward;
        }
        else
        {
            manual_control.z = 500;
        }
        pressedKey.S = false;
    }
    
    else if (event->key() == Qt::Key_A)
    {
        qDebug() << "You Released Key A";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("TurnLeft",0.5);
        
		// IF OPPOSITE KEY IS PRESSED THEN DONT RESET VALUE TO ZERO
        if (pressedKey.D)
        {
            manual_control.r = keyControlValue.turnRight;
        }
        else
        {
            manual_control.r = 0;
        }
        pressedKey.A = false;
    }
    
    else if (event->key() == Qt::Key_D)
    {
        qDebug() << "You Released Key D";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("TurnRight",0.5);
        // IF OPPOSITE KEY IS PRESSED THEN DONT RESET VALUE TO ZERO
        if (pressedKey.A)
        {
            manual_control.r = keyControlValue.turnLeft;
        }
        else
        {
            manual_control.r = 0;
        }
        pressedKey.D = false;
    }

    else if (event->key() == Qt::Key_Up)
    {
        qDebug() << "You Released Key Up";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("ForWard",0.5);
        // IF OPPOSITE KEY IS PRESSED THEN DONT RESET VALUE TO ZERO
        if (pressedKey.Down)
        {
            manual_control.x = keyControlValue.backward;
        }
        else
        {
            manual_control.x = 0;
        }
        pressedKey.Up = false;
    }

    else if (event->key() == Qt::Key_Down)
    {
        qDebug() << "You Released Key Down";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("BackWard",0.5);
        // IF OPPOSITE KEY IS PRESSED THEN DONT RESET VALUE TO ZERO
        if (pressedKey.Up)
        {
            manual_control.x = keyControlValue.forward;
        }
        else
        {
            manual_control.x = 0;
        }
        pressedKey.Down = false;
    }
    
    else if (event->key() == Qt::Key_Left)
    {
        qDebug() << "You Released Key Left";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("MoveLeft",0.5);
        // IF OPPOSITE KEY IS PRESSED THEN DONT RESET VALUE TO ZERO
        if (pressedKey.Right)
        {
            manual_control.y = keyControlValue.rightward;
        }
        else
        {
            manual_control.y = 0;
        }
        pressedKey.Left = false;
    }

    else if (event->key() == Qt::Key_Right)
    {
        qDebug() << "You Released Key Right";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("MoveRight",0.5);
        // IF OPPOSITE KEY IS PRESSED THEN DONT RESET VALUE TO ZERO
        if (pressedKey.Left)
        {
            manual_control.y = keyControlValue.leftward;
        }
        else
        {
            manual_control.y = 0;
        }
        pressedKey.Right = false;
    }
  
 else if (event->key() == Qt::Key_R)
    {
        qDebug() << "You Released Key R";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("CamUp",0.5);
        manual_control.buttons = 0;
    }
 
 else if (event->key() == Qt::Key_F)
    {
        qDebug() << "You Released Key F";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("CamDown",0.5);
        manual_control.buttons = 0;
    }
 
 else if (event->key() == Qt::Key_T)
    {
        qDebug() << "You Released Key T";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("LightsUp",0.5);
        manual_control.buttons = 0;
    }
 
 else if (event->key() == Qt::Key_G)
    {
        qDebug() << "You Released Key G";
        UpdateKeyControlValue(false);
        SetQMLItemOpacity("LightsDown",0.5);
        manual_control.buttons = 0;
    }
    
    else
    {
        //qDebug() << "You Released NOT supported Key";
    }

    // SEND UPDATED INPUT VALUES TO DEBUG
    //qDebug() << "x: " << manual_control.x;
    //qDebug() << "y: " << manual_control.y;
    //qDebug() << "z: " << manual_control.z;
    //qDebug() << "r: " << manual_control.r;
}


void MainWindow::on_actionVideo_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(0);
    ui->actionVideo->setChecked(true);
    ui->actionVideo->setDisabled(true);
    ui->actionMenu->setChecked(false);
    ui->actionMenu->setDisabled(false);
    ui->actionSonarGps->setChecked(false);
    ui->actionSonarGps->setDisabled(false);
    qDebug() << "VIDEO PAGE DISPLAYED (1048)";
}


void MainWindow::on_actionMenu_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(1);
    ui->actionMenu->setChecked(true);
    ui->actionMenu->setDisabled(true);
    ui->actionVideo->setChecked(false);
    ui->actionVideo->setDisabled(false);
    ui->actionSonarGps->setChecked(false);
    ui->actionSonarGps->setDisabled(false);
    qDebug() << "HELP PAGE DISPLAYED (1061)";
}


void MainWindow::modeComboBox_currentIndexChanged(int index)
{
	//Depreciated...
}


void MainWindow::armCheckBox_stateChanged(bool checked)		// ARM & DISARM ROBOT
{
    if (!AS::as_api_check_vehicle(currentVehicle))
    {	
    	// IF NO CONNECTION TO ROBOT
        armCheckBox->setChecked(false);
        UpdateMapTopLableText("ROBOT DATA LINK FAILED");
        QObject* markerItem3=ui->quickWidget->rootObject()->findChild<QObject*>("ArmState");
    	markerItem3->setProperty("enabled",QVariant::fromValue(false));
        bConnection = false;
        qDebug() << "Robot is not connected (296)";
        return;
    }

    if (armCheckBox->isChecked())
    {
        // ARM ROBOT
        AS::as_api_vehicle_arm(currentVehicle, 1);
        UpdateMapTopLableText("");
        PlayMediaFileMapText("arm");

        SetQMLItemText("ArmState","ARMED");
        bArmState = true;
        SetQMLItemOpacity("ArmState",1.0);
           
		QObject* markerItem=ui->quickWidget->rootObject()->findChild<QObject*>("Speed");
   		markerItem->setProperty("enabled",QVariant::fromValue(true));
    	QObject* markerItem2=ui->quickWidget->rootObject()->findChild<QObject*>("DiveMode");
    	markerItem2->setProperty("enabled",QVariant::fromValue(true));

		SetQMLItemText("Speed","HIGH SPEED");
        bSpeed = true;
        SetQMLItemOpacity("Speed",1.0);

        SetQMLItemText("DiveMode","STABILIZED");
        bMode = true;
        SetQMLItemOpacity("DiveMode",1.0);
        UpdateKeyControlValue();

    	manual_control.buttons = 2056;     //Dual command - Stability and High Speed Mode

        qDebug() << "ROBOT ARMED";
        return;
    }
    if (!armCheckBox->isChecked())
    {
    	// CHANGE ALL ROBOT INPUT VALUES TO ZERO BEFORE DISARMING
        manual_control.x = 0;
        manual_control.y = 0;
        manual_control.z = 500;
        manual_control.r = 0;
        manual_control.buttons = 0;

        // DISARM ROBOT
        AS::as_api_vehicle_disarm(currentVehicle, 1);

		SetQMLItemText("ArmState","DISARMED");
        bArmState = false;
        SetQMLItemOpacity("ArmState",0.8);
    
		QObject* markerItem=ui->quickWidget->rootObject()->findChild<QObject*>("Speed");
   		markerItem->setProperty("enabled",QVariant::fromValue(false));
    	QObject* markerItem2=ui->quickWidget->rootObject()->findChild<QObject*>("DiveMode");
    	markerItem2->setProperty("enabled",QVariant::fromValue(false));

        UpdateMapTopLableText("");
        PlayMediaFileMapText("disarm");
        qDebug() << "DISARMING ROBOT (1176)";
        return;
    }
}


void MainWindow::on_actionSonarGps_triggered()
{
   //WHEN MAP BUTTON IS PRESSED SHOW MAP
    ui->mainStackedWidget->setCurrentIndex(2);
    ui->actionSonarGps->setChecked(true);
    ui->actionSonarGps->setDisabled(true);
    ui->actionMenu->setChecked(false);
    ui->actionMenu->setDisabled(false);
    ui->actionVideo->setChecked(false);
    ui->actionVideo->setDisabled(false);
    qDebug() << "DISPLAY MAP PAGE (1193)";
}


void MainWindow::on_updateConfidence()
{
    // UPDATE SONAR VALUES DISTANCE AND CONFIDENCE - ALSO UPDATE WARNING AND DANGER
    float fDistance=pingLink->getDistance()/1000.0;
    float fConfidence=pingLink->getConfidence();
    if (fConfidence = 100)
    {
        fConfidence = 99;
    }
    //QString strValue=QString("%1 M (%2\%)   ").arg(fDistance = floor(100*fDistance) / 100).arg(fConfidence);    
    QString strValue=QString("%1 M (%2\%)   ").arg(round(fDistance * 100) / 100.0).arg(fConfidence);    //LIMIT DISTANCE TO 0.00 
    SonarlValue->setText(strValue);

    // IF SONAR CONFIDENCE VALUE IS LESS THAN CONFIDENCE SETTING THEN OUTPUT FALSE - IGNORE SONAR DISTANCE VALUES
    if(fConfidence<ConfidenceSetting)
        return;
    QString strLabelName="FALSE SONAR: ";//normal
    QString strNormalsty="color: rgb(0, 0, 0);";
    QTime tcurrent=QTime::currentTime();
    QString mapTopLableText="";
    
    // IF SONAR DISTANCE VALUE IS GREATER THAN WARNING DISTANCE SETTING THEN OUTPUT NORMAL
    if(fDistance>WarnDistance)
    {
        SonarAlarm=false;
        strLabelName="SONAR: ";
        strNormalsty="color: rgb(0, 85, 0);";
        PrevTime=tcurrent;
        QString mapTopLableText="";
    }
    
    // IF SONAR DISTANCE VALUE IS LESS THAN WARNING DISTANCE SETTING THEN OUTPUT WARNING
    else if(fDistance>MinDistance&&fDistance<WarnDistance)
    {
        SonarAlarm=false;
        strLabelName="WARNING SONAR: ";
        strNormalsty="color: rgb(245, 81, 0);";
        PrevTime=tcurrent;
        QString mapTopLableText="";
        UpdateKeyControlValue();
        if (pressedKey.S)
        {
        	manual_control.z = keyControlValue.downward;
        }
        else if (pressedKey.Up)
        {
        	manual_control.x = keyControlValue.forward;
        }
        else if (pressedKey.Left)
        {
        	manual_control.y = keyControlValue.leftward;
        }
        else if (pressedKey.Right)
        {
        	manual_control.y = keyControlValue.rightward;
        }

    }
    
    // IF SONAR DISTANCE VALUE IS LESS THAN MIN DISTANCE SETTING THEN OUTPUT DANGER
    else if(fDistance<MinDistance)
    {
        SonarAlarm=true;
        strLabelName="DANGER SONAR: ";
        strNormalsty="color: rgb(255, 0, 0);";
        mapTopLableText="PROXIMITY ALERT";
        qDebug() << "PROXIMITY ALARM (1239)";

        // RESET ALL ROBOT INPUT VALUES TO ZERO - HALT ROBOT INPUT
        manual_control.x = 0;
        manual_control.y = 0;
        manual_control.z = 500;
        manual_control.r = 0;
        manual_control.buttons = 0;

        // IF SONAR DISTANCE VALUE IS LESS THAN MIN DISTANCE SETTING FOR ALARMSETTING SECONDS THEN OUTPUT COLLISION AVOIDANCE AND DISARM ROBOT
        if(PrevTime.msecsTo(tcurrent)/1000>AlarmSetting)
        {
            armCheckBox->setChecked(false);
            armCheckBox_stateChanged(true);
            PrevTime=tcurrent;
            mapTopLableText="OBSTACLE AVOIDANCE - DISARMED";
            PlayMediaFileMapText("collision");
            qDebug() << "OBSTACLE AVOIDANCE ENGAGED - DISARMING ROBOT";
        }
    }

    // SONAR VALUE UPDATE
    strNormalsty+="font: 87 10pt \"Arial Black\"";
    SonarLabel->setText(strLabelName);
    SonarLabel->setStyleSheet(strNormalsty);
    SonarlValue->setStyleSheet(strNormalsty);
    UpdateMapTopLableText(mapTopLableText);
}


void MainWindow::on_statusChanged(QQuickWidget::Status status)
{
    // MAP STUFF
    if(status==QQuickWidget::Ready)
    {
        qmlTimer=ui->quickWidget_2->rootObject()->findChild<QObject*>("qmlTimer");
        //use ini file Coordinates
        UpdateMapCenterCoordinates(fMapCoordinates);
        UpdateMarkerCoordinates(fMarkerCoordinates);
    }
}


void MainWindow::on_mainStackedWidget_currentChanged(int arg1)
{
 /*
    // AUTO MAP ZOOM FEATURE
    if(qmlTimer)
    {
        if(arg1==2)
        {
           QMetaObject::invokeMethod(qmlTimer,"start",Qt::QueuedConnection);
        }
        else
        {
            QMetaObject::invokeMethod(qmlTimer,"stop",Qt::QueuedConnection);
        }
    }
    */
}


void MainWindow::CheckRollOrPitchChang(bool bTimerOut)
{
	// CHECK ROBOT ROLL AND PITCH VALUES - IF THEY ARE THE SAME FOR X SECONDS THEN RESTART CONNECTION
    QString strRoll=rollLabelValue->text();
    QString strPitch=pitchLabelValue->text();

    if(strRollValue!=strRoll||strPitchValue!=strPitch)
    {
        strRollValue=strRoll;
        strPitchValue=strPitch;

        // START TIMER
        rollLPitchCheckTimer.start();
        

    }
    else
    {
        if(bTimerOut)
        {
            // RESTART CONNECTION

            RestartNetWork();
        }
    }
}


void MainWindow::RestartNetWork()
{
    // RESTART THE CONNECTION TO ROBOT

	// STOP ALL TIMERS
	qDebug() << "TRYING TO RECONNECT TO ROBOT AFTER TIMEOUT (1334)";
    rollLPitchCheckTimer.stop();
    vehicleDataUpdateTimer.stop();
    manualControlTimer.stop();
    qDebug() << "STOPPED ALL TIMERS (1338)";

    // RESET TO STARTING VALUES
    currentVehicle=1;
    firstRun = false;

    // DISARM AND DISCONNECT FROM ROBOT
    armCheckBox->setChecked(false);
    armCheckBox_stateChanged(true);
    AS::as_api_deinit();
    qDebug() << "DEINITIALIZED ROBOT CONNECTION (1348)";

    // RECONNECT TO THE ROBOT
    std::string ip("192.168.2.");
    AS::as_api_init(ip.c_str(), F_THREAD_ALL);
    qDebug() << "RETRYING ROBOT CONNECTION AGAIN (1353)";

    // RECONNECT TO PING SONAR AND RESTART TIMERS
    pingLink->connectLink();
    rollLPitchCheckTimer.start();
    vehicleDataUpdateTimer.start();
    manualControlTimer.start();
    qDebug() << "RESTARTED TIMERS (1360)";
}


void MainWindow::LoadInIConfig()
{
    // LOAD IN CONFIG FILE
    QSettings sets("Teleportal.ini",QSettings::IniFormat);
    if(!sets.allKeys().size())
    {
        // GPS VALUES
        sets.setValue("GPS/MapCoordinates",QStringList{"-14.0094983494893","80.1233232234234"});
        sets.setValue("GPS/MarkerCoordinates",QStringList{"-14.0094983494893","80.1233232234234"});
        sets.setValue("GPS/ardusubCoordinates",false);

        // KEYBOARD COMMAND PRESS
        sets.setValue("KEYBOARD_KEYPRESS/forward",500);
        sets.setValue("KEYBOARD_KEYPRESS/backward",-500);
        sets.setValue("KEYBOARD_KEYPRESS/leftward",-500);
        sets.setValue("KEYBOARD_KEYPRESS/rightward",500);
        sets.setValue("KEYBOARD_KEYPRESS/upward",700);
        sets.setValue("KEYBOARD_KEYPRESS/downward",300);
        sets.setValue("KEYBOARD_KEYPRESS/turnLeft",-500);
        sets.setValue("KEYBOARD_KEYPRESS/turnRight",500);
        sets.setValue("KEYBOARD_KEYPRESS/turnLeftM",-500);
        sets.setValue("KEYBOARD_KEYPRESS/turnRightM",500);

        // KEYBOARD COMMAND HOLD
        sets.setValue("KEYBOARD_KEYHOLD/forward",999);
        sets.setValue("KEYBOARD_KEYHOLD/backward",-999);
        sets.setValue("KEYBOARD_KEYHOLD/leftward",-999);
        sets.setValue("KEYBOARD_KEYHOLD/rightward",999);
        sets.setValue("KEYBOARD_KEYHOLD/upward",900);
        sets.setValue("KEYBOARD_KEYHOLD/downward",100);
        sets.setValue("KEYBOARD_KEYHOLD/turnLeft",-999);
        sets.setValue("KEYBOARD_KEYHOLD/turnRight",999);
        sets.setValue("KEYBOARD_KEYHOLD/turnLeftM",-300);
        sets.setValue("KEYBOARD_KEYHOLD/turnRightM",300);

        // KEYBOARD COMMAND HOLD - SONAR WARNING
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_WARNING/forward",500);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_WARNING/backward",-999);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_WARNING/leftward",-999);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_WARNING/rightward",999);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_WARNING/upward",900);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_WARNING/downward",300);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_WARNING/turnLeft",-999);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_WARNING/turnRight",999);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_WARNING/turnLeftM",-300);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_WARNING/turnRightM",300);

        // KEYBOARD COMMAND PRESS - SONAR WARNING
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_WARNING/forward",500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_WARNING/backward",-500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_WARNING/leftward",-500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_WARNING/rightward",500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_WARNING/upward",700);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_WARNING/downward",300);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_WARNING/turnLeft",-500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_WARNING/turnRight",500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_WARNING/turnLeftM",-500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_WARNING/turnRightM",500);

        // KEYBOARD COMMAND HOLD - SONAR ALARM
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_ALARM/forward",0);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_ALARM/backward",-999);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_ALARM/leftward",-999);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_ALARM/rightward",999);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_ALARM/upward",900);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_ALARM/downward",500);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_ALARM/turnLeft",-999);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_ALARM/turnRight",999);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_ALARM/turnLeftM",-300);
        sets.setValue("KEYBOARD_KEYHOLD_SONAR_ALARM/turnRightM",300);

        // KEYBOARD COMMAND PRESS - SONAR ALARM
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_ALARM/forward",0);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_ALARM/backward",-500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_ALARM/leftward",-500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_ALARM/rightward",500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_ALARM/upward",700);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_ALARM/downward",500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_ALARM/turnLeft",-500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_ALARM/turnRight",500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_ALARM/turnLeftM",-500);
        sets.setValue("KEYBOARD_KEYPRESS_SONAR_ALARM/turnRightM",500);

        // SONAR SETTINGS 
        sets.setValue("SONAR/WarnDistance",1.5);
        sets.setValue("SONAR/MinDistance",0.75);
        sets.setValue("SONAR/AlarmSetting",3);
        sets.setValue("SONAR/ConfidenceSetting",90);
        sets.setValue("SONAR/PingInterval",250);

        // GAMEPAD SETTINGS
        sets.setValue("GAMEPAD/buttonL1","Qt::Key_F");
        sets.setValue("GAMEPAD/buttonR1","Qt::Key_R");
        sets.setValue("GAMEPAD/buttonUp","Qt::Key_T");
        sets.setValue("GAMEPAD/buttonDown","Qt::Key_G");
        sets.setValue("GAMEPAD/buttonSelect","Qt::Key_4");
        sets.setValue("GAMEPAD/buttonStart","Qt::Key_5");
        sets.setValue("GAMEPAD/buttonX","Qt::Key_1");
        sets.setValue("GAMEPAD/buttonY","Qt::Key_2");
        sets.setValue("GAMEPAD/buttonB","Qt::Key_3");

        // MISC SETTINGS

        // TIME IN SECONDS OF INACTIVITY BEFORE AUTO DISARM OF ROBOT
        sets.setValue("MISC/IdleSetting",180);

        // TIME IN SECONDS SINCE THE LAST PICTURE WAS TAKEN BEFORE A NEW ONE CAN BE TAKEN
        sets.setValue("MISC/PhotoDelay",5);
    }

    // HERE WE TRANSFER TELEPORTAL.INI TO VARIABLES IN APP

    // GPS SETTINGS
    fMapCoordinates=sets.value("GPS/MapCoordinates",QStringList{"-14.0094983494893","80.1233232234234"}).toStringList();
    fMarkerCoordinates=sets.value("GPS/MarkerCoordinates",QStringList{"-14.0094983494893","80.1233232234234"}).toStringList();
    bardusubCoordinates=sets.value("GPS/ardusubCoordinates").toBool();

    // KEYBOARD CONTROL SETTINGS
    keyControlValue_Press.forward=sets.value("KEYBOARD_KEYPRESS/forward").toInt();
    keyControlValue_Press.backward=sets.value("KEYBOARD_KEYPRESS/backward").toInt();
    keyControlValue_Press.leftward=sets.value("KEYBOARD_KEYPRESS/leftward").toInt();
    keyControlValue_Press.rightward=sets.value("KEYBOARD_KEYPRESS/rightward").toInt();
    keyControlValue_Press.upward=sets.value("KEYBOARD_KEYPRESS/upward").toInt();
    keyControlValue_Press.downward=sets.value("KEYBOARD_KEYPRESS/downward").toInt();
    keyControlValue_Press.turnLeft=sets.value("KEYBOARD_KEYPRESS/turnLeft").toInt();
    keyControlValue_Press.turnRight=sets.value("KEYBOARD_KEYPRESS/turnRight").toInt();
    keyControlValue_Press.turnLeftM=sets.value("KEYBOARD_KEYPRESS/turnLeftM").toInt();
    keyControlValue_Press.turnRightM=sets.value("KEYBOARD_KEYPRESS/turnRightM").toInt();

    keyControlValue_Hold.forward=sets.value("KEYBOARD_KEYHOLD/forward").toInt();
    keyControlValue_Hold.backward=sets.value("KEYBOARD_KEYHOLD/backward").toInt();
    keyControlValue_Hold.leftward=sets.value("KEYBOARD_KEYHOLD/leftward").toInt();
    keyControlValue_Hold.rightward=sets.value("KEYBOARD_KEYHOLD/rightward").toInt();
    keyControlValue_Hold.upward=sets.value("KEYBOARD_KEYHOLD/upward").toInt();
    keyControlValue_Hold.downward=sets.value("KEYBOARD_KEYHOLD/downward").toInt();
    keyControlValue_Hold.turnLeft=sets.value("KEYBOARD_KEYHOLD/turnLeft").toInt();
    keyControlValue_Hold.turnRight=sets.value("KEYBOARD_KEYHOLD/turnRight").toInt();
    keyControlValue_Hold.turnLeftM=sets.value("KEYBOARD_KEYHOLD/turnLeftM").toInt();
    keyControlValue_Hold.turnRightM=sets.value("KEYBOARD_KEYHOLD/turnRightM").toInt();

    keyControlValue_Warning.forward=sets.value("KEYBOARD_KEYPRESS_SONAR_WARNING/forward").toInt();
    keyControlValue_Warning.backward=sets.value("KEYBOARD_KEYPRESS_SONAR_WARNING/backward").toInt();
    keyControlValue_Warning.leftward=sets.value("KEYBOARD_KEYPRESS_SONAR_WARNING/leftward").toInt();
    keyControlValue_Warning.rightward=sets.value("KEYBOARD_KEYPRESS_SONAR_WARNING/rightward").toInt();
    keyControlValue_Warning.upward=sets.value("KEYBOARD_KEYPRESS_SONAR_WARNING/upward").toInt();
    keyControlValue_Warning.downward=sets.value("KEYBOARD_KEYPRESS_SONAR_WARNING/downward").toInt();
    keyControlValue_Warning.turnLeft=sets.value("KEYBOARD_KEYPRESS_SONAR_WARNING/turnLeft").toInt();
    keyControlValue_Warning.turnRight=sets.value("KEYBOARD_KEYPRESS_SONAR_WARNING/turnRight").toInt();
    keyControlValue_Warning.turnLeftM=sets.value("KEYBOARD_KEYPRESS_SONAR_WARNING/turnLeftM").toInt();
    keyControlValue_Warning.turnRightM=sets.value("KEYBOARD_KEYPRESS_SONAR_WARNING/turnRightM").toInt();

    keyControlValue_HoldWarning.forward=sets.value("KEYBOARD_KEYHOLD_SONAR_WARNING/forward").toInt();
    keyControlValue_HoldWarning.backward=sets.value("KEYBOARD_KEYHOLD_SONAR_WARNING/backward").toInt();
    keyControlValue_HoldWarning.leftward=sets.value("KEYBOARD_KEYHOLD_SONAR_WARNING/leftward").toInt();
    keyControlValue_HoldWarning.rightward=sets.value("KEYBOARD_KEYHOLD_SONAR_WARNING/rightward").toInt();
    keyControlValue_HoldWarning.upward=sets.value("KEYBOARD_KEYHOLD_SONAR_WARNING/upward").toInt();
    keyControlValue_HoldWarning.downward=sets.value("KEYBOARD_KEYHOLD_SONAR_WARNING/downward").toInt();
    keyControlValue_HoldWarning.turnLeft=sets.value("KEYBOARD_KEYHOLD_SONAR_WARNING/turnLeft").toInt();
    keyControlValue_HoldWarning.turnRight=sets.value("KEYBOARD_KEYHOLD_SONAR_WARNING/turnRight").toInt();
    keyControlValue_HoldWarning.turnLeftM=sets.value("KEYBOARD_KEYHOLD_SONAR_WARNING/turnLeftM").toInt();
    keyControlValue_HoldWarning.turnRightM=sets.value("KEYBOARD_KEYHOLD_SONAR_WARNING/turnRightM").toInt();

    keyControlValue_HoldAlarm.forward=sets.value("KEYBOARD_KEYHOLD_SONAR_ALARM/forward").toInt();
    keyControlValue_HoldAlarm.backward=sets.value("KEYBOARD_KEYHOLD_SONAR_ALARM/backward").toInt();
    keyControlValue_HoldAlarm.leftward=sets.value("KEYBOARD_KEYHOLD_SONAR_ALARM/leftward").toInt();
    keyControlValue_HoldAlarm.rightward=sets.value("KEYBOARD_KEYHOLD_SONAR_ALARM/rightward").toInt();
    keyControlValue_HoldAlarm.upward=sets.value("KEYBOARD_KEYHOLD_SONAR_ALARM/upward").toInt();
    keyControlValue_HoldAlarm.downward=sets.value("KEYBOARD_KEYHOLD_SONAR_ALARM/downward").toInt();
    keyControlValue_HoldAlarm.turnLeft=sets.value("KEYBOARD_KEYHOLD_SONAR_ALARM/turnLeft").toInt();
    keyControlValue_HoldAlarm.turnRight=sets.value("KEYBOARD_KEYHOLD_SONAR_ALARM/turnRight").toInt();
    keyControlValue_HoldAlarm.turnLeftM=sets.value("KEYBOARD_KEYHOLD_SONAR_ALARM/turnLeftM").toInt();
    keyControlValue_HoldAlarm.turnRightM=sets.value("KEYBOARD_KEYHOLD_SONAR_ALARM/turnRightM").toInt();

    keyControlValue_Alarm.forward=sets.value("KEYBOARD_KEYPRESS_SONAR_ALARM/forward").toInt();
    keyControlValue_Alarm.backward=sets.value("KEYBOARD_KEYPRESS_SONAR_ALARM/backward").toInt();
    keyControlValue_Alarm.leftward=sets.value("KEYBOARD_KEYPRESS_SONAR_ALARM/leftward").toInt();
    keyControlValue_Alarm.rightward=sets.value("KEYBOARD_KEYPRESS_SONAR_ALARM/rightward").toInt();
    keyControlValue_Alarm.upward=sets.value("KEYBOARD_KEYPRESS_SONAR_ALARM/upward").toInt();
    keyControlValue_Alarm.downward=sets.value("KEYBOARD_KEYPRESS_SONAR_ALARM/downward").toInt();
    keyControlValue_Alarm.turnLeft=sets.value("KEYBOARD_KEYPRESS_SONAR_ALARM/turnLeft").toInt();
    keyControlValue_Alarm.turnRight=sets.value("KEYBOARD_KEYPRESS_SONAR_ALARM/turnRight").toInt();
    keyControlValue_Alarm.turnLeftM=sets.value("KEYBOARD_KEYPRESS_SONAR_ALARM/turnLeftM").toInt();
    keyControlValue_Alarm.turnRightM=sets.value("KEYBOARD_KEYPRESS_SONAR_ALARM/turnRightM").toInt();

    // SONAR SETTINGS
    WarnDistance=sets.value("SONAR/WarnDistance").toFloat();
    MinDistance=sets.value("SONAR/MinDistance").toFloat();
    AlarmSetting=sets.value("SONAR/AlarmSetting").toInt();
    ConfidenceSetting=sets.value("SONAR/ConfidenceSetting").toInt();
    PingSensor::_firmwareDefaultPingInterval=sets.value("SONAR/PingInterval").toInt();

    // MISC SETTINGS
    iIdleSetting=sets.value("MISC/IdleSetting").toUInt();

    // PHOTO SETTINGS
    strTakPhontoName=sets.value("MISC/TakPhontoName").toString();
    strRemoteDir=sets.value("MISC/RemoteDir").toString();
    strHost=sets.value("MISC/host").toString();
    strUser=sets.value("MISC/user").toString();
    strPass=sets.value("MISC/pass").toString();
    strPhotoDelay=sets.value("MISC/PhotoDelay").toInt();
    qDebug() << "LOADED CONFIG FILE (1571)";
}


void MainWindow::UpdateMapCenterCoordinates(QStringList coord)
{
    // UPDATE MAP CENTER
    QObject* mapView=ui->quickWidget_2->rootObject()->findChild<QObject*>("qmlMapView");
    QGeoCoordinate qmlCoord=mapView->property("center").value<QGeoCoordinate>();
    qmlCoord.setLatitude(coord.at(0).toDouble());
    qmlCoord.setLongitude(coord.at(1).toDouble());
    mapView->setProperty("center",QVariant::fromValue(qmlCoord));
}


void MainWindow::UpdateMarkerCoordinates(QStringList coord)
{
    // UPDATE ROBOT ICON LOCATION
    QObject* markerItem=ui->quickWidget_2->rootObject()->findChild<QObject*>("markerItem");
    QGeoCoordinate qmlCoord=markerItem->property("coordinate").value<QGeoCoordinate>();
    qmlCoord.setLatitude(coord.at(0).toDouble());
    qmlCoord.setLongitude(coord.at(1).toDouble());
    markerItem->setProperty("coordinate",QVariant::fromValue(qmlCoord));
}


void MainWindow::UpdateModeLable()
{
    // CHECK ROBOT DATA AND CHANGE BUTTON TO SHOW ACTUAL ROBOT MODE
    if(vehicle_data)
    {
    //    if(vehicle_data->custom_mode==AS::ALT_HOLD)
    //    {
    //    }
    //    else if(vehicle_data->custom_mode==AS::MANUAL)        //Problem with it since limiting button presses - needs timer to add delay?
    //    {
    //    }
    //    else if(vehicle_data->custom_mode==AS::STABILIZE)
    //    {
    //    }
        
  		// CHECK ROBOT DATA AND CHANGE BUTTON TO SHOW ACTUAL ROBOT ARM STATUS WITH 2 SECOND DELAY

        if(vehicle_data->system_status==3&&bArmState==true)
        {
            QTimer::singleShot(2000, this,[&]()
    			{
                    if(vehicle_data->system_status==3&&bArmState==true)
         				{
						armCheckBox->setChecked(false);
            			armCheckBox_stateChanged(false);
         				}
    			});
        }
    }
}


void MainWindow::HandleNewKey(QKeyEvent *event)
{
    	// THIS HAS BEEN REMOVED
}


void MainWindow::InitGamePad()
{
    connect(QGamepadManager::instance(),SIGNAL(gamepadConnected(int)), this,SLOT(on_gamepadConnected(int)));
    connect(QGamepadManager::instance(),SIGNAL(gamepadDisconnected(int)), this,SLOT(on_gamepadDisconnected(int)));
}


void MainWindow::LoadMapingKey()
{
    // LOAD GAMEPAD MAPPING KEYS
    static QMap<QString,Qt::Key> keymap{
        {"Qt::Key_F",Qt::Key_F},
        {"Qt::Key_R",Qt::Key_R},
        {"Qt::Key_T",Qt::Key_T},
        {"Qt::Key_G",Qt::Key_G},
        {"Qt::Key_4",Qt::Key_4},
        {"Qt::Key_5",Qt::Key_5},
        {"Qt::Key_2",Qt::Key_2},
        {"Qt::Key_1",Qt::Key_1},
        {"Qt::Key_3",Qt::Key_3}
    };

    if(_gameKeyNavigation)
    {
         QSettings sets("Teleportal.ini",QSettings::IniFormat);
         _gameKeyNavigation->setButtonL1Key(keymap[sets.value("GAMEPAD/buttonL1","Qt::Key_F").toString()]);
         _gameKeyNavigation->setButtonR1Key(keymap[sets.value("GAMEPAD/buttonR1","Qt::Key_R").toString()]);
         _gameKeyNavigation->setUpKey(keymap[sets.value("GAMEPAD/buttonUp","Qt::Key_T").toString()]);
         _gameKeyNavigation->setDownKey(keymap[sets.value("GAMEPAD/buttonDown","Qt::Key_G").toString()]);
         _gameKeyNavigation->setButtonSelectKey(keymap[sets.value("GAMEPAD/buttonSelect","Qt::Key_4").toString()]);
         _gameKeyNavigation->setButtonStartKey(keymap[sets.value("GAMEPAD/buttonStart","Qt::Key_5").toString()]);
         _gameKeyNavigation->setButtonXKey(keymap[sets.value("GAMEPAD/buttonX","Qt::Key_2").toString()]);
         _gameKeyNavigation->setButtonYKey(keymap[sets.value("GAMEPAD/buttonY","Qt::Key_1").toString()]);
         _gameKeyNavigation->setButtonBKey(keymap[sets.value("GAMEPAD/buttonB","Qt::Key_3").toString()]);

         //unused gamepad buttons
         _gameKeyNavigation->setLeftKey(Qt::Key_unknown);
         _gameKeyNavigation->setRightKey(Qt::Key_unknown);
         _gameKeyNavigation->setButtonAKey(Qt::Key_unknown);
         _gameKeyNavigation->setButtonL2Key(Qt::Key_unknown);
         _gameKeyNavigation->setButtonL3Key(Qt::Key_unknown);
         _gameKeyNavigation->setButtonR2Key(Qt::Key_unknown);
         _gameKeyNavigation->setButtonR3Key(Qt::Key_unknown);
         _gameKeyNavigation->setButtonGuideKey(Qt::Key_unknown);
    }
}


void MainWindow::UpdateKeyControlValue(bool bPress)
{
    // ON KEYBOARD INPUT CHANGE INPUT VALUES BASED ON SONAR DISTANCE VALUE
    // THESE VALUES SEEM WRONG - BUT WORK
    keyControlValue_t keyControlValue_Up;
    keyControlValue_t keyControlValue_Down;
    QString strLable=SonarLabel->text();
    if(strLable.contains("DANGER",Qt::CaseInsensitive))
    {
        keyControlValue_Up=keyControlValue_Alarm;
        keyControlValue_Down=keyControlValue_HoldAlarm;
    }
    else if(strLable.contains("WARNING",Qt::CaseInsensitive))
    {
        keyControlValue_Up=keyControlValue_Warning;
        keyControlValue_Down=keyControlValue_HoldWarning;
    }
    else
    {
        keyControlValue_Up=keyControlValue_Press;
        keyControlValue_Down=keyControlValue_Hold;
    }
    if(bPress)
    {
        keyControlValue=keyControlValue_Down;
    }
    else
    {
        keyControlValue=keyControlValue_Up;
    }
}


void MainWindow::on_setQmlText()
{
    // RESET LARGE WARNING TEXT IF NO WARNING
    QObject* markerItem=ui->quickWidget->rootObject()->findChild<QObject*>("videoLabel");
    if(markerItem)
    {
        QString qmlText=markerItem->property("text").value<QString>();
        if(mapTextCache!=qmlText)
        {
            qmlText=mapTextCache;
            markerItem->setProperty("text",QVariant::fromValue(qmlText));
        }
        mapTextCache="";
    }
}


void MainWindow::UpdateMapTopLableText(QString strTip)
{
	// RESET COLLISION WARNING AFTER 3 SECONDS
    mapTextCache=strTip;
    if(bmapState)
        return;
    emit SetQMLText();
    if(strTip=="OBSTACLE AVOIDANCE - DISARMED")
    {
        bmapState=true;
        QTimer::singleShot(3000,this, [&](){
            bmapState=false;
            emit SetQMLText();
        });
    }
}


void MainWindow::PlayMediaFileMapText(QString strText)
{
    // PLAY MP3 FILE
        // PlayMediaFileMapText("disarm");
        // fileUrl=QUrl::fromLocalFile("/Users/me/Music/coolsong.mp3");
        // fileUrl=QUrl("http://");
        // fileUrl=QUrl("qrc:/assets/mp3/disarm.mp3");

        if(!player)
        {
            player = new QMediaPlayer;
        }
        QUrl fileUrl;//local or net
        if(strText=="disarm")
        {
            fileUrl=QUrl("qrc:/assets/mp3/disarm.mp3");
        }
        else if(strText=="arm")
        {
            fileUrl=QUrl("qrc:/assets/mp3/arm.mp3");
        }
         else if(strText=="stability")
        {
            fileUrl=QUrl("qrc:/assets/mp3/stability.mp3");
        }
         else if(strText=="depth")
        {
            fileUrl=QUrl("qrc:/assets/mp3/depth.mp3");
        }
         else if(strText=="manual")
        {
            fileUrl=QUrl("qrc:/assets/mp3/manual.mp3");
        }
         else if(strText=="collision")
        {
            fileUrl=QUrl("qrc:/assets/mp3/collision.mp3");
        }
         else if(strText=="timeout")
        {
            fileUrl=QUrl("qrc:/assets/mp3/timeout.mp3");
        }
        else if(strText=="reconnect")
        {
            fileUrl=QUrl("qrc:/assets/mp3/reconnect.mp3");
        }
        else if(strText=="photo")
        {
            fileUrl=QUrl("qrc:/assets/mp3/photo.mp3");
        }
        player->setMedia(fileUrl);
        
        // MUTE VOLUME IF CHECKED ON TOOLBAR
        if(!ui->actionMute->isChecked())
        {
            player->setVolume(50);
        }
        else
        {
             player->setVolume(0);
        }

        player->play();
}


void MainWindow::on_axisLeftXChanged(double value)
{
    // SIMPLE GAMEPAD TO KEYBOARD INPUTS FOR X AXIS

    //value rang -1.0 1.0
    QKeyEvent *event=nullptr;
    if(value>0.8)
    {
        //right
        event=new QKeyEvent(QEvent::KeyPress,Qt::Key_Right, Qt::NoModifier);
    }
    else if(value<-0.8)
    {
        //left
        event=new QKeyEvent(QEvent::KeyPress,Qt::Key_Left, Qt::NoModifier);
    }
    else if(value>-0.1&&value < 0.1)
    {
        //mid
        if(pressedKey.Left == true)
            event=new QKeyEvent(QEvent::KeyRelease,Qt::Key_Left, Qt::NoModifier);
        else if(pressedKey.Right == true)
            event=new QKeyEvent(QEvent::KeyRelease,Qt::Key_Right, Qt::NoModifier);
    }
    if(event)
    {
        QGuiApplication::sendEvent(this, event);
    }
}


void MainWindow::on_axisLeftYChanged(double value)
{
    // SIMPLE GAMEPAD TO KEYBOARD INPUTS FOR Y AXIS
    QKeyEvent *event=nullptr;
    if(value<-0.8)
    {
        //forward
         event=new QKeyEvent(QEvent::KeyPress,Qt::Key_Up, Qt::NoModifier);
    }
    else if(value>0.8)
    {
        //Reverse
        event=new QKeyEvent(QEvent::KeyPress,Qt::Key_Down, Qt::NoModifier);
    }
    else if(value>-0.1&&value < 0.1)
    {
        //mid
        if(pressedKey.Up == true)
            event=new QKeyEvent(QEvent::KeyRelease,Qt::Key_Up, Qt::NoModifier);
        else if(pressedKey.Down == true)
            event=new QKeyEvent(QEvent::KeyRelease,Qt::Key_Down, Qt::NoModifier);
    }
    if(event)
    {
        QGuiApplication::sendEvent(this, event);
    }
}


void MainWindow::on_axisRightXChanged(double value)
{
    // SIMPLE GAMEPAD TO KEYBOARD INPUTS FOR ROTATION AXIS
    QKeyEvent *event=nullptr;
    if(value>0.8)
    {
        //yaw right
        event=new QKeyEvent(QEvent::KeyPress,Qt::Key_D, Qt::NoModifier);
    }
    else if(value<-0.8)
    {
        //yaw left
        event=new QKeyEvent(QEvent::KeyPress,Qt::Key_A, Qt::NoModifier);
    }
    else if(value>-0.1&&value < 0.1)
    {
        //mid
        if(pressedKey.D == true)
            event=new QKeyEvent(QEvent::KeyRelease,Qt::Key_D, Qt::NoModifier);
        else if(pressedKey.A == true)
            event=new QKeyEvent(QEvent::KeyRelease,Qt::Key_A, Qt::NoModifier);
    }
    if(event)
    {
        QGuiApplication::sendEvent(this, event);
    }
}


void MainWindow::on_axisRightYChanged(double value)
{
    // SIMPLE GAMEPAD TO KEYBOARD INPUTS FOR Z AXIS
    QKeyEvent *event=nullptr;
    if(value<-0.8)
    {
        //ascend
         event=new QKeyEvent(QEvent::KeyPress,Qt::Key_W, Qt::NoModifier);
    }
    else if(value>0.8)
    {
        //descend
         event=new QKeyEvent(QEvent::KeyPress,Qt::Key_S, Qt::NoModifier);
    }
    else if(value>-0.1&&value < 0.1)
    {
        //mid
        if(pressedKey.W == true)
            event=new QKeyEvent(QEvent::KeyRelease,Qt::Key_W, Qt::NoModifier);
        else if(pressedKey.S == true)
            event=new QKeyEvent(QEvent::KeyRelease,Qt::Key_S, Qt::NoModifier);
    }
    if(event)
    {
        QGuiApplication::sendEvent(this, event);
    }
}


void MainWindow::on_gamepadConnected(int deviceId)
{
    // CONNECT GAMEPAD
    if(!_gamepad)
    {
        _gamepad=new QGamepad(deviceId,this);
        if(!_gameKeyNavigation)
        {
            _gameKeyNavigation=new QGamepadKeyNavigation(this);
            _gameKeyNavigation->setGamepad(_gamepad);
            LoadMapingKey();
        }
    }
    connect(_gamepad,&QGamepad::axisLeftXChanged,this,&MainWindow::on_axisLeftXChanged);
    connect(_gamepad,&QGamepad::axisLeftYChanged,this,&MainWindow::on_axisLeftYChanged);
    connect(_gamepad,&QGamepad::axisRightXChanged,this,&MainWindow::on_axisRightXChanged);
    connect(_gamepad,&QGamepad::axisRightYChanged,this,&MainWindow::on_axisRightYChanged);
}


void MainWindow::on_gamepadDisconnected(int deviceId)
{
    // DISCONNECT GAMEPAD
    if(_gamepad)
    {
        if(_gamepad->deviceId()==deviceId)
        {
            disconnect(_gamepad,&QGamepad::axisLeftXChanged,this,&MainWindow::on_axisLeftXChanged);
            disconnect(_gamepad,&QGamepad::axisLeftYChanged,this,&MainWindow::on_axisLeftYChanged);
            disconnect(_gamepad,&QGamepad::axisRightXChanged,this,&MainWindow::on_axisRightXChanged);
            disconnect(_gamepad,&QGamepad::axisRightYChanged,this,&MainWindow::on_axisRightYChanged);
        }
        if(_gameKeyNavigation)
        {
            _gameKeyNavigation->deleteLater();
            _gameKeyNavigation=nullptr;
        }
        _gamepad->deleteLater();
        _gamepad=nullptr;
    }
}


void MainWindow::on_actionTakePhoto_triggered()
{
    if(!bsftpIdle)
        return;
    bsftpIdle=false;
    // TAKE SCREENSHOT PHOTO AND UPLOAD TO SFTP SERVER

    // DISABLE PHOTO ICON FOR PHOTO DELAY SETTING SECONDS
    ui->actionTakePhoto->setDisabled(true);
    /*
            QTimer::singleShot(strPhotoDelay*1000, this,[&]()
            {
                 ui->actionTakePhoto->setEnabled(true);
            });
      */      
    // TAKE SCREENSHOT OF PROGRAM WINDOW
    std::thread uploaderThread([&](){
    QScreen* scr=this->screen();
    QPixmap result = scr->grabWindow(this->winId());

    // SAVE FILE LOCALLYS USING PHOTO SETTINGS
    QFileInfo info(strTakPhontoName);
    QString strTime=QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    QString strLocaTempFile="sftpfile/";
    strLocaTempFile+=info.baseName();
    strLocaTempFile+=strTime;
    strLocaTempFile+='.';
    strLocaTempFile+=info.suffix();
    QDir dir;
    dir.mkdir("sftpfile");
    result.save(strLocaTempFile);
    PlayMediaFileMapText("photo");

    // UPLOAD LOCAL FILE TO SFTP SERVER
    SecureFileUploader sftp;
        
        QEventLoop loop;
        connect(&sftp,&SecureFileUploader::SftpEndcomplete,[&]{			//Looked Nice, but didnt work, never ended loop...
            bsftpIdle=true;
            ui->actionTakePhoto->setEnabled(true);
            loop.quit();
        });

    sftp.upload(strLocaTempFile,strRemoteDir,strHost,strUser,strPass);
      
        loop.exec();
    });

    uploaderThread.detach();
}


void  MainWindow::On_QML_ArmState()
{
    qDebug()<<"On_QML_ArmState";
   
   if (bArmState)
   {
      		armCheckBox->setChecked(false);
            armCheckBox_stateChanged(true);
            SetQMLItemVar("rectangle","arm",QVariant::fromValue(true));
    }

   

   else if (!bArmState)
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
            SetQMLItemVar("rectangle","arm",QVariant::fromValue(false));
        }
}


void  MainWindow::On_QML_DiveMode()
{
    qDebug()<<"On_QML_DiveMode";
   
   if (bMode)
   {
    if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
            return;
         }
            qDebug() << "You Pressed Key 3 - MANUAL MODE";
            SetQMLItemText("DiveMode","MANUAL");
            bMode = false;
            SetQMLItemOpacity("DiveMode",0.8);
            manual_control.buttons = 2;
            UpdateKeyControlValue();
            SetQMLItemVar("rectangle","mode",QVariant::fromValue(false));
   }
   else if (!bMode)
        {
           if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
            return;
         }
            qDebug() << "You Pressed Key 4 - STABILIZED MODE";
            SetQMLItemText("DiveMode","STABILIZED");
            bMode = true;
            SetQMLItemOpacity("DiveMode",1.0);
            manual_control.buttons = 8;
            UpdateKeyControlValue();
            SetQMLItemVar("rectangle","mode",QVariant::fromValue(true));
        }
}
void  MainWindow::On_QML_Speed()
{
     qDebug()<<"On_QML_Speed Button Clicked";

     if (bSpeed)
        {
            if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
            return;
         }
            qDebug() << "You Pressed Key 1 - LOW SPEED";
            SetQMLItemText("Speed","LOW SPEED");
            bSpeed = false;
            SetQMLItemOpacity("Speed",0.8);
            manual_control.buttons = 4096;
            SetQMLItemVar("rectangle","speed",QVariant::fromValue(false));
        }
     else if (!bSpeed)
        {
           if (!armCheckBox->isChecked())
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
            return;
         }
            qDebug() << "You Pressed Key 2 - HIGH SPEED";
            SetQMLItemText("Speed","HIGH SPEED");
            bSpeed = true;
            SetQMLItemOpacity("Speed",1.0);
            manual_control.buttons = 2048;
            SetQMLItemVar("rectangle","speed",QVariant::fromValue(true));
        }
}

void  MainWindow::Call_QML_ChangeKey(const QString& strKey)
{
    //Depreciated...
}
void  MainWindow::SetQMLItemOpacity(QString strObName,qreal fOpacity)
{
    if(m_qmlObjectMap.find(strObName)==m_qmlObjectMap.end())
        return;
    m_qmlObjectMap[strObName]->setProperty("opacity",QVariant::fromValue(fOpacity));
}

void  MainWindow::SetQMLItemText(QString strObName,QString strButtonText)
{
    if(m_qmlObjectMap.find(strObName)==m_qmlObjectMap.end())
        return;
    m_qmlObjectMap[strObName]->setProperty("text",QVariant::fromValue(strButtonText));
}
void  MainWindow::SetQMLItemVar(QString strObName,QString strVarName,const QVariant& var)
{
    if(m_qmlObjectMap.find(strObName)==m_qmlObjectMap.end())
        return;
    m_qmlObjectMap[strObName]->setProperty(strVarName.toStdString().c_str(),var);
}
void  MainWindow::On_QML_StatusChanged(QQuickWidget::Status status)
 {
    if(status==QQuickWidget::Ready)
    {
        auto hButton=ui->quickWidget->rootObject()->findChild<QObject*>("DiveMode");
        QObject::connect(hButton,SIGNAL(clicked()),this,SLOT(On_QML_DiveMode()));
        m_qmlObjectMap["DiveMode"]=hButton;
        hButton=ui->quickWidget->rootObject()->findChild<QObject*>("Speed");
        QObject::connect(hButton,SIGNAL(clicked()),this,SLOT(On_QML_Speed()));
        m_qmlObjectMap["Speed"]=hButton;
        hButton=ui->quickWidget->rootObject()->findChild<QObject*>("ArmState");
        QObject::connect(hButton,SIGNAL(clicked()),this,SLOT(On_QML_ArmState()));
        m_qmlObjectMap["ArmState"]=hButton;
        m_qmlObjectMap["rectangle"]=ui->quickWidget->rootObject();
        //keyimg
        QStringList strImgList={"ForWard","MoveLeft","BackWard",
                                "MoveRight","Up","TurnLeft","Down",
                                "TurnRight","CamUp","LightsUp",
                               "CamDown","LightsDown"};

       for(auto i:strImgList)
       {
           m_qmlObjectMap[i]=ui->quickWidget->rootObject()->findChild<QObject*>(i);
       }

    }
 }


	// HEY THIS WAS A HARD PROJECT TO LEARN TO CODE WITH QT & C++ FOR FIRST TIME - GIVE ME A BREAK
	// adam@osibot.com
