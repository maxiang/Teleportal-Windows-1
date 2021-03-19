#include <QApplication>

#include "mainwindow.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QtMsgHandler>
#include <QMessageLogContext>
#include <QString>

//Add crash dump file
#include <windows.h>
#include <dbghelp.h>
#include <QDir>

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

//Add crash dump file
long ApplicationCrashHandler(EXCEPTION_POINTERS *pException){


        QDir *dmp = new QDir;
        QString ExceptDmpPath = QCoreApplication::applicationDirPath()+"/ExceptDmp/";
        bool exist = dmp->exists(ExceptDmpPath);
        if(exist == false)
            dmp->mkdir(ExceptDmpPath);

        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy_MM_dd_hh_mm_ss");
        QString time =  current_date + ".dmp";
          EXCEPTION_RECORD *record = pException->ExceptionRecord;
          QString errCode(QString::number(record->ExceptionCode, 16));
          QString errAddr(QString::number((uint64_t)record->ExceptionAddress, 16));
          QString errFlag(QString::number(record->ExceptionFlags, 16));
          QString errPara(QString::number(record->NumberParameters, 16));
          qDebug()<<"errCode: "<<errCode;
          qDebug()<<"errAddr: "<<errAddr;
          qDebug()<<"errFlag: "<<errFlag;
          qDebug()<<"errPara: "<<errPara;
          HANDLE hDumpFile = CreateFile((LPCWSTR)QString(ExceptDmpPath + time).utf16(),
                   GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
          if(hDumpFile != INVALID_HANDLE_VALUE) {
              MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
              dumpInfo.ExceptionPointers = pException;
              dumpInfo.ThreadId = GetCurrentThreadId();
              dumpInfo.ClientPointers = TRUE;
              MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
              CloseHandle(hDumpFile);
          }
          else{
              qDebug()<<"hDumpFile == null";
          }
          return EXCEPTION_EXECUTE_HANDLER;
}


int main(int argc, char *argv[])
{
    gst_init (&argc, &argv);
    QApplication a(argc, argv);
    QDir dir;
    dir.mkdir("log");
    QString strTime=QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    QString strLocaLogFile="log/";
	//QString strLocaLogFile="%USERPROFILE%/Documents/Teleportal/";
	//QString strLocaLogFile=QDir::homePath();
	//QString strLocaLogFile=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	
    strLocaLogFile+=strTime;
    strLocaLogFile+=".log";
    g_LogFile=strLocaLogFile;
    OutDebugInfoTofile();

    //Add crash dump file
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
    
    MainWindow w;
    w.showMaximized();
    w.setWindowFlags(Qt::FramelessWindowHint);      //ENABLE FULLSCREEN WINDOW FOR APP TO RUN
    w.resizeWindowsManual();
    w.show();
    return a.exec();
}
