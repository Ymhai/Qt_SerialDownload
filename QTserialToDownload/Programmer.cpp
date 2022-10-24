#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <stdio.h>
#include <QThread>
#include <QWaitCondition>
#include <QTime>


int  address = 0x08000000;
long int filelength = 0;
char  first= 0;
char Usart_Flag = 0;

char ClearSectorSize = 6;
char Picture_Flag = 0;
void Sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
void MainWindow::timerEvent ( QTimerEvent *event ) {
    QMessageBox title;
    if(SerialOpenFlag)
    {
        TimeSerial++;
        if(TimeSerial>SetSerialTime)
        {
            TimeSerial = 0;
            SerialOpenFlag = 0;
            title.critical(this,"错误操作","操作超时请重新开始",1,0);
            //数据初始化
            address = 0x08000000;
            filelength = 0;
            first= 0;
            Usart_Flag = 0xff;
        }
    }
}
//烧写开始
void MainWindow::on_pushButton_clicked()
{
    const char usart_buff[1] = {0x7f};
    QMessageBox title;
    if(ui->pushButton_OpenSerial->text() == tr("打开串口"))
    {
        title.critical(this,"错误操作","请先打开串口",1,0);
        return;
    }
    SerialOpenFlag = 1;//开始标志 定时器使用
    ui->textEdit_ReceiveMsg->clear();
    ui->textEdit_ReceiveMsg->append("芯片连接中......");
    std::string str = ui->comboBox_Picture->currentText().toStdString();
    const char *tempstr = str.c_str();
    std::string str1 = ui->comboBox_2->currentText().toStdString();
    const char *tempstr1 = str1.c_str();
    //打开文件
    QString fileName;

    //读取对应文件
     fileName.sprintf("./烧录文件/%s/%s.bin",tempstr1,tempstr);
     qDebug()<<fileName<<endl;
     QFile file1(fileName);
     if(!file1.open(QIODevice::ReadOnly))  //读取文件
     {
         ui->textEdit_ReceiveMsg->append("读取文件失败！！你查看是否存在对应文件");
         return;
     }
     else {
         file_array=file1.readAll();//读取文件
         ui->textEdit_ReceiveMsg->append("文件名：");
         ui->textEdit_ReceiveMsg->append(fileName);
         file_length=file_array.size();//计算长度
         ui->textEdit_ReceiveMsg->append(QString("文件大小：%1 Bytes").arg(file_length));
     }
    //发送启动指令
    serial->write(usart_buff,1);

    sendedBytes = 0;
    unSendedBytes = file_length;
    ui->progressBar->setValue(static_cast<int>((static_cast<float>(sendedBytes) / static_cast<float>(file_length)) * 100));

    //数据初始化
    address = 0x08000000;
    filelength = 0;
    first= 0;
    Usart_Flag = 0;
    ClearSectorSize = 6;
    TimeSerial = 0;
    file1.close();

//    ui->pushButton->setEnabled(true);

}


void MainWindow::on_pushButton_2_clicked()
{
    const char usart_buff[1] = {0x7f};
    QMessageBox title;
    if(ui->pushButton_OpenSerial->text() == tr("打开串口"))
    {
        title.critical(this,"错误操作","请先打开串口",1,0);
        return;
    }
    SerialOpenFlag = 1;//开始标志
    ui->textEdit_ReceiveMsg->clear();
    ui->textEdit_ReceiveMsg->append("芯片连接中......");
    //打开文件
    QString fileName;

    //读取对应文件
     fileName.sprintf("./烧录文件/WritePicture.bin");
     qDebug()<<fileName<<endl;
     QFile file1(fileName);
     if(!file1.open(QIODevice::ReadOnly))  //读取文件
     {
         ui->textEdit_ReceiveMsg->append("读取文件失败！！你查看是否存在对应文件");
         return;
     }
     else {
         file_array.clear();
         file_array=file1.readAll();//读取文件
         ui->textEdit_ReceiveMsg->append("文件名：");
         ui->textEdit_ReceiveMsg->append(fileName);
         file_length=file_array.size();//计算长度
         ui->textEdit_ReceiveMsg->append(QString("文件大小：%1 Bytes").arg(file_length));
     }
    //发送启动指令
    serial->write(usart_buff,1);

    sendedBytes = 0;
    unSendedBytes = file_length;
    ui->progressBar->setValue(static_cast<int>((static_cast<float>(sendedBytes) / static_cast<float>(file_length)) * 100));

    //数据初始化
    address = 0x08000000;
    filelength = 0;
    first= 0;
    Usart_Flag = 0;
    ClearSectorSize = 1;
    Picture_Flag = 1;

    TimeSerial = 0;
    file1.close();
}





//数据接收
void MainWindow::recv_data(void)
{
    QWaitCondition wait;

    static char R_Flag = 0;

    char usart_buff[20] = {0};
    int ArraySize = 128;
    unsigned char Count = 0;
//    serial->waitForReadyRead(5);
    QByteArray buf = serial->readAll();
    if(!buf.isEmpty())
    {
        //如果字符串相同三次及以上发送传输成功标志
        if(  buf.at(0)== 0x79 )
        {
            TimeSerial = 0;
            switch (Usart_Flag)
            {
                case 0:
                        Usart_Flag = 1;//烧录开始标志
                        ui->textEdit_ReceiveMsg->append("芯片连接成功");
                        //获取芯片ID
                        usart_buff[0] = 0x02;usart_buff[1] = 0xFD;
                        serial->write(usart_buff,2);
                break;
                case 1:
                        Usart_Flag = 2;//获取ID结束
                        sprintf(usart_buff,"ID:%x%x",buf.at(2),buf.at(3));
                        ui->textEdit_ReceiveMsg->append(usart_buff);
                        //开始擦除 因为烧录的局限性只需要擦除6个扇区
                        usart_buff[0] = 0x44;usart_buff[1] = 0xBB;
                        ui->textEdit_ReceiveMsg->append("开始擦除程序");
                        serial->write(usart_buff,2);//擦除
                break;
                case 2:
                        usart_buff[0] = 0x00;usart_buff[1] = ClearSectorSize;
                        Usart_Flag = 3;//开始循环编程
                        for(int i=0;i<=ClearSectorSize;i++){
                            usart_buff[2+i*2] = 0x00;usart_buff[3+i*2] = i;
                        }
                        for( int i=0; i<(ClearSectorSize) ; i++ )
                        {
                            usart_buff[(ClearSectorSize+1)*2+2]=usart_buff[(ClearSectorSize+1)*2+2]^i;
                        }
                        serial->write(usart_buff,((ClearSectorSize+1)*2+2+1));//擦除
                        ui->textEdit_ReceiveMsg->append("等待1-6s");
                break;
                case 3:
                        if(first == 0)
                        {
                            first = 1;
                            ui->textEdit_ReceiveMsg->append("开始烧录！");
                        }
                        Usart_Flag = 4;//开始循环编程
                        usart_buff[0] = 0x31;usart_buff[1] = 0xCE;
                        serial->write(usart_buff,2);//开始编程
                break;
                case 4:
                        Usart_Flag = 5;//开始循环编程
                        usart_buff[0] = (address>>24)&0xff;
                        usart_buff[1] = (address>>16)&0xff;
                        usart_buff[2] = (address>>8)&0xff;
                        usart_buff[3] = (address)&0xff;
                        usart_buff[4]=   usart_buff[0]^usart_buff[1]^usart_buff[2]^usart_buff[3];//校验和
                        address += 0x80;
                        serial->write(usart_buff,5);//开始编程
                break;
                case 5:
                        Usart_Flag = 3;//开始循环编程
                        if((file_length-filelength)>128)ArraySize = 128;
                        else {
                            ArraySize = file_length-filelength;
                        }

                        usart_buff[0] = ArraySize-1;
                        serial->write(usart_buff,1);//开始标志

                        Count = file_array.at(filelength);
                        for(int i = 0 ; i <usart_buff[0]; i++ )
                        {
                            Count =  Count^(file_array.at(filelength+i+1));
                        }
                        Count =  Count^(ArraySize-1);
                        usart_buff[0] = Count;
                        //发送数据 文件管理
                        serial->write(file_array.mid(filelength),ArraySize);
                        filelength +=ArraySize;//每次128个字节

                        ui->progressBar->setValue(static_cast<int>((static_cast<float>(filelength) / static_cast<float>(file_length)) * 100));

                        serial->write(usart_buff,1);//开始编程
                        if(filelength>=file_length)//烧录完毕则完成
                        {
                            Usart_Flag = 6;
                        }
                    break;
                case 6:
                    Usart_Flag = 7;
                    usart_buff[0] = 0x21;usart_buff[1] = 0xDE;
                    serial->write(usart_buff,2);
                   break;
                 case 7:
                    Usart_Flag = 0xff;
                    address = 0x08000000;
                    TimeSerial = 0;
                    SerialOpenFlag = 0;

                    usart_buff[0] = (address>>24)&0xff;
                    usart_buff[1] = (address>>16)&0xff;
                    usart_buff[2] = (address>>8)&0xff;
                    usart_buff[3] = (address)&0xff;
                    usart_buff[4]=   usart_buff[0]^usart_buff[1]^usart_buff[2]^usart_buff[3];//校验和

                    serial->write(usart_buff,5);//跳转至地址头
                    ui->textEdit_ReceiveMsg->append("烧录完毕");

                    if(Picture_Flag)
                    {
                        QMessageBox::information(this,"烧写图片","已经进入仪器烧写模式请勿拔插仪器!");
                        Picture_Flag = 0;
                        serial->close();
                        //设置串口号
                            serial->setPortName(ui->comboBox_COMx->currentText());
                        if(serial->open(QIODevice::ReadWrite)){
                            serial->setBaudRate(921600);
                            serial->setDataBits(QSerialPort::Data8);
                            serial->setFlowControl(QSerialPort::NoFlowControl);
                            serial->setStopBits(QSerialPort::OneStop);
                            serial->setParity(QSerialPort::NoParity);
                            connect(serial,SIGNAL(readyRead()),this,SLOT(recv_data()));
                        }
                        else {
                            qDebug()<<"Open Failed"<<endl;
                        }

                        ui->textEdit_ReceiveMsg->append("已经进入仪器烧写模式请勿拔插仪器!\n");
                        QString fileName;

                        std::string str = ui->comboBox_3->currentText().toStdString();
                        const char *tempstr = str.c_str();
                        //读取对应文件
                        fileName.sprintf("./图片文件/%s.bin",tempstr);

                        QFile file2(fileName);
                        if(!file2.open(QIODevice::ReadOnly))  //读取文件
                            QMessageBox::information(this,"警告栏","读取文件失败！！");
                        else {
                            file_array.clear();
                            file_array=file2.readAll();//读取文件
                            ui->textEdit_ReceiveMsg->append("文件名：");
                            ui->textEdit_ReceiveMsg->append(fileName);
                            file_length=file_array.size();//计算长度
                            ui->textEdit_ReceiveMsg->append(QString("文件大小：%1 Bytes").arg(file_length));
                            serial->write(file_array);
                        }
                        file2.close();
                        R_Flag = 2;
                    }
                    break;

            }


        }
        else if(tr(buf)=="ZZDSERXC"&&R_Flag!=3)// 如果获取到发送文件结束的符号 则发送提示框
        {
             QMessageBox::information(this,"文件发送成功","文件发送成功");
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
                 serial->clear();
             }
             else {
                 qDebug()<<"Open Failed"<<endl;
             }
             R_Flag = 3;
        }
        else if(R_Flag==2)
        {
            QString str = ui->textEdit_ReceiveMsg->toPlainText();
            str+=tr(buf);
            ui->textEdit_ReceiveMsg->clear();
            ui->textEdit_ReceiveMsg->append(str);
        }
        else if(  buf.at(0)== 0x1f )
        {

        }
    }
}
