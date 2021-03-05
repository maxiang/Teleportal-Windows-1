/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtQuickWidgets/QQuickWidget>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "qFlightInstruments.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionVideo;
    QAction *actionMenu;
    QAction *actionSonarGps;
    QAction *actionMute;
    QAction *actionTakePhoto;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QStackedWidget *mainStackedWidget;
    QWidget *videoPage;
    QQuickWidget *quickWidget;
    QADI *qADI;
    QCompass *qCompass;
    QWidget *menuPage;
    QWidget *mapPage;
    QHBoxLayout *horizontalLayout_2;
    QQuickWidget *quickWidget_2;
    QToolBar *tabsToolBar;
    QToolBar *vehicleToolBar;
    QToolBar *statusToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1200, 700);
        QFont font;
        font.setFamily(QString::fromUtf8("ADMUI3Lg"));
        MainWindow->setFont(font);
        MainWindow->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        actionVideo = new QAction(MainWindow);
        actionVideo->setObjectName(QString::fromUtf8("actionVideo"));
        actionVideo->setCheckable(true);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/assets/icons/video.svg"), QSize(), QIcon::Normal, QIcon::Off);
        actionVideo->setIcon(icon);
        actionMenu = new QAction(MainWindow);
        actionMenu->setObjectName(QString::fromUtf8("actionMenu"));
        actionMenu->setCheckable(true);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/assets/icons/help.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMenu->setIcon(icon1);
        actionSonarGps = new QAction(MainWindow);
        actionSonarGps->setObjectName(QString::fromUtf8("actionSonarGps"));
        actionSonarGps->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/assets/icons/map.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSonarGps->setIcon(icon2);
        actionMute = new QAction(MainWindow);
        actionMute->setObjectName(QString::fromUtf8("actionMute"));
        actionMute->setCheckable(true);
        actionMute->setChecked(false);
        actionMute->setEnabled(true);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/assets/icons/mute_off.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon3.addFile(QString::fromUtf8(":/assets/icons/mute_on.png"), QSize(), QIcon::Normal, QIcon::On);
        icon3.addFile(QString::fromUtf8(":/assets/icons/mute_on.png"), QSize(), QIcon::Disabled, QIcon::Off);
        icon3.addFile(QString::fromUtf8(":/assets/icons/mute_off.png"), QSize(), QIcon::Selected, QIcon::Off);
        icon3.addFile(QString::fromUtf8(":/assets/icons/mute_on.png"), QSize(), QIcon::Selected, QIcon::On);
        actionMute->setIcon(icon3);
        actionTakePhoto = new QAction(MainWindow);
        actionTakePhoto->setObjectName(QString::fromUtf8("actionTakePhoto"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/assets/icons/camera.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon4.addFile(QString::fromUtf8(":/assets/icons/camera_busy.png"), QSize(), QIcon::Disabled, QIcon::Off);
        actionTakePhoto->setIcon(icon4);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        mainStackedWidget = new QStackedWidget(centralwidget);
        mainStackedWidget->setObjectName(QString::fromUtf8("mainStackedWidget"));
        videoPage = new QWidget();
        videoPage->setObjectName(QString::fromUtf8("videoPage"));
        quickWidget = new QQuickWidget(videoPage);
        quickWidget->setObjectName(QString::fromUtf8("quickWidget"));
        quickWidget->setGeometry(QRect(10, 10, 400, 400));
        quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        qADI = new QADI(videoPage);
        qADI->setObjectName(QString::fromUtf8("qADI"));
        qADI->setGeometry(QRect(1030, 200, 160, 160));
        qADI->setStyleSheet(QString::fromUtf8(""));
        qCompass = new QCompass(videoPage);
        qCompass->setObjectName(QString::fromUtf8("qCompass"));
        qCompass->setGeometry(QRect(1030, 30, 160, 160));
        qCompass->setStyleSheet(QString::fromUtf8(""));
        mainStackedWidget->addWidget(videoPage);
        menuPage = new QWidget();
        menuPage->setObjectName(QString::fromUtf8("menuPage"));
        mainStackedWidget->addWidget(menuPage);
        mapPage = new QWidget();
        mapPage->setObjectName(QString::fromUtf8("mapPage"));
        horizontalLayout_2 = new QHBoxLayout(mapPage);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        quickWidget_2 = new QQuickWidget(mapPage);
        quickWidget_2->setObjectName(QString::fromUtf8("quickWidget_2"));
        quickWidget_2->setResizeMode(QQuickWidget::SizeRootObjectToView);

        horizontalLayout_2->addWidget(quickWidget_2);

        mainStackedWidget->addWidget(mapPage);

        horizontalLayout->addWidget(mainStackedWidget);

        MainWindow->setCentralWidget(centralwidget);
        tabsToolBar = new QToolBar(MainWindow);
        tabsToolBar->setObjectName(QString::fromUtf8("tabsToolBar"));
        tabsToolBar->setMinimumSize(QSize(0, 0));
        tabsToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
        tabsToolBar->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        tabsToolBar->setMovable(false);
        tabsToolBar->setIconSize(QSize(36, 36));
        tabsToolBar->setFloatable(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, tabsToolBar);
        vehicleToolBar = new QToolBar(MainWindow);
        vehicleToolBar->setObjectName(QString::fromUtf8("vehicleToolBar"));
        vehicleToolBar->setMinimumSize(QSize(0, 0));
        vehicleToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
        vehicleToolBar->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        vehicleToolBar->setMovable(false);
        vehicleToolBar->setAllowedAreas(Qt::TopToolBarArea);
        vehicleToolBar->setIconSize(QSize(36, 36));
        vehicleToolBar->setFloatable(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, vehicleToolBar);
        statusToolBar = new QToolBar(MainWindow);
        statusToolBar->setObjectName(QString::fromUtf8("statusToolBar"));
        statusToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
        statusToolBar->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        statusToolBar->setMovable(false);
        statusToolBar->setIconSize(QSize(36, 36));
        statusToolBar->setFloatable(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, statusToolBar);

        tabsToolBar->addAction(actionVideo);
        tabsToolBar->addAction(actionSonarGps);
        tabsToolBar->addAction(actionMute);
        tabsToolBar->addAction(actionTakePhoto);
        tabsToolBar->addAction(actionMenu);

        retranslateUi(MainWindow);

        mainStackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Teleportal", nullptr));
        actionVideo->setText(QCoreApplication::translate("MainWindow", "Video", nullptr));
#if QT_CONFIG(tooltip)
        actionVideo->setToolTip(QCoreApplication::translate("MainWindow", "Robot Video", nullptr));
#endif // QT_CONFIG(tooltip)
        actionMenu->setText(QCoreApplication::translate("MainWindow", "Menu", nullptr));
#if QT_CONFIG(tooltip)
        actionMenu->setToolTip(QCoreApplication::translate("MainWindow", "Help", nullptr));
#endif // QT_CONFIG(tooltip)
        actionSonarGps->setText(QCoreApplication::translate("MainWindow", "SonarGps", nullptr));
#if QT_CONFIG(tooltip)
        actionSonarGps->setToolTip(QCoreApplication::translate("MainWindow", "Robot Location", nullptr));
#endif // QT_CONFIG(tooltip)
        actionMute->setText(QCoreApplication::translate("MainWindow", "Mute", nullptr));
#if QT_CONFIG(tooltip)
        actionMute->setToolTip(QCoreApplication::translate("MainWindow", "Mute Audio", nullptr));
#endif // QT_CONFIG(tooltip)
        actionTakePhoto->setText(QCoreApplication::translate("MainWindow", "Take Photo", nullptr));
        tabsToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Tabs", nullptr));
        vehicleToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Vehicle", nullptr));
        statusToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Status", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
