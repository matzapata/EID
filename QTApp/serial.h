#ifndef SERIAL_H
#define SERIAL_H


#include <QSerialPort>
#include <QTimer>
#include <QObject>

class Serial : public QObject
{
    Q_OBJECT
public:
    Serial();

    bool Connect(QString PortName, qint32 BaudRate = QSerialPort::Baud115200);
    bool Disconnect(void);
    bool SendBuffer(const char * buffer, int size);
    bool SendByte(const char byte);
    bool SendCommand(const char command);
    void Close(void);

private:
    void ReadData();
    void EmitBuffer(void);

    QSerialPort *m_serial = new QSerialPort;

    QTimer * RxTimer = new QTimer();
    QByteArray RxBuffer;

    QByteArray TxBuffer;
    unsigned int TxBufferSize;
    unsigned int TxBufferIndex;
    bool TxSendingFlag;

signals:
    void BufferReady(QByteArray buffer);
};

#endif // SERIAL_H
