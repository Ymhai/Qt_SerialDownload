#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>         // 提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>     // 提供系统中存在的串口的信息
#include "qdebug.h"
#include <QFileDialog>
#include <QTextCursor>
#include <QWidget>
#include <qstring.h>
#include <string.h>
#include <stdio.h>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void recv_data(void);
    void on_pushButton_OpenSerial_clicked();

    void on_pushButton_clicked();

    void on_Refsh_Serial_clicked();

    void on_comboBox_2_activated(const QString &arg1);

    void on_comboBox_COMx_currentIndexChanged(const QString &arg1);

    void on_pushButton_2_clicked();

protected:
    void timerEvent ( QTimerEvent *event );
private:
    void UI_COMBOBOX_CLEAR(char Flag);
    Ui::MainWindow *ui;
    QSerialPort *serial;


    QByteArray file_array;    //文件数据buf
    int file_length=0;         //文件字节长度
    QSerialPort mySerial;   //串口对象

    int sendedBytes = 0;//已发送字节
    int unSendedBytes = 0;  //未发送字节
    int terminal_recv_OK = 0;
    int receive_finish_Flag = 0;
    int currentLen = 0;
    int transfer_Started_flag = 0;

    //定时时间
    int TimeSerial = 0;
    int SerialOpenFlag = 0;
    int SetSerialTime = 8;
};
#endif // MAINWINDOW_H
