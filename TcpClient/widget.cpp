#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include "tcpclient.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , client(new TCPClient)// 客户端
    , ui(new Ui::Widget)

{
    ui->setupUi(this);
    /* ... */
    QStringList datas;
    datas << "TCP Client" << "TCP Server";
    ui->comboBox_TcpMode->addItems(datas);
    /* 设置端口，打开端口 */
    connect(ui->pushButton_openPort,&QPushButton::clicked,[=]{
        if(ui->pushButton_openPort->text() == "打开"){
            ui->pushButton_openPort->setText("关闭");
            /* 打开端口 */
            client->setIP();
            client->setPort();
            client->open();
        }else{
            ui->pushButton_openPort->setText("打开");
            /* 关闭端口 */
            client->close();
        }
    });
    /* ... */

}

Widget::~Widget()
{
    delete ui;
}

