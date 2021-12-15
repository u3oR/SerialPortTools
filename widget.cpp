#include "widget.h"
#include "run.h"
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>



Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , serialPortComboBox(new QComboBox())
    , refreshButton(new QPushButton(tr("刷新")))
    , openButton(new QPushButton(tr("打开串口")))
    , closeButton(new QPushButton(tr("关闭串口")))
    , reinfostext(new QTextEdit(tr("Seturn Infos")))
    , edinfostext(new QTextEdit(tr("Send Infos")))
    , seinfosbutton(new QPushButton(tr("发送")))
    , reinfosbutton(new QPushButton(tr("接收")))
    , statusText(new QTextEdit("Status:"))
//    , cledtextbutton(new QPushButton(tr("清除发送窗口")))
//    , clretextbutton(new QPushButton(tr("清除接收窗口")))
//    , (new QComboBox()) //波特率
//    , (new QComboBox()) //数据位
//    , (new QComboBox()) //停止位
//    , (new QComboBox()) //校验位


{
    qDebug() << "正在构建窗口..." ;
    this->setWindowTitle("Serial Port Tool");
    //固定窗口大小
    this->setMinimumSize(700,500);
    this->setMaximumSize(700,500);
    /*******************************************/
    auto mylayout = new QGridLayout;
    mylayout->addWidget(serialPortComboBox,1,0);
    mylayout->addWidget(refreshButton,1,1);
    mylayout->addWidget(openButton);
    mylayout->addWidget(closeButton);
    mylayout->addWidget(seinfosbutton);
    mylayout->addWidget(reinfosbutton);
    mylayout->addWidget(edinfostext,0,0);
    mylayout->addWidget(reinfostext,0,1);
    mylayout->addWidget(statusText);
    this->setLayout(mylayout);
    updateStatus("构建窗口完成") ;
    /*******************************************/

    refreshSerialPort();//首次刷新串口信息
    statusText->setReadOnly(true);
    closeButton->setEnabled(false);
    reinfosbutton->setEnabled(false);
    seinfosbutton->setEnabled(false);
    reinfostext->setReadOnly(true);

    myserialport = new QSerialPort();

    connect(refreshButton,&QPushButton::clicked,this,&Widget::refreshSerialPort);//刷新串口
    connect(openButton,&QPushButton::clicked,this,&Widget::openSerialPort);//打开端口
    connect(closeButton,&QPushButton::clicked,this,&Widget::closeSerialPort);//关闭打开的端口
    connect(reinfosbutton,&QPushButton::clicked,this,&Widget::reinfos);//关联<接收>按钮和<接收数据>函数
    connect(seinfosbutton,&QPushButton::clicked,this,&Widget::seinfos);//关联<发送>按钮和<发送数据>函数


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
    updateStatus("尝试获取串口列表...");

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo & info: infos) {
        serialPortComboBox->addItem(info.portName());
        qDebug()<<"正在加载串口"<<info.portName()<<"...";
    }
    updateStatus("获取完成");
}
//打开端口
void Widget::openSerialPort(){

    if(myserialport->isOpen()) {
        myserialport->clear();
        myserialport->close();
    }

    //问题2：myserialport仅设置了名字就能够代表串口？
    updateStatus("尝试打开串口...");
    myserialport->setPortName(serialPortComboBox->currentText());

    //https://doc.qt.io/qt-5/qserialport.html#open
    //open()使用OpenMode模式打开串口，成功则返回true,否则返回 false 并设置错误代码...
    if(myserialport->open(QIODevice::ReadWrite)){ //打开方式：读写
        qDebug()<<"打开串口"<<myserialport->portName()<<"成功";

        openButton->setEnabled(false);
        closeButton->setEnabled(true);
        reinfosbutton->setEnabled(true);
        seinfosbutton->setEnabled(true);

        //设置串口参数
        updateStatus("设置串口参数...");
        //
        setSerialPort();//设置串口参数函数
        myserialport->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率115200和读写方向
        myserialport->setDataBits(QSerialPort::Data8);//数据位
        myserialport->setStopBits(QSerialPort::OneStop);//停止位
        myserialport->setParity(QSerialPort::NoParity);//校验位

        updateStatus("设置串口参数完毕");
    }
    else{
        qDebug()<<"打开串口"<<myserialport->portName()<<"失败";
    }
}
//关闭端口
void Widget::closeSerialPort(){
    myserialport->close();

    openButton->setEnabled(true);
    closeButton->setEnabled(false);
    seinfosbutton->setEnabled(false);
    reinfosbutton->setEnabled(false);

}
//设置串口参数函数
void Widget::setSerialPort(){

}
//接收数据 return infos
void Widget::reinfos(){
    QString infos = myserialport->readAll();//读取端口数据
    //处理数据
    reinfostext->append(infos);//设置reinfoslabel文本

}
// 发送数据 send infos
void Widget::seinfos(){
    //    读取文本框数据
    QString readysendinfos = edinfostext->toPlainText();//读取编辑文本框的内容

    qDebug()<<"正在发送数据"<<readysendinfos;
    //处理数据
    //...

    //发送数据
    //write() 参考文档：https://blog.csdn.net/qq_16093323/article/details/79556807
    //qint64 QIODevice::write(const QByteArray &byteArray)
    //(QString).toLatin1方法的作用是将QString转换到QByteArray类型
    myserialport->write(readysendinfos.toLatin1());
    //...

}
//更新状态信息
void Widget::updateStatus(QString Text){
    statusText->append(Text);
}
