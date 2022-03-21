#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QTcpSocket>

class TCPClient{
    public:
        QTcpSocket *client;
        enum codingMode{
            Hex = 1,
            Ascii = 0
        };
    public:
        void setIP();// 设置IP
        void setPort();// 设置端口
        void close();
        void open();
        void sendData(QByteArray data,codingMode Mode);// 发送数据
        void readData(codingMode Mode);// 接收数据
    public:
        TCPClient();
        ~TCPClient();
};

#endif // TCPCLIENT_H
