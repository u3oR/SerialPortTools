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
//#include <3rd_qextserialport/qextserialport.h>
//#include <QRadioButton>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , serialPortComboBox(new QComboBox())
    , refreshButton(new QPushButton(tr("刷新串口")))
    , openButton(new QPushButton(tr("打开串口")))
    , closeButton(new QPushButton(tr("关闭串口")))
    , reinfostext(new QTextEdit(tr("接收框")))
    , edinfostext(new QTextEdit(tr("发送框")))
    , seinfosbutton(new QPushButton(tr("发送")))
    , reinfosbutton(new QPushButton(tr("接收")))
    , statusText(new QLabel("Status:"))
//    , clrstatusbutton(new QPushButton(tr("清空状态栏")))
    , clrsebutton(new QPushButton(tr("清空发送框")))
    , clrrebutton(new QPushButton(tr("清空接收框")))
    , HexseCheck(new QCheckBox(tr("以16进制发送")))
    , HexreCheck(new QCheckBox(tr("以16进制显示")))
    , BaudrateBox(new QComboBox()) //波特率
    , DatabitBox(new QComboBox()) //数据位
    , StopbitBox(new QComboBox()) //停止位
    , CheckDigitBox(new QComboBox()) //校验位

{
    //Qt如何设置界面风格？
    //https://www.zhihu.com/question/26241920
    //
    this->setWindowTitle("SerialPortTool");
    //固定窗口大小
    this->setMinimumSize(757,468);
    this->setMaximumSize(757,468);
    /*******************************************/
    auto mylayout = new QGridLayout;
    /*********端口参数布局***********/
    auto portVBox = new QVBoxLayout;
    auto portlayout = new QFormLayout;
    portVBox->addWidget(closeButton);
    portVBox->addWidget(refreshButton);
    portlayout->addRow("⚫串口",serialPortComboBox);
    portlayout->addRow("波特率",BaudrateBox);
    portlayout->addRow("数据位",DatabitBox);
    portlayout->addRow("停止位",StopbitBox);
    portlayout->addRow("校验位",CheckDigitBox);
    portVBox->addLayout(portlayout);
    portVBox->addWidget(openButton);
    /**********控制控件布局***********/
    auto CtrlVBox1 = new QVBoxLayout;
    CtrlVBox1->addWidget(reinfosbutton);
    CtrlVBox1->addWidget(seinfosbutton);
    /***********清除按钮布局**********/
    auto CtrlVBox2 =new QVBoxLayout;
//  CtrlVBox2->addWidget(clrstatusbutton);
    CtrlVBox2->addWidget(clrrebutton);
    CtrlVBox2->addWidget(clrsebutton);

    /**********主窗口布局***********/
    mylayout->addWidget(reinfostext,0,0,9,12);//接收框
    mylayout->addLayout(portVBox,0,12,9,4);//串口参数
    mylayout->addWidget(edinfostext,9,0,5,10);//编辑框
    mylayout->addLayout(CtrlVBox1,9,13,7,3);//发送和接受按钮
    mylayout->addLayout(CtrlVBox2,9,10,7,3);
    mylayout->addWidget(statusText,15,0,1,10);//状态栏


    /****************************/
//    mylayout->addWidget(HexseCheck);
//    mylayout->addWidget(HexreCheck);

    this->setLayout(mylayout);
//    updateStatus("构建窗口完成") ;
    /*******************************************/

    refreshSerialPort();//首次刷新串口信息
    setSerialPort();//初始化串口参数
//    statusText->setReadOnly(true);
    closeButton->setEnabled(false);
    reinfosbutton->setEnabled(false);
    seinfosbutton->setEnabled(false);
//    reinfostext->setReadOnly(true);

    myserialport = new QSerialPort();

    /*********************************************/
    connect(refreshButton,&QPushButton::clicked,this,&Widget::refreshSerialPort);//刷新串口
    connect(openButton,&QPushButton::clicked,this,&Widget::openSerialPort);//打开端口
    connect(closeButton,&QPushButton::clicked,this,&Widget::closeSerialPort);//关闭打开的端口
    connect(reinfosbutton,&QPushButton::clicked,this,&Widget::reinfos);//关联<接收>按钮和<接收数据>函数
    connect(seinfosbutton,&QPushButton::clicked,this,&Widget::seinfos);//关联<发送>按钮和<发送数据>函数
//    connect(clrstatusbutton,&QPushButton::clicked,this,&Widget::clrStatus);
    connect(clrrebutton,&QPushButton::clicked,this,&Widget::clrreinfos);
    connect(clrsebutton,&QPushButton::clicked,this,&Widget::clrseinfos);

}
Widget::~Widget(){
    //问题1：如果没有下面的语句，关闭程序后串口还会被占用吗
    if (myserialport->isOpen()){
        myserialport->close();
    }
    delete myserialport;
}

//刷新端口
void Widget::refreshSerialPort(){

    serialPortComboBox->clear();//清楚列表信息
//    updateStatus("获取串口列表...");

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo & info: infos) {
        serialPortComboBox->addItem(info.portName());
//        updateStatus("正在加载串口"+info.portName()+"...");
    }
    updateStatus("更新串口列表完成");
}
//打开端口
void Widget::openSerialPort(){
    if(myserialport->isOpen()) {
        myserialport->clear();
        myserialport->close();
    }
    //问题2：myserialport仅设置了名字就能够代表串口？
    updateStatus("打开串口\""+myserialport->portName()+"\"...");
    myserialport->setPortName(serialPortComboBox->currentText());

    //https://doc.qt.io/qt-5/qserialport.html#open
    //open()使用OpenMode模式打开串口，成功则返回true,否则返回 false 并设置错误代码...
    if(myserialport->open(QIODevice::ReadWrite)){ //打开方式：读写
        updateStatus("已打开"+myserialport->portName());

        openButton->setEnabled(false);
        closeButton->setEnabled(true);
        refreshButton->setEnabled(false);
        reinfosbutton->setEnabled(true);
        seinfosbutton->setEnabled(true);
        serialPortComboBox->setEnabled(false);
        BaudrateBox->setEnabled(false);
        DatabitBox->setEnabled(false);
        StopbitBox->setEnabled(false);
        CheckDigitBox->setEnabled(false);

        //设置串口参数
//        updateStatus("设置串口参数...");
        myserialport->setBaudRate((QSerialPort::BaudRate)BaudrateBox->currentText().toInt(),QSerialPort::AllDirections);
        myserialport->setDataBits((QSerialPort::DataBits)DatabitBox->currentText().toInt());//数据位
        myserialport->setStopBits((QSerialPort::StopBits)StopbitBox->currentIndex());//停止位
        myserialport->setParity((QSerialPort::Parity)CheckDigitBox->currentIndex());//校验位
        /*updateStatus("波特率\""+BaudrateBox->currentText()+"\"\n数据位\""+DatabitBox->currentText()+ \
                    "\"\n停止位\""+StopbitBox->currentText()+"\"\n检验位\""+CheckDigitBox->currentText()+"\"");*/
//        updateStatus("设置串口参数完毕");
    }
    else{
        updateStatus("打开串口"+myserialport->portName()+"失败");
    }
}
//关闭端口
void Widget::closeSerialPort(){
    myserialport->close();

    openButton->setEnabled(true);
    closeButton->setEnabled(false);
    seinfosbutton->setEnabled(false);
    reinfosbutton->setEnabled(false);
    refreshButton->setEnabled(true);

    serialPortComboBox->setEnabled(true);
    BaudrateBox->setEnabled(true);
    DatabitBox->setEnabled(true);
    StopbitBox->setEnabled(true);
    CheckDigitBox->setEnabled(true);
    updateStatus("Colsed");

}
//设置串口参数函数
 void Widget::setSerialPort(){
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
//接收数据 return infos
void Widget::reinfos(){
    QString infos = ChineseEnable(myserialport->readAll());//读取端口数据
    //处理数据
    updateStatus("接收数据...");
    if(HexreCheck->isChecked()){
        //qstring需要先转成qbytearray之后才能再转成16进制
        reinfostext->append(infos.toLatin1().toHex());//设置reinfoslabel文本 16Hex
    }else{
        reinfostext->append(infos);//设置reinfoslabel文本 Str
    }

}
// 发送数据 send infos
void Widget::seinfos(){
    QString readysendinfos = edinfostext->toPlainText();//读取编辑文本框的内容
    updateStatus("发送数据...");
//    updateStatus("发送数据\""+readysendinfos+"\"");
    //处理数据
    //...
    //发送数据
    //write() 参考文档：https://blog.csdn.net/qq_16093323/article/details/79556807
    //qint64 QIODevice::write(const QByteArray &byteArray)
    //(QString).toLatin1方法的作用是将QString转换到QByteArray类型
    if(HexseCheck->isChecked()){
        myserialport->write(readysendinfos.toLatin1().toHex());
    }else{
        myserialport->write(readysendinfos.toLatin1());
    }
}
//更新状态信息
void Widget::updateStatus(QString Text){
    QString currentTime = QDateTime::currentDateTime().toString("[yyyy.MM.dd hh:mm:ss:zzz]");
    statusText->setText("Status:"+currentTime+Text);
}
//清空状态信息
/*void Widget::clrStatus(){
    statusText->clear();
    statusText->setText("Status:");
}*/
void Widget::clrseinfos(){
    edinfostext->clear();
    edinfostext->append("发送框");
}
void Widget::clrreinfos(){
    reinfostext->clear();
    reinfostext->append("接收框");
}

QByteArray Widget::StrtoHex(QByteArray str){
    return str.toHex();
}
QByteArray Widget::HextoStr(QByteArray hex){
    QByteArray str = QByteArray::fromHex(hex);
    return str;
}

QString Widget::ChineseEnable(QByteArray source){
    QString str;
    QTextCodec *coding = QTextCodec::codecForName("GBK");
    str =coding->toUnicode(source);
    return str;
    //
    //QByteArray显示中文
    //https://blog.csdn.net/xiaoyangger/article/details/5450348
    //
    //QTextCodec *tc = QTextCodec::codecForName("GBK");
    //QByteArray ba = file.readAll();
    //str = tc->toUnicode(ba);
    //cout<<str<<endl;
    //
}

