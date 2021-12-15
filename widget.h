#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QString>

QT_BEGIN_NAMESPACE

class QLabel;
//class QLineEdit;
//class QSpinBox;
class QPushButton;
class QComboBox;
class QTextEdit;

QT_END_NAMESPACE


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    void refresh_SerialPort();//刷新端口
    void openSerialPort();//打开端口
    void closeSerialPort();//关闭端口
    void reinfos();//接收数据 return infos
    void seinfos();//发送数据 send infos
    void updateStatus(QString Text);//更新数据
    void setSerialPort();
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
    QTextEdit *statusText;//当前状态栏
//, cledtextbutton(new QPushButton(tr("清除发送窗口")))
//, clretextbutton(new QPushButton(tr("清除接收窗口")))

};
#endif // WIDGET_H

