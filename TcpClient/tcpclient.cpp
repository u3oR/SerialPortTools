#include "tcpclient.h"

#include <QTcpSocket>
#include <QByteArray>



TCPClient::TCPClient(){
    client = new QTcpSocket;
    client->abort();
}
void TCPClient::open(){

    client->connectToHost("172.23.128.1",8080);
}
void TCPClient::setIP(){

}
void TCPClient::setPort(){

}
void TCPClient::close(){
    if(client){
        client->close();
    }
}
//接收数据
/*void Widget::Reinfos(){
    QByteArray infos = myserialport->readAll();//读取端口数据
    QString str = QString::fromUtf8(infos);
    //处理数据
    UpdateStatus("接收数据...");
    if(HexreCheck->isChecked()){
        reinfostext->setText(infos.toHex(' ').toUpper());//设置reinfoslabel文本 16Hex
    }else{
        reinfostext->append(str);//设置reinfoslabel文本 Str
    }
}*/


void TCPClient::sendData(QByteArray data,codingMode Mode){
    /* hex OR ascii */
    if(Mode == Hex){
        client->write(QByteArray::fromHex(data));
    }else{
        client->write(data);
    }
}
void TCPClient::readData(codingMode Mode){
    /* hex OR ascii */
    if(Mode == Hex){

    }else{

    }
}

TCPClient::~TCPClient(){
    if(client){
        delete client;
    }
}
