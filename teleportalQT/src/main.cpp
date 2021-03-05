#include <QApplication>

#include "mainwindow.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QtMsgHandler>
#include <QMessageLogContext>
#include <QString>
#define MAX_LOG_SIZE 1024*1024*10       //10M
QString  g_LogFile;
void OutDebugInfoTofile()
{
//#ifndef _DEBUG
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg){

        static QMutex mutex;
        QMutexLocker locker(&mutex);
        QString strType;
         switch (type)
         {
            case QtDebugMsg:
             strType="QtDebugMsg";
             break;
            case QtInfoMsg:
             strType="QtInfoMsg";
             break;
            case QtWarningMsg:
             strType="QtWarningMsg";
             break;
            case QtCriticalMsg:
             strType="QtCriticalMsg";
             break;
            case QtFatalMsg:
             strType="QtFatalMsg";
             break;
         }
        QString strOutMsg=QString("%1 %2:FILE:%3 LINE:%4 MSG:%5").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(strType).arg(context.file).arg(context.line).arg(msg);

        QFile data(g_LogFile);
        if (data.open(QFile::WriteOnly | QFile::Append)) {
              if(data.size()>MAX_LOG_SIZE)
              {
                  data.resize(0);
              }
              QTextStream out(&data);
              out <<strOutMsg<<"\r\n";
              data.flush();
              data.close();
          }


    });

 //#endif
}


int main(int argc, char *argv[])
{
    gst_init (&argc, &argv);
    QApplication a(argc, argv);
    QDir dir;
    dir.mkdir("log");
    QString strTime=QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    QString strLocaLogFile="log/";
    strLocaLogFile+=strTime;
    strLocaLogFile+=".log";
    g_LogFile=strLocaLogFile;
    OutDebugInfoTofile();
    MainWindow w;
    w.showMaximized();
    w.setWindowFlags(Qt::FramelessWindowHint);      //ENABLE FULLSCREEN WINDOW FOR APP TO RUN
    w.resizeWindowsManual();
    w.show();
    return a.exec();
}
