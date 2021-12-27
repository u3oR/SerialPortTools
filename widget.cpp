#include "widget.h"
#include "run.h"
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>
#include <QFormLayout>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>
#include <QByteArray>
#include <QCheckBox>
#include <QButtonGroup>
#include <QTextCodec>
#include <QApplication>
#include <QTime>
#include <QThread>
//#include <3rd_qextserialport/qextserialport.h>
//#include <QRadioButton>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , COMcBox(new QComboBox())
    , BaudrateBox(new QComboBox()) //波特率
    , DatabitBox(new QComboBox()) //数据位
    , StopbitBox(new QComboBox()) //停止位
    , CheckDigitBox(new QComboBox()) //校验位
    , refreshBtn(new QPushButton(tr("刷新串口")))
    , openBtn(new QPushButton(tr("打开串口")))
    , closeBtn(new QPushButton(tr("关闭串口")))
    , reinfostext(new QTextEdit(tr("")))
    , edinfostext(new QTextEdit(tr("")))
    , seinfosBtn(new QPushButton(tr("发送")))
    , reinfosBtn(new QPushButton(tr("接收")))
    , statusText(new QLabel("Status:"))
    , ClrseBtn(new QPushButton(tr("清空发送框")))
    , ClrreBtn(new QPushButton(tr("清空接收框")))
    , HexseCheck(new QCheckBox(tr("以16进制发送")))
    , HexreCheck(new QCheckBox(tr("以16进制显示接收")))
//    , CodingLabel(new QLabel("UTF-8"))//编码格式


{
    this->setWindowTitle("SerialPortTool");
    this->setMinimumSize(600,380);//固定窗口大小
    this->setMaximumSize(600,380);
    /******************************/
    auto mylayout = new QGridLayout;
    /*********端口参数布局***********/
    auto portVBox = new QVBoxLayout;
    portVBox->addWidget(closeBtn);
    portVBox->addWidget(refreshBtn);
        auto portlayout = new QFormLayout;
        portlayout->addRow("⚫串口",COMcBox);
        portlayout->addRow("波特率",BaudrateBox);
        portlayout->addRow("数据位",DatabitBox);
        portlayout->addRow("停止位",StopbitBox);
        portlayout->addRow("校验位",CheckDigitBox);
    portVBox->addLayout(portlayout);
    portVBox->addWidget(openBtn);
    /**********发送接收控件布局***********/
    auto CtrlVBox2 = new QVBoxLayout;
    CtrlVBox2->addWidget(reinfosBtn);
    CtrlVBox2->addWidget(seinfosBtn);
    /***********控制按钮布局**********/
    auto CtrlVBox1 =new QVBoxLayout;
    CtrlVBox1->addWidget(HexreCheck);
    CtrlVBox1->addWidget(HexseCheck);
    CtrlVBox1->addWidget(ClrreBtn);
    CtrlVBox1->addWidget(ClrseBtn);
//    CodingLabel->setAlignment(Qt::AlignCenter);
    /**********主窗口布局***********/
    mylayout->addWidget(reinfostext,0,0,9,12);//接收框
    mylayout->addLayout(portVBox,0,12,9,4);//串口参数
    mylayout->addWidget(edinfostext,9,0,5,10);//编辑框
    mylayout->addLayout(CtrlVBox2,9,10,7,3);//发送和接受按钮
    mylayout->addLayout(CtrlVBox1,9,13,7,3);//控制控件
    mylayout->addWidget(statusText,15,0,1,10);//状态栏
//    mylayout->addWidget(CodingLabel,15,13,1,3);
    /*****************************/

    this->setLayout(mylayout);
    RefreshPort();//首次刷新串口信息
    SetPort();//初始化串口参数
    myserialport = new QSerialPort();
    closeBtn->setEnabled(false);
    reinfosBtn->setEnabled(false);
    seinfosBtn->setEnabled(false);
//    QTextCodec *coding = QTextCodec::codecForName("UTF-8");
    /*********************************************/
    connect(refreshBtn,&QPushButton::clicked,this,&Widget::RefreshPort);//刷新串口
    connect(openBtn,&QPushButton::clicked,this,&Widget::OpenPort);//打开端口
    connect(closeBtn,&QPushButton::clicked,this,&Widget::ClosePort);//关闭打开的端口
//    connect(reinfosBtn,&QPushButton::clicked,this,&Widget::Reinfos);//关联<接收>按钮和<接收数据>函数
    connect(seinfosBtn,&QPushButton::clicked,this,&Widget::Seinfos);//关联<发送>按钮和<发送数据>函数
    connect(ClrreBtn,&QPushButton::clicked,[=]{reinfostext->clear();});//清空接收框
    connect(ClrseBtn,&QPushButton::clicked,[=]{edinfostext->clear();});//清空发送框
    connect(HexseCheck,&QCheckBox::stateChanged,this,&Widget::Hexseinfos);
    connect(HexreCheck,&QCheckBox::stateChanged,this,&Widget::Hexreinfos);

    connect(myserialport,&QSerialPort::readyRead,[=]{
        QString readyReadinfos = myserialport->readAll();
        QByteArray infosarray = readyReadinfos.toUtf8();

        reinfostext->append(infosarray);
    });

/*  读取串口数据
    QString strinfos = myserialport->readAll();//读取端口数据
    QByteArray infos = strinfos.toUtf8();

        if(readyReadinfos.right(1).compare("\n")==0){
            readyReadinfos.chop(1);
        }
        if(myserialport->readAll().isEmpty()){
            reinfostext->append(readyReadinfos);
            readyReadinfos="";
        }


        while(myserialport->waitForReadyRead(10)){
            infos += myserialport->readAll();
            if(infos.at(infos.length()-1)==' '){
                infos.chop(1);
            }
        }
*/


    //connect(HexreCheck,&QCheckBox::stateChanged,this,&Widget::Hexinfos(HexreCheck,reinfostext));
}
//QString strinfos = myserialport->readAll();//读取端口数据
//QByteArray infos = strinfos.toUtf8();
//窗口关闭
Widget::~Widget(){
    //问题1：如果没有下面的语句，关闭程序后串口还会被占用吗
    if (myserialport->isOpen()){
        myserialport->close();
    }
    delete myserialport;
}
//刷新端口
void Widget::RefreshPort(){
    COMcBox->clear();//清空列表信息
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo & info: infos) {
        COMcBox->addItem(info.portName());
    }
    UpdateStatus("更新串口列表完成");
}
//打开端口
void Widget::OpenPort(){
    if(myserialport->isOpen()){
        myserialport->clear();
        myserialport->close();
    }
    myserialport->setPortName(COMcBox->currentText());
    //https://doc.qt.io/qt-5/qserialport.html#open
    //open()使用OpenMode模式打开串口，成功则返回true,否则返回 false 并设置错误代码...
    if(myserialport->open(QIODevice::ReadWrite)){ //打开方式：读写
        openBtn->setEnabled(false);
        closeBtn->setEnabled(true);
        refreshBtn->setEnabled(false);
        reinfosBtn->setEnabled(true);
        seinfosBtn->setEnabled(true);
        COMcBox->setEnabled(false);
        BaudrateBox->setEnabled(false);
        DatabitBox->setEnabled(false);
        StopbitBox->setEnabled(false);
        CheckDigitBox->setEnabled(false);
        //设置串口参数
        //updateStatus("设置串口参数...");
        myserialport->setBaudRate((QSerialPort::BaudRate)BaudrateBox->currentText().toInt(),QSerialPort::AllDirections);
        myserialport->setDataBits((QSerialPort::DataBits)DatabitBox->currentText().toInt());//数据位
        myserialport->setStopBits((QSerialPort::StopBits)StopbitBox->currentIndex());//停止位
        myserialport->setParity((QSerialPort::Parity)CheckDigitBox->currentIndex());//校验位
        UpdateStatus("已打开"+myserialport->portName());
    }
    else{
        UpdateStatus("打开串口"+myserialport->portName()+"失败");
    }
}
//关闭端口
void Widget::ClosePort(){
    myserialport->close();

    openBtn->setEnabled(true);
    closeBtn->setEnabled(false);
    seinfosBtn->setEnabled(false);
    reinfosBtn->setEnabled(false);
    refreshBtn->setEnabled(true);

    COMcBox->setEnabled(true);
    BaudrateBox->setEnabled(true);
    DatabitBox->setEnabled(true);
    StopbitBox->setEnabled(true);
    CheckDigitBox->setEnabled(true);
    UpdateStatus("Colsed");

}
//设置串口参数函数
 void Widget::SetPort(){
    QStringList datas ;
    datas <<"110"<<"300"<<"600"<<"1200"<<"2488"<<"4800"<<"9600"<<"14400"
         <<"38400"<<"43000"<<"57600"<<"76800"<<"115200"<<"128000"<<"230400"
        <<"256000"<<"460800"<<"921600";
    BaudrateBox->addItems(datas);
    datas.clear();
    datas<<"5"<<"6"<<"7"<<"8";
    DatabitBox->addItems(datas);
    datas.clear();
    datas<<" "<<"1"<<"2"<<"1.5";
    StopbitBox->addItems(datas);
    datas.clear();
    datas<<""<<"无"<<"奇"<<"偶"<<"空格";
    CheckDigitBox->addItems(datas);
    datas.clear();
    //默认值
    BaudrateBox->setCurrentText("115200");
    DatabitBox->setCurrentText("8");
    StopbitBox->setCurrentText("1");
    CheckDigitBox->setCurrentText("无");
}
//接收框的16进制转换
void Widget::Hexreinfos(){
 QString strinfos = reinfostext->toPlainText();
 QByteArray infos = strinfos.toUtf8();
 if(HexreCheck->isChecked()){
     reinfostext->setText(infos.toHex(' ').toUpper());
 }else{
     //把接收框的16进制数据转成普通文本
     reinfostext->setText(QByteArray::fromHex(infos));
 }
}
//发送框的16进制转换
void Widget::Hexseinfos(){
 QString strinfos = edinfostext->toPlainText();
 QByteArray infos = strinfos.toUtf8();
 if(HexseCheck->isChecked()){
     edinfostext->setText(infos.toHex(' ').toUpper());
 }else{
     //把编辑框的16进制数据转成普通文本
     edinfostext->setText(QByteArray::fromHex(infos));
 }
}
/*
void Widget::Hexinfos(QCheckBox *Check,QTextEdit *Text){
    QByteArray infos = Text->toPlainText().toLatin1();
    if(Check->isChecked()){
        Text->setText(infos.toHex(' ').toUpper());
    }else{
        Text->setText(QByteArray::fromHex(infos));
    }
}
*/
//接收数据
void Widget::Reinfos(){
    QString strinfos = myserialport->readAll();//读取端口数据
    QByteArray infos = strinfos.toUtf8();
    //处理数据
    UpdateStatus("接收数据...");
    if(HexreCheck->isChecked()){
        reinfostext->setText(infos.toHex(' ').toUpper());//设置reinfoslabel文本 16Hex
    }else{
        reinfostext->setText(infos);//设置reinfoslabel文本 Str
    }
}
// 发送数据
void Widget::Seinfos(){
    QString strinfos = edinfostext->toPlainText();
    QByteArray readysendinfos = strinfos.toUtf8();//读取编辑文本框的内容

    UpdateStatus("发送数据...");
    if(HexseCheck->isChecked()){
        myserialport->write(QByteArray::fromHex(readysendinfos));
    }else{
        myserialport->write(readysendinfos);
    }
    //处理数据
    //...
    //发送数据
    //write() 参考文档：https://blog.csdn.net/qq_16093323/article/details/79556807
    //qint64 QIODevice::write(const QByteArray &byteArray)
    //(QString).toLatin1方法的作用是将QString转换到QByteArray类型
}
//更新状态信息
void Widget::UpdateStatus(QString Text){
    QString currentTime = QDateTime::currentDateTime().toString("[yyyy.MM.dd hh:mm:ss:zzz]");
    statusText->setText("*UTF-8* Status:"+currentTime+Text);
}
//Chinese
QByteArray Widget::SetCoding(QByteArray source){

    QByteArray str;
    QTextCodec *coding = QTextCodec::codecForName("UTF-8");
    str = coding->toUnicode(source).toLatin1();
    return str;
    //QByteArray显示中文
    //https://blog.csdn.net/xiaoyangger/article/details/5450348
    //QTextCodec *tc = QTextCodec::codecForName("GBK");
    //QByteArray ba = file.readAll();
    //str = tc->toUnicode(ba);
    //cout<<str<<endl;
}


