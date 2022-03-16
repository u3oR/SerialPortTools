#include "tcpclient.h"

#include <QTcpSocket>



TCPClient::TCPClient(){
    client = new QTcpSocket;
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
void TCPClient::open(){
    if(client){

    }
}
void TCPClient::sendData(){

}
void TCPClient::readData(readMode Mode){
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
