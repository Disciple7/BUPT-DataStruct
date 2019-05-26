#include"log.h"
#include"server.h"
#include<iostream>
#include"mainwindow.h"

//注：日志功能已经整合到MainWindow中。现在支持任意信息的日志了，并且能够保存在文件中。

QString str2qstr(const string str)//QString和string互相转码函数
{
    return QString::fromLocal8Bit(str.data());
}
string qstr2str(const QString qstr)
{
    QByteArray cdata = qstr.toLocal8Bit();
    return string(cdata);
}
