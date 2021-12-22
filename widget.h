#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

#include <QString>
#include <QByteArray>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QTextCodec>
#include <QApplication>
#include <QTime>
//#include <3rd_qextserialport/qextserialport.h>

QT_BEGIN_NAMESPACE

class QLabel;
//class QLineEdit;
//class QSpinBox;
class QPushButton;
class QComboBox;
class QTextEdit;
class QRadioButton;
class Run;
class QTextCodec;

QT_END_NAMESPACE


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    void refreshSerialPort();//刷新端口
    void openSerialPort();//打开端口
    void closeSerialPort();//关闭端口
    void reinfos();//接收数据 return infos
    void seinfos();//发送数据 send infos
    void updateStatus(QString Text);//更新数据
    void clrStatus();//清除状态栏数据
    void clrreinfos();
    void clrseinfos();
    void setSerialPort();
    void seinfostoHex();
    QString ChineseEnable(QByteArray source);
    QByteArray StrtoHex(QByteArray);
    QByteArray HextoStr(QByteArray);
private:
    QSerialPort *myserialport;//端口指针
private:
    QComboBox *serialPortComboBox;
    QPushButton *refreshButton;
    QPushButton *openButton;
    QPushButton *closeButton;
    QTextEdit *reinfostext;
    QTextEdit *edinfostext;
    QPushButton *seinfosbutton;
    QPushButton *reinfosbutton;
    QLabel *statusText;//当前状态栏
//    QPushButton *clrstatusbutton;//清空状态框
    QPushButton *clrsebutton;
    QPushButton *clrrebutton;
    QCheckBox *HexseCheck;
    QCheckBox *HexreCheck;
    /***************************/
    QComboBox *BaudrateBox;//波特率
    QComboBox *DatabitBox; //数据位
    QComboBox *StopbitBox; //停止位
    QComboBox *CheckDigitBox; //校验位

};
#endif // WIDGET_H

