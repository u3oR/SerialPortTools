## Qt Official example

这里面放的Qt官方串口例程，加了注释，作为学习

## SerialTool Github 项目

https://github.com/Skiars/SerialTool

转串口数据转utf8部分参考了这个Github项目(发现了宝藏)

不过因为安装Qt时有些库没有安装，不能实时编译，只能硬看下去

对我来说这个项目大，内容多，不好理解。

下面是串口接收数据部分，转码只copy了转UTF8

//接收数据部分


```c++
//接收串口数据
/* 
    setManager关联了readyRead信号和controller::readPortData槽
    同样是readyRead信号关联readPortData，没有使用waitforreadyRead等。
    controller::readPortData接收串口数据(QbyteArray)，调用receiveData处理
    controller::receiveData调用viewmanager::receiveData
    viewmanager::receiveData调用texttrview::receiveData
    最后由texttrview::receiveData处理数据
*/
void TextTRView::receiveData(const QByteArray &array/*串口数据*/)
{
    QString string //array转换后的字符串
    QString pre;//pre是字符串前缀(回车)
    if (ui->portReadAscii->isChecked()) {//ascii显示
        //定义:int m_hexCount(十六进制数)
        if (m_hexCount > 0) {
            pre = '\n';
        }
        m_hexCount = -1;
        arrayToString(string, array);//转成相应编码的字符串
    } else {
        if (m_hexCount == -1) {
            m_hexCount = 0;
            pre = '\n';
        }
        arrayToHex(string, array, 16);//转成16进制的字符串
    }
    
    if (m_timeStamp) {//应该是判断是否加上时间戳
        appendTimeStamp(string);
    } else {
        ui->textEditRx->append(pre + string);
    }
}
```

// array转UTF8
```c++
QByteArray *m_asciiBuf;
m_asciiBuf = new Qbytearray;
//m_asciiBuf必须再外部定义，因为转码完毕后m_asciiBuf里面可能还要保存部分字符

void TextTRView::arrayToUTF8(QString &str/*返回的字符串指针*/
                             ,const QByteArray &array/*串口数据数组*/)
{
    int lastIndex, cut = 0;
    bool isCut = false;
    //m_asciiBuf是头文件中定义的Qbytearray型指针
    // QByteArray *m_asciiBuf;
    m_asciiBuf->append(array);
    lastIndex = m_asciiBuf->length() - 1;
    if (m_asciiBuf->at(lastIndex) & 0x80) { // 0xxx xxxx -> OK
        // UTF8最大编码为4字节，因此向前搜寻三字节
        for (int i = lastIndex; i >= 0 && ++cut < 4; --i) {
            uint8_t byte = uint8_t(m_asciiBuf->at(i));
            if (((cut < 2) && (byte & 0xE0) == 0xC0) ||
                ((cut < 3) && (byte & 0xF0) == 0xE0) ||
                (byte & 0xF8) == 0xF0) {
                isCut = true;
                break;
            }
        }
    }
    lastIndex -= isCut ? cut - 1 : -1;
    QByteArray cutArray = m_asciiBuf->mid(lastIndex);
    m_asciiBuf->remove(lastIndex, cut);
    QTextCodec *code = QTextCodec::codecForName(m_codecName);
    str = code->toUnicode(*m_asciiBuf);
    m_asciiBuf->clear();
    m_asciiBuf->append(cutArray);
}
```



## ...

以上⏳