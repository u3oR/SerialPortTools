#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
// #include "tcpclient.h"
#include <QByteArray>
#include <QTcpSocket>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , client(new TCPClient)// 客户端
    , ui(new Ui::Widget)


{
    ui->setupUi(this); // 初始化UI
    QTcpSocket *client = new QTcpSocket;
    client->abort();
    ui->comboBox_TcpMode->setEnabled(false);
    /* 参数设置 */
    /*
    QStringList datas;
    datas << "TCP Client" << "TCP Server";
    ui->comboBox_TcpMode->addItems(datas);
    */

    /* 设置端口，打开端口 */
    connect(ui->pushButton_openPort,&QPushButton::clicked,[=]{
        if(ui->pushButton_openPort->text() == "打开"){
            ui->pushButton_openPort->setText("关闭");
            /* 端口参数组件不可设置 */
            ui->comboBox_IP->setEnabled(false);
            // ui->comboBox_TcpMode->setEnabled(false);
            ui->comboBox_Port->setEnabled(false);
            /* 打开端口 */
            client->connectToHost(ui->comboBox_IP->currentText(),ui->comboBox_Port->currentText().toInt());
        }else{
            ui->pushButton_openPort->setText("打开");
            /* 端口参数组件不可设置 */
            ui->comboBox_IP->setEnabled(true);
            // ui->comboBox_TcpMode->setEnabled(true);
            ui->comboBox_Port->setEnabled(true);
            /* 关闭端口 */
            client->close();
        }
    });

    /* 发送数据，将Send_textEdit组件中的内容发送出去 */
    connect(ui->SendButton,&QPushButton::clicked,[=]{
        QByteArray readysendinfos = ui->Send_textEdit->toPlainText().toUtf8();
        if(ui->radioButton_Send_HEX->isChecked()){
            client->write(QByteArray::fromHex(readysendinfos));
        }else{
            client->write(readysendinfos);
        }
    });

    /* 接收数据，自动接收数据并显示到textBrowser组件中 */
    connect(client,&QTcpSocket::readyRead,[=]{
        QByteArray infos = client->readAll();
        QString str = QString::fromUtf8(infos);
        if(ui->radioButton_Read_HEX->isChecked()){
            ui->textBrowser->append(infos.toHex(' ').toUpper());
        }else{
            ui->textBrowser->append(str);
        }
    });
    /*  */
    /* ... */
    /* ... */
    /* ... */
    /* ... */

}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    ui->textBrowser->clear();
}
