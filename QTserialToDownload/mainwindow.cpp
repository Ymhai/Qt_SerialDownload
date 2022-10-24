#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QDir>
char Rev_Flag = 0;

void MainWindow::UI_COMBOBOX_CLEAR(char Flag)
{
    QString path;
    QStringList fileNames;
    QFileInfo fileInfo;
    QFileInfoList list;
    std::string str;
    const char *tempstr;
    if(Flag == 0)path.sprintf("./烧录文件");
    else if(Flag == 1){
        str = ui->comboBox_2->currentText().toStdString();
        tempstr = str.c_str();
        path.sprintf("./烧录文件/%s",tempstr);
    }
    //图片文件
    else if(Flag == 2){
        path.sprintf("./图片文件");
    }
    QDir dir(path);
    if(Flag == 0){
        //获取filepath下的所有文件
        dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        //获取文件夹下所有文件(文件夹+文件)
        list = dir.entryInfoList();
        if(list.size() == 0)
        {
            ui->comboBox_2->clear();
            return;
        }
        //遍历
        for(int i = 0; i < list.size(); i++)
        {
            fileInfo = list.at(i);
            fileNames.append(fileInfo.baseName());//保存全部文件名
        }
        ui->comboBox_2->clear();
        ui->comboBox_2->addItems(fileNames);
    }
    else if(Flag == 1){
        //获取filepath下的所有文件
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        //获取文件夹下所有文件(文件夹+文件)
        list = dir.entryInfoList();
        if(list.size() == 0)
        {
            ui->comboBox_Picture->clear();
            return;
        }

        //遍历
        for(int i = 0; i < list.size(); i++)
        {
            fileInfo = list.at(i);
            if(fileInfo.suffix() == "bin")//设定后缀
            {
                fileNames.append(fileInfo.baseName());//保存全部文件名
            }
        }
            ui->comboBox_Picture->clear();
            ui->comboBox_Picture->addItems(fileNames);
    }
    else if(Flag == 2){
        //获取filepath下的所有文件
        dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        //获取文件夹下所有文件(文件夹+文件)
        list = dir.entryInfoList();
        if(list.size() == 0)
        {
            ui->comboBox_3->clear();
            return;
        }

        //遍历
        for(int i = 0; i < list.size(); i++)
        {
            fileInfo = list.at(i);
            if(fileInfo.suffix() == "bin")//设定后缀
            {
                fileNames.append(fileInfo.baseName());//保存全部文件名
            }
        }
            ui->comboBox_3->clear();
            ui->comboBox_3->addItems(fileNames);
    }

}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //添加所有串口到comboBox里
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        ui->comboBox_COMx->addItem((info.portName()));
    }
    serial = new QSerialPort(this);
    //开启一个1s的定时器
    startTimer(1000);
    //获取对应路径下的文件名
    UI_COMBOBOX_CLEAR(0);
    UI_COMBOBOX_CLEAR(1);
    UI_COMBOBOX_CLEAR(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_pushButton_OpenSerial_clicked()
{
    if(ui->pushButton_OpenSerial->text() == tr("打开串口")){
        ui->pushButton_OpenSerial->setText(tr("关闭串口"));
        //设置串口号
        serial->setPortName(ui->comboBox_COMx->currentText());
        if(serial->open(QIODevice::ReadWrite)){
            qDebug()<<"Open OK"<<endl;
            //设置串口参数 固定为921600
            serial->setBaudRate((ui->comboBox->currentText().toInt()));
            serial->setDataBits(QSerialPort::Data8);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setParity(QSerialPort::EvenParity);
            connect(serial,SIGNAL(readyRead()),this,SLOT(recv_data()));
        }
        else {
            qDebug()<<"Open Failed"<<endl;
        }
    }
    else if(ui->pushButton_OpenSerial->text() == tr("关闭串口")){
        ui->pushButton_OpenSerial->setText(tr("打开串口"));
        serial->close();
    }
    TimeSerial = 0;
    SerialOpenFlag = 0;
}




void MainWindow::on_Refsh_Serial_clicked()
{
    QStringList strList_port;
    for(int i = 0; i < QSerialPortInfo::availablePorts().size(); i++ ){
        strList_port.append(QSerialPortInfo::availablePorts().at(i).portName());
    }
    ui->comboBox_COMx->clear();
    ui->comboBox_COMx->addItems(strList_port);
}


void MainWindow::on_comboBox_2_activated(const QString &arg1)
{
    QString path;
    QStringList fileNames;
    std::string str = ui->comboBox_2->currentText().toStdString();
    const char *tempstr = str.c_str();
    path.sprintf("./烧录文件/%s",tempstr);
    QDir dir(path);
    //获取filepath下的所有文件
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    //获取文件夹下所有文件(文件夹+文件)
    QFileInfoList list = dir.entryInfoList();
    if(list.size() == 0)
    {
        ui->comboBox_Picture->clear();
        return;
    }

    //遍历
    for(int i = 0; i < list.size(); i++)
    {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.suffix() == "bin")//设定后缀
        {
            fileNames.append(fileInfo.baseName());//保存全部文件名
        }
    }
        ui->comboBox_Picture->clear();
        ui->comboBox_Picture->addItems(fileNames);
}

void MainWindow::on_comboBox_COMx_currentIndexChanged(const QString &arg1)
{
    if(ui->pushButton_OpenSerial->text() == tr("关闭串口")){
        serial->close();
        //设置串口号
            serial->setPortName(ui->comboBox_COMx->currentText());
        if(serial->open(QIODevice::ReadWrite)){
            serial->setBaudRate((ui->comboBox->currentText().toInt()));
            serial->setDataBits(QSerialPort::Data8);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setParity(QSerialPort::EvenParity);
            connect(serial,SIGNAL(readyRead()),this,SLOT(recv_data()));
        }
        else {
            qDebug()<<"Open Failed"<<endl;
        }
    }
}


