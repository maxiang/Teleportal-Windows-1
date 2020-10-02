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
    manual_control.buttons = 0;

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
    m_modeIndex = 1;
    IdleTime = 1;
    firstRun = false;
    robot_status_delay = false;

    // SETUP VIDEO AND TOOLBAR
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

    // SETUP MAIN LOOP
    connect(this,SIGNAL(SetQMLText()),this,SLOT(on_setQmlText()));
    setupTimer();
    videoReceiver->start(ui->quickWidget);
    UpdateMapTopLableText("NO CONNECTION TO ROBOT");

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
}


MainWindow::~MainWindow()
{
    //ON SHUTDOWN DISARM ROBOT
    armCheckBox->setChecked(false);
    armCheckBox_stateChanged(Qt::Unchecked);
    delete ui;
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
   armCheckBox->setStyleSheet("border-style: outset;"
                              "border-width: 1px;"
                              "border-radius: 2px;"
                              "border-color: beige;"
                              "min-width: 10em;"
                              "padding: 6px;"
                              "background:rgb(0, 255, 0);"
                              "color: rgb(255, 255, 255);"
                              "font: 87 12pt \"Arial Black\";");
    armCheckBox->setFocusPolicy(Qt::NoFocus);
    AddToolBarSpacer(ui->vehicleToolBar);
    ui->vehicleToolBar->addWidget(armCheckBox);
    AddToolBarSpacer(ui->vehicleToolBar);
    connect(armCheckBox,SIGNAL(clicked(bool)),
                     this,SLOT(armCheckBox_stateChanged(bool)));
    UpdateMapTopLableText("NO CONNECTION TO ROBOT");
    AddToolBarSpacer(ui->tabsToolBar,100);
    QLabel *modeLable = new QLabel(this);
    modeLable->setText("DIVE MODE: ");
    modeLable->setStyleSheet("font: 87 10pt \"Arial Black\"");
    ui->tabsToolBar->addWidget(modeLable);
    modeComboBox = new QPushButton(this);
    modeComboBox->setText("Unknown");
    modeComboBox->setStyleSheet("font: 87 10pt \"Arial Black\"");
    modeComboBox->setFocusPolicy(Qt::NoFocus);
    modeComboBox->show();
    connect (modeComboBox , SIGNAL(clicked()) , this , SLOT(on_modeBt_clicked()) );
    ui->tabsToolBar->addWidget(modeComboBox);

    // SONAR DISPLAY
    SonarLabel=new QLabel("SONAR: ");
    SonarlValue = new QLabel("21.0 M (95%)   ");
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
    yawLabelValue->setFixedWidth(50);
    pitchLabelValue->setFixedWidth(50);
    rollLabelValue->setFixedWidth(50);
    depthLabelValue->setFixedWidth(100);

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

   	//WHEN USER CLICKS ON DIVE MODE BUTTON - CHANGE MODE
    QKeyEvent* event=nullptr;

		if (!armCheckBox->isChecked())		
        {
            return;
         }

	    if(modeComboBox->text()=="Depth Hold")
	    {
	        event=new QKeyEvent(QEvent::KeyPress,Qt::Key_3, Qt::NoModifier);
	    }
	    else if(modeComboBox->text()=="Stability")
	    {
	        event=new QKeyEvent(QEvent::KeyPress,Qt::Key_2, Qt::NoModifier);
	    }
	    else if(modeComboBox->text()=="Manual")
	    {
	        event=new QKeyEvent(QEvent::KeyPress,Qt::Key_1, Qt::NoModifier);
	    }
		if(event)
		    {
		        QGuiApplication::sendEvent(this, event);
		    }
			    
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
    //UPDATES ROBOT VARIABLES & MAIN TIMING LOOP

    if (!AS::as_api_check_vehicle(currentVehicle))
    {
        // IF ROBOT IS NOT CONNECTED RETURN
        return;
    }

    if (!firstRun)
    {
        // SEND COMMANDS TO ROBOT ON STARTUP (DISARM, STABILITY MODE)
            armCheckBox->setChecked(false);				// DISARM ROBOT
            armCheckBox_stateChanged(true);
            firstRun = true;
            //QGuiApplication::sendEvent(this,new QKeyEvent(QEvent::KeyPress,Qt::Key_B, Qt::NoModifier));
            m_modeIndex = 1;
            manual_control.buttons = 8;            		 // STABILITY MODE
            modeComboBox->setText("Stability");

            PlayMediaFileMapText("reconnect");			//PLAY WELCOME BACK MP3
    }

    //GET ROBOT DATA
    AS::as_api_get_vehicle_data2(currentVehicle, vehicle_data);

    if (vehicle_data == nullptr)
    {
        // IF ROBOT IS NOT CONNECTED RETURN
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
            //use dev coord
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
            pImgItem->setRotation(yawLableCompass);

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
            armCheckBox_stateChanged(Qt::Unchecked);
            PlayMediaFileMapText("timeout");
        }

    //UPDATE DIVE MODE LABEL    
    CheckRollOrPitchChang(false);
    UpdateModeLable();
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
    }
}


void MainWindow::resizeWindowsManual()
{
    int m_width = ui->videoPage->width();

    ui->quickWidget->setGeometry(0, 0, m_width, ui->videoPage->height());

    ui->qCompass->setGeometry(m_width - 160, 0, 160, 160);
    ui->qADI->setGeometry(m_width - 160, 160, 160, 160);
    //ui->TakePhoto->move(m_width - 160+(ui->qADI->width()/2-ui->TakePhoto->width()/2),326);
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
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())		
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

        // SEND COMMAND TO ROBOT
        qDebug() << "You Pressed Key W";
        UpdateKeyControlValue();
        pressedKey.W = true;
        manual_control.z = keyControlValue.upward;		
    }

    // KEY S - DOWN
    else if (event->key() == Qt::Key_S)
    {
        // CHECK IF ROBOT IS ARMED, IF NOT REARM ROBOT
        if (!armCheckBox->isChecked())		
        {
            armCheckBox->setChecked(true);
            armCheckBox_stateChanged(true);
         }

        qDebug() << "You Pressed Key S - Down";
        UpdateKeyControlValue();

        // IF PROXIMITY ALERT THEN DONT SEND COMMAND
        if(!SonarAlarm)					
        {
            // SEND COMMAND TO ROBOT
            pressedKey.S = true;
            manual_control.z = keyControlValue.downward;		
        }
    }

    // KEY A - ROTATE ROBOT LEFT
    else if (event->key() == Qt::Key_A)
    {
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
        if (m_modeIndex == 0)			
        {
            manual_control.r = keyControlValue.turnLeftM;
        }
        else if (m_modeIndex == 1)
        {
            manual_control.r = keyControlValue.turnLeft;
        }
        else if (m_modeIndex == 2)
        {
            manual_control.r = keyControlValue.turnLeft;
        }
    }

    // KEY D - ROTATE ROBOT RIGHT
    else if (event->key() == Qt::Key_D)
    {
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
        if (m_modeIndex == 0)
        {
            manual_control.r = keyControlValue.turnRightM;
        }
        else if (m_modeIndex == 1)
        {
           manual_control.r = keyControlValue.turnRight;
        }
        else if (m_modeIndex == 2)
        {
            manual_control.r = keyControlValue.turnRight;
        }
    }

    // KEY UP - MOVE ROBOT FORWARD
    else if (event->key() == Qt::Key_Up)
    {
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
        }
         qDebug() << "You Pressed Key Up";
    }

    // KEY DOWN - MOVE ROBOT BACKWARD
    else if (event->key() == Qt::Key_Down)
    {
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


      // KEY 1 - STABILITY MODE
      else if (event->key() == Qt::Key_1)
     {
         qDebug() << "You Pressed Key 1 - STABILITY MODE";
         UpdateKeyControlValue();
         m_modeIndex = 1;
         manual_control.buttons = 8;
         modeComboBox->setText("Stability");
         PlayMediaFileMapText("stability");
     }
   
   // KEY 2 - DEPTH HOLD MODE
   else if (event->key() == Qt::Key_2)
     {
         qDebug() << "You Pressed Key 2 - DEPTH HOLD MODE";
         UpdateKeyControlValue();
         m_modeIndex = 2;
         manual_control.buttons = 4;
         modeComboBox->setText("Depth Hold");
         PlayMediaFileMapText("depth");
     }
     
     // KEY 3 - MANUAL MODE
     else if (event->key() == Qt::Key_3)
     {
         qDebug() << "You Pressed Key 3 - MANUAL MODE";
         UpdateKeyControlValue();
         m_modeIndex = 0;
         manual_control.buttons = 2;
         modeComboBox->setText("Manual");
         PlayMediaFileMapText("manual");
     }

     // KEY 4 - DISARM ROBOT
     else if (event->key() == Qt::Key_4)
     {
         qDebug() << "You Pressed Key 4 - DISARM ROBOT";
         UpdateKeyControlValue();

         // RESET ROBOT INPUT VALUES BEFORE DISARMING
         manual_control.x = 0;
         manual_control.y = 0;
         manual_control.z = 500;
         manual_control.r = 0;
         manual_control.buttons = 0;

         // DISARM ROBOT
         AS::as_api_vehicle_disarm(currentVehicle, 1);
         armCheckBox->setStyleSheet("border-style: outset;"
                                    "border-width: 1px;"
                                    "border-radius: 2px;"
                                    "border-color: beige;"
                                    "min-width: 10em;"
                                    "padding: 6px;"
                                    "background:rgb(0, 255, 0);"
                                    "color: rgb(255, 255, 255);"
                                    "font: 87 12pt \"Arial Black\";");
         armCheckBox->setText("CLICK TO START - ROBOT DISARMED");
         armCheckBox->setChecked(false);
         UpdateMapTopLableText("");
         PlayMediaFileMapText("disarm");
         qDebug() << "DISARM";
     }

     // KEY 5 - ARM ROBOT
     else if (event->key() == Qt::Key_5)
     {
         qDebug() << "You Pressed Key 5 - ARM ROBOT";
         UpdateKeyControlValue();

         // ARM ROBOT
         AS::as_api_vehicle_arm(currentVehicle, 1);
         armCheckBox->setStyleSheet("border-style: outset;"
                                    "border-width: 1px;"
                                    "border-radius: 2px;"
                                    "border-color: beige;"
                                    "min-width: 10em;"
                                    "padding: 6px;"
                                    "background:rgb(255,0, 0);"
                                    "color: rgb(255, 255, 255);"
                                    "font: 87 12pt \"Arial Black\";");
         armCheckBox->setText("ROBOT ARMED");
         armCheckBox->setChecked(true);
         UpdateMapTopLableText("");
         PlayMediaFileMapText("arm");
         qDebug() << "ARM";
         
     }

    else
    {
        qDebug() << "You Pressed NOT supported Key";
    }

    // OUTPUT VALUE TO DEBUG
    qDebug() << "x: " << manual_control.x;
    qDebug() << "y: " << manual_control.y;
    qDebug() << "z: " << manual_control.z;
    qDebug() << "r: " << manual_control.r;
}


void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // ON KEY RELEASE CHANGE ROBOT COMMANDS BACK TO IDLE VALUES
    if (event->isAutoRepeat())
    {
         // IF THE KEY RELEASES TO MANY TIMES? I HAVE NO IDEA WHY THIS IS HERE.
        return;
    }

    if (event->key() == Qt::Key_W)
    {
        qDebug() << "You Released Key W";
        UpdateKeyControlValue(false);
        
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
        manual_control.buttons = 0;
    }
 
 else if (event->key() == Qt::Key_F)
    {
        qDebug() << "You Released Key F";
        UpdateKeyControlValue(false);
        manual_control.buttons = 0;
    }
 
 else if (event->key() == Qt::Key_T)
    {
        qDebug() << "You Released Key T";
        UpdateKeyControlValue(false);
        manual_control.buttons = 0;
    }
 
 else if (event->key() == Qt::Key_G)
    {
        qDebug() << "You Released Key G";
        UpdateKeyControlValue(false);
        manual_control.buttons = 0;
    }
    
    else
    {
        qDebug() << "You Released NOT supported Key";
    }

    // SEND UPDATED INPUT VALUES TO DEBUG
    qDebug() << "x: " << manual_control.x;
    qDebug() << "y: " << manual_control.y;
    qDebug() << "z: " << manual_control.z;
    qDebug() << "r: " << manual_control.r;
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
}


void MainWindow::modeComboBox_currentIndexChanged(int index)
{
	// CHANGE MODE OF ROBOT WHEN MODE BUTTON IS CLICKED
    
	// CHECK IS ROBOT IS CONNECTED FIRST
    if (!AS::as_api_check_vehicle(currentVehicle))
    {
        qDebug() << "vehicle: " << currentVehicle << "is not ready!";
        return;
    }

    // CHANGE MODE ON ROBOT
    switch (index)
    {
    case 0:
        manual_control.buttons = 2;
        modeComboBox->setText("Manual");
        break;

    case 1:
        manual_control.buttons = 8;
        modeComboBox->setText("Stability");
        break;

    case 2:
        manual_control.buttons = 4;
        modeComboBox->setText("Depth Hold");
        break;

    default:
        break;
    }
}


void MainWindow::armCheckBox_stateChanged(bool checked)
{
    // ARM & DISARM ROBOT 

    if (!AS::as_api_check_vehicle(currentVehicle))
    {
        
    	// IF NO CONNECTION TO ROBOT
        qDebug() << "vehicle: " << currentVehicle << "is not ready!";
        armCheckBox->setChecked(false);
        armCheckBox->setStyleSheet("border-style: outset;"
                                   "border-width: 1px;"
                                   "border-radius: 2px;"
                                   "border-color: beige;"
                                   "min-width: 10em;"
                                   "padding: 6px;"
                                   "background:rgb(0, 255, 0);"
                                   "color: rgb(255, 255, 255);"
                                   "font: 87 12pt \"Arial Black\";");
        armCheckBox->setText("CLICK TO START - ROBOT DISARMED");
        UpdateMapTopLableText("NO CONNECTION TO ROBOT");
        return;
    }

    if (armCheckBox->isChecked())
    {
        
    	// ARM ROBOT
        AS::as_api_vehicle_arm(currentVehicle, 1);
        armCheckBox->setStyleSheet("border-style: outset;"
                                   "border-width: 1px;"
                                   "border-radius: 2px;"
                                   "border-color: beige;"
                                   "min-width: 10em;"
                                   "padding: 6px;"
                                   "background:rgb(255,0, 0);"
                                   "color: rgb(255, 255, 255);"
                                   "font: 87 12pt \"Arial Black\";");
        armCheckBox->setText("ROBOT ARMED");
        UpdateMapTopLableText("");
        PlayMediaFileMapText("arm");
        qDebug() << "ARM";
    }
    else
    {
    	// CHANGE ALL ROBOT INPUT VALUES TO ZERO BEFORE DISARMING
        manual_control.x = 0;
        manual_control.y = 0;
        manual_control.z = 500;
        manual_control.r = 0;
        manual_control.buttons = 0;

        // DISARM ROBOT
        AS::as_api_vehicle_disarm(currentVehicle, 1);
        armCheckBox->setStyleSheet("border-style: outset;"
                                   "border-width: 1px;"
                                   "border-radius: 2px;"
                                   "border-color: beige;"
                                   "min-width: 10em;"
                                   "padding: 6px;"
                                   "background:rgb(0, 255, 0);"
                                   "color: rgb(255, 255, 255);"
                                   "font: 87 12pt \"Arial Black\";");
        armCheckBox->setText("CLICK TO START - ROBOT DISARMED");
        UpdateMapTopLableText("");
        PlayMediaFileMapText("disarm");
        qDebug() << "DISARM";
    }
}


void MainWindow::on_actionSonarGps_triggered()
{
    ui->mainStackedWidget->setCurrentIndex(2);
    ui->actionSonarGps->setChecked(true);
    ui->actionSonarGps->setDisabled(true);
    ui->actionMenu->setChecked(false);
    ui->actionMenu->setDisabled(false);
    ui->actionVideo->setChecked(false);
    ui->actionVideo->setDisabled(false);

}


void MainWindow::on_updateConfidence()
{
    // UPDATE SONAR VALUES DISTANCE AND CONFIDENCE - ALSO UPDATE WARNING AND DANGER
    float fDistance=pingLink->getDistance()/1000.0;
    float fConfidence=pingLink->getConfidence();
    QString strValue=QString("%1 M (%2\%)   ").arg(fDistance).arg(fConfidence);
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
    }
    
    // IF SONAR DISTANCE VALUE IS LESS THAN WARNING DISTANCE SETTING THEN OUTPUT WARNING
    else if(fDistance>MinDistance&&fDistance<WarnDistance)
    {
        SonarAlarm=false;
        strLabelName="WARNING SONAR: ";
        strNormalsty="color: rgb(245, 81, 0);";
        PrevTime=tcurrent;
    }
    
    // IF SONAR DISTANCE VALUE IS LESS THAN MIN DISTANCE SETTING THEN OUTPUT DANGER
    else if(fDistance<MinDistance)
    {
        SonarAlarm=true;
        strLabelName="-DANGER- SONAR: ";
        strNormalsty="color: rgb(255, 0, 0);";
        mapTopLableText="PROXIMITY ALARM";

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
            mapTopLableText="OBSTACLE AVOIDANCE ENGAGED - DISARMING ROBOT";
            PlayMediaFileMapText("collision");
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
    // MAP STUFF?
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
    rollLPitchCheckTimer.stop();
    vehicleDataUpdateTimer.stop();
    manualControlTimer.stop();

    // RESET TO STARTING VALUES
    currentVehicle=1;
    firstRun = false;

    // DISARM AND DISCONNECT FROM ROBOT
    armCheckBox->setChecked(false);
    armCheckBox_stateChanged(Qt::Unchecked);
    AS::as_api_deinit();

    // RECONNECT TO THE ROBOT
    std::string ip("192.168.2.");
    AS::as_api_init(ip.c_str(), F_THREAD_ALL);

    // RECONNECT TO PING SONAR AND RESTART TIMERS
    pingLink->connectLink();
    rollLPitchCheckTimer.start();
    vehicleDataUpdateTimer.start();
    manualControlTimer.start();
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
    // THIS HAS BEEN REMOVED
    if(vehicle_data)
    {
        if(vehicle_data->custom_mode==AS::ALT_HOLD)
        {
             modeComboBox->setText("Depth Hold");
             m_modeIndex = 2;
        }
        else if(vehicle_data->custom_mode==AS::MANUAL)
        {
            modeComboBox->setText("Manual");
            m_modeIndex = 0;
        }
        else if(vehicle_data->custom_mode==AS::STABILIZE)
        {
            modeComboBox->setText("Stability");
            m_modeIndex = 1;
        }
        
        //if(vehicle_data->system_status==AS::SYS_DISARMED&&armCheckBox->text()!="CLICK TO START - ROBOT DISARMED")
        if(vehicle_data->system_status==3&&armCheckBox->text()!="CLICK TO START - ROBOT DISARMED")
        {
            QTimer::singleShot(2000, this,[&]()
    			{
         			if(vehicle_data->system_status==3&&armCheckBox->text()!="CLICK TO START - ROBOT DISARMED")
         				{
         				armCheckBox->setStyleSheet("border-style: outset;"
                                       "border-width: 1px;"
                                       "border-radius: 2px;"
                                       "border-color: beige;"
                                       "min-width: 10em;"
                                       "padding: 6px;"
                                       "background:rgb(0, 255, 0);"
                                       "color: rgb(255, 255, 255);"
                                       "font: 87 12pt \"Arial Black\";");
            			armCheckBox->setText("CLICK TO START - ROBOT DISARMED");
            			armCheckBox->setChecked(false);
            			UpdateMapTopLableText("");
            			PlayMediaFileMapText("disarm");
         				}
    			});
             
        }
        
        qDebug() << "mode: " << vehicle_data->custom_mode;
    	qDebug() << "status: " << vehicle_data->system_status;
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
        {"Qt::Key_O",Qt::Key_O},
        {"Qt::Key_L",Qt::Key_L},
        {"Qt::Key_H",Qt::Key_H},
        {"Qt::Key_B",Qt::Key_B},
        {"Qt::Key_M",Qt::Key_M}
    };
    if(_gameKeyNavigation)
    {
         QSettings sets("Teleportal.ini",QSettings::IniFormat);
         _gameKeyNavigation->setButtonL1Key(keymap[sets.value("GAMEPAD/buttonL1","Qt::Key_F").toString()]);
         _gameKeyNavigation->setButtonR1Key(keymap[sets.value("GAMEPAD/buttonR1","Qt::Key_R").toString()]);
         _gameKeyNavigation->setUpKey(keymap[sets.value("GAMEPAD/buttonUp","Qt::Key_T").toString()]);
         _gameKeyNavigation->setDownKey(keymap[sets.value("GAMEPAD/buttonDown","Qt::Key_G").toString()]);
         //new key
         _gameKeyNavigation->setButtonSelectKey(keymap[sets.value("GAMEPAD/buttonSelect","Qt::Key_O").toString()]);
         _gameKeyNavigation->setButtonStartKey(keymap[sets.value("GAMEPAD/buttonStart","Qt::Key_L").toString()]);
         _gameKeyNavigation->setButtonXKey(keymap[sets.value("GAMEPAD/buttonX","Qt::Key_H").toString()]);
         _gameKeyNavigation->setButtonYKey(keymap[sets.value("GAMEPAD/buttonY","Qt::Key_B").toString()]);
         _gameKeyNavigation->setButtonBKey(keymap[sets.value("GAMEPAD/buttonB","Qt::Key_M").toString()]);

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
    // THESE VALUES SEEM WRONG - BUT WORK?
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
	// RESET COLLISION WARNING AFTER 5 SECONDS
    mapTextCache=strTip;
    if(bmapState)
        return;
    emit SetQMLText();
    if(strTip=="OBSTACLE AVOIDANCE ENGAGED - DISARMING ROBOT")
    {
        bmapState=true;
        QTimer::singleShot(5000,this, [&](){
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
    // TAKE SCREENSHOT PHOTO AND UPLOAD TO SFTP SERVER

    // DISABLE PHOTO ICON FOR PHOTO DELAY SETTING SECONDS
    ui->actionTakePhoto->setDisabled(true);
    QTimer::singleShot(strPhotoDelay*1000, this,[&]()
    {
         ui->actionTakePhoto->setEnabled(true);
    });

    // TAKE SCREENSHOT OF PROGRAM WINDOW
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
    static SecureFileUploader sftp;
    connect(&sftp,&SecureFileUploader::SftpEndcomplete,this,[&]{
        bsftpIdle=true;

    });
    sftp.upload(strLocaTempFile,strRemoteDir,strHost,strUser,strPass);
}

	// HEY THIS WAS A HARD PROJECT TO LEARN TO CODE WITH - GIVE ME A BREAK
