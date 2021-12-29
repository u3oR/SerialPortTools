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
    void RefreshPort();//刷新端口
    void OpenPort();//打开端口
    void ClosePort();//关闭端口
    void Readinfos();
    void Reinfos();//手动接收数据 return infos
    void Seinfos();//发送数据 send infos
    void UpdateStatus(QString Text);//更新数据
    void ClrStatus();//清除状态栏数据
    void Clrreinfos();
    void Clrseinfos();
    void SetPort();
    void Hexseinfos();
    void Hexreinfos();
    void Hexinfos(QCheckBox *Check,QTextEdit *Text);
    QByteArray SetCoding(QByteArray source);
    QByteArray StrtoHex(QByteArray);
    QByteArray HextoStr(QByteArray);
private:
    QSerialPort *myserialport;//端口指针


private:
    QComboBox *COMcBox;
    QComboBox *BaudrateBox;//波特率
    QComboBox *DatabitBox; //数据位
    QComboBox *StopbitBox; //停止位
    QComboBox *CheckDigitBox; //校验位

    QPushButton *refreshBtn;
    QPushButton *openBtn;
    QPushButton *closeBtn;

    QTextEdit *reinfostext;
    QTextEdit *edinfostext;
    QPushButton *seinfosBtn;
    QPushButton *reinfosBtn;
    QLabel *statusText;//当前状态栏
    QPushButton *ClrseBtn;
    QPushButton *ClrreBtn;

    QCheckBox *HexseCheck;
    QCheckBox *HexreCheck;

//    QLabel *CodingLabel;
    /**********************/


};
#endif // WIDGET_H

