#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QTcpSocket>
class TCPClient{
    public:
        QTcpSocket *client;
        enum readMode{
            Hex,
            Ascii
        };
    public:
        void setIP();// 设置IP
        void setPort();// 设置端口
        void close();
        void open();
        void sendData();// 发送数据
        void readData(readMode);// 接收数据
    public:
        TCPClient();
        ~TCPClient();
};

#endif // TCPCLIENT_H
