#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("open success");
        //将Qbyte转化为QString
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }
    else{
        qDebug("open failed");
    }

    QString fileName = "config.ini";
    QString appPath = QCoreApplication::applicationDirPath();
    QString configPath = QDir::toNativeSeparators(appPath + QDir::separator() + fileName);
    qDebug() << configPath;
    QSettings settings(configPath, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://" + gate_host + ":" + gate_port;
    qDebug() << gate_url_prefix;
    MainWindow w;
    w.show();
    return a.exec();
}
