#include <QApplication>

#include <QPushButton>
#include <QCheckBox>
#include <QButtonGroup>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>
#include <QFormLayout>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>
#include <QByteArray>
#include <QTextCodec>
#include <QTime>
#include <QThread>
#include <QFile>
#include <QFileInfo>
#include "widget.h"
#include "serialthread.h"

//#include <3rd_qextserialport/qextserialport.h>
//#include <3rd_qextserialport/qextserialport_global.h>
//#include <3rd_qextserialport/qextserialport_p.h>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , count(0)
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
    , statusText(new QLabel("当前状态:"))
    , ClrseBtn(new QPushButton(tr("清空发送框")))
    , ClrreBtn(new QPushButton(tr("清空接收框")))
    , HexseCheck(new QCheckBox(tr("Send in Hex")))
    , HexreCheck(new QCheckBox(tr("Receive in Hex")))
//    , CodingLabel(new QLabel("UTF-8"))//编码格式


{
    buf = new QByteArray;
    initfile();
    this->setWindowTitle(tr("串口调试工具-简陋版ヾ(•ω•`)o"));
    this->setWindowIcon(QIcon(":/image/image/logo.png"));
    int width =580;double scale=0.7;
    this->setMinimumSize(width,(int)width*scale);//固定窗口大小
    this->setMaximumSize(width,(int)width*scale);
    /******************************/
    auto mylayout = new QGridLayout;
    /*********端口参数布局***********/
    auto portVBox = new QVBoxLayout;
    portVBox->addWidget(closeBtn);
    portVBox->addWidget(refreshBtn);
        auto portlayout = new QFormLayout;
        portlayout->addRow("(*^o^*)",COMcBox);
        portlayout->addRow("波特率",BaudrateBox);
        portlayout->addRow("数据位",DatabitBox);
        portlayout->addRow("停止位",StopbitBox);
        portlayout->addRow("校验位",CheckDigitBox);
    portVBox->addLayout(portlayout);
    portVBox->addWidget(openBtn);
    /**********发送接收控件布局***********/
//    auto CtrlVBox2 = new QVBoxLayout;
//    CtrlVBox2->addWidget(seinfosBtn);
    /***********控制按钮布局**********/
    auto CtrlVBox1 =new QVBoxLayout;
    CtrlVBox1->addWidget(seinfosBtn);
    CtrlVBox1->addWidget(HexreCheck);
    CtrlVBox1->addWidget(HexseCheck);
    CtrlVBox1->addWidget(ClrreBtn);
    CtrlVBox1->addWidget(ClrseBtn);
//    CodingLabel->setAlignment(Qt::AlignCenter);
    /**********主窗口布局***********/
    mylayout->addWidget(reinfostext,0,0,9,12);//接收框
    mylayout->addLayout(portVBox,0,12,9,4);//串口参数
    mylayout->addWidget(edinfostext,9,0,5,13);//编辑框
//    mylayout->addLayout(CtrlVBox2,9,10,7,3);//发送和接受按钮
    mylayout->addLayout(CtrlVBox1,9,13,7,3);//控制控件
    mylayout->addWidget(statusText,15,0,1,10);//状态栏
//    mylayout->addWidget(CodingLabel,15,13,1,3);
    /*****************************/

    this->setLayout(mylayout);
    /*******************************/
//    RefreshPort();//首次刷新串口信息
    COMcBox->clear();//清空列表信息
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo & info: infos) {
        COMcBox->addItem(info.portName());
    }
    UpdateStatus("更新串口列表完成");
    COMcBox->setCurrentIndex(1);
    SetPort();//初始化串口参数
    myserialport = new QSerialPort();
    closeBtn->setEnabled(false);
    reinfosBtn->setEnabled(false);
    seinfosBtn->setEnabled(false);
    reinfostext->setReadOnly(true);
    /*********************************************/
    connect(refreshBtn,&QPushButton::clicked,[=]{ //刷新串口
        COMcBox->clear();//清空列表信息
        const auto infos = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo & info: infos) {
            COMcBox->addItem(info.portName());
        }
        UpdateStatus("更新串口列表完成");
    });
    connect(openBtn,&QPushButton::clicked,this,&Widget::OpenPort);//打开端口
    connect(closeBtn,&QPushButton::clicked,this,&Widget::ClosePort);//关闭打开的端口
    connect(seinfosBtn,&QPushButton::clicked,this,&Widget::Seinfos);//关联<发送>按钮和<发送数据>函数
    connect(ClrreBtn,&QPushButton::clicked,[=]{reinfostext->clear();Widget::count=0;});//清空接收框
    connect(ClrseBtn,&QPushButton::clicked,[=]{edinfostext->clear();});//清空发送框
    connect(HexseCheck,&QCheckBox::stateChanged,this,&Widget::Hexseinfos);
    connect(HexreCheck,&QCheckBox::stateChanged,this,&Widget::Hexreinfos);
    connect(myserialport,&QSerialPort::readyRead,[=]{
        qDebug()<<"Readinfos计数君:"<<++count;
        Readinfos();
    });
}

// 读取串口数据
void Widget::Readinfos(){

        QByteArray array,
                    testarray;//debug用
        QString Str;//传入接收框的字符串
        array= myserialport->readAll();
        testarray=array;
        if(!HexreCheck->isChecked()){
//            arrayToUTF8(Str,array);//utf转码接收，可以防止乱码
            arraytoutf8(Str,array);//这个是我写的
            if(Str.right(1)=='\r')
                Str.chop(1);
            reinfostext->moveCursor(QTextCursor::End);//保证接收框的是只读的，不然会有麻烦
            reinfostext->insertPlainText(Str);
        }else {
            reinfostext->append(array.toHex(' ').toUpper());
        }
        /*注意！
         *  此处不能使用reinfostext->append(Str);
         *  "Appends a $new paragraph$ with text to the end of the text edit."
         *  因为每次append都会添加一个新@段落@，
         *  所以每次append时都会在前面都加一个'\n'，导致排版凌乱。
         *  所以使用insertPlainText()在后面追加文本，问题就迎刃而解
        */
        qDebug()
            <<"   arraySize:"<<testarray.size()
            <<"\n array:"<<testarray
            <<"\n arrayHex"<<testarray.toHex()
            <<"\n Str:"<<Str
            <<"\n Size:"<<Str.size();
//            <<"\nReinfostext_Str:"<<reinfostext->toPlainText();

        testarray.clear();
}


//https://www.ruanyifeng.com/blog/2007/10/ascii_unicode_and_utf-8.html
//关于utf8编码规则的博客。
void Widget::arraytoutf8(QString &string,QByteArray &array){

    int mark=0;
    bool iscut=false;
    buf->append(array);
    int endindex = buf->length()-1;//endindex是长度减一,那么buf.at(endindex)就正好是最后一个字节
    /* 如果m_asciiBuf最后的那个字节和1000 0000(0x80)相与后不是零，
     * 那么那个字节就该是1xxxxxxx，说明不是单字节字符，那就应该接着向前判断
     * 如果相与后是零，那它就是单字节字符，后面就可以断开
     * 0xxx xxxx -> OK
     */
    if(buf->at(endindex)&0x80){//如果最后一个字节是单字节，就不想用向下了
        for(int count=endindex;count>=0/*不能取到buf的负索引*/&&++mark<=3/*控制向前搜索的字节数*/;--count/*第一次count是倒数第二个索引,...*/){
            uint byte = buf->at(count);
            if(((mark<=1) && (byte&0xE0)==0xC0) ||   //0xE0=11100000 0xC0=11000000
               ((mark<=2) && (byte&0xF0)==0xE0) ||   //0xF0=11110000 0xE0=11100000
               ((mark<=3)/*这个条件其实根本没用，留着吧*/ && (byte&0xF8)==0xF0)   ){  //0xF8=11111000 0xF0=11110000
                iscut=true;
                break;
            }
        }
    }
    int pointer;
    if(iscut){
        pointer=endindex-mark+1;
    }else{
        pointer=endindex+1;
    }
    QByteArray cutarray = buf->mid(pointer,-1);/*-1表示到最后*///把后面不完整的几个字节留下啊
    buf->remove(pointer,mark);//把buf后面的字节删除
    string = QString::fromUtf8(*buf);//用string将所有正常的字节传出去
    buf->clear();//将buf清空后
    buf->append(cutarray);//再把残留的字节加回来
}

//窗口关闭
Widget::~Widget(){
    if (myserialport->isOpen()){
        myserialport->close();
    }
    delete myserialport;
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
    QByteArray infos = reinfostext->toPlainText().toUtf8();
    if(HexreCheck->isChecked()){
     reinfostext->setText(infos.toHex(' ').toUpper());
    //     reinfostext->setText(infos.toHex());
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


//接收数据
void Widget::Reinfos(){
    QByteArray infos = myserialport->readAll();//读取端口数据
    QString str = QString::fromUtf8(infos);
    //处理数据
    UpdateStatus("接收数据...");
    if(HexreCheck->isChecked()){
        reinfostext->setText(infos.toHex(' ').toUpper());//设置reinfoslabel文本 16Hex
    }else{
        reinfostext->append(str);//设置reinfoslabel文本 Str
    }
}

// 发送数据
void Widget::Seinfos(){
    QByteArray readysendinfos = edinfostext->toPlainText().toUtf8();//读取编辑文本框的内容

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
    QString currentTime =
            QDateTime::currentDateTime()
            .toString("[yyyy.MM.dd hh:mm:ss:zzz]");

    statusText->setText("*UTF-8* Status:"
                        +currentTime+Text);
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

//初始化文件
void Widget::initfile(){
    edfile.setFileName("./woode_ed.txt");
    edfile.open(QIODevice::ReadWrite);
//    edfile.
    refile.setFileName("./woode_re.txt");
    refile.open(QIODevice::ReadWrite);
}

//清空文件
void Widget::refileclear(){

    refile.close();
    refile.open(QFile::WriteOnly|QFile::Truncate);
    refile.close();
    refile.open(QIODevice::ReadWrite);
}


