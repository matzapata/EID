#include "serial.h"
#include <QDebug>
#include <QSerialPort>

Serial::Serial(){
    TxSendingFlag = false;
}

bool Serial::Connect(QString PortName, qint32 BaudRate) {
    m_serial->setPortName(PortName);
    m_serial->setBaudRate(BaudRate);
    if (m_serial->open(QIODevice::ReadWrite)) {
        // qDebug() << "CONECTADOS";
        connect(m_serial, &QSerialPort::readyRead, this, &Serial::ReadData);
        connect(RxTimer, &QTimer::timeout, this, &Serial::EmitBuffer);
        return true;
    } else {
        // qDebug() << "Error de conexion";
        return false;
    }

}

void Serial::ReadData()
{
    const QByteArray data = m_serial->readAll();
    // qDebug() << "Leemos: " << QString(data);
    if (TxSendingFlag) {
        if (data.toInt() == 0 && TxBufferIndex < TxBufferSize-1) {
            // Recibimos confirmacion de lo que enviamos y todavia no terminamos de enviar
            SendByte(TxBuffer.at(TxBufferIndex));
            TxBufferIndex ++;
        }
        else {
            // Terminamos de enviar o no recibimos confirmacion
            // qDebug() << "Index: " << TxBufferIndex << " Char: " << TxBuffer.at(TxBufferIndex);
            SendByte(TxBuffer.at(TxBufferIndex));
            TxBuffer.clear();
            TxBufferSize = 0;
            TxBufferIndex = 0;
            TxSendingFlag = false;
        }
    }
    else {
        // Usamos timer para asegurarnos que recibimos toda la data antes de procesarla
        RxTimer->stop();
        RxBuffer.append(data);
        RxTimer->start(500);
    }
}

void Serial::EmitBuffer(void)
{
    RxTimer->stop();
    emit BufferReady(RxBuffer);
    RxBuffer.clear();
}

bool Serial::SendBuffer(const char * buffer, int size) {
    if (TxSendingFlag) return false;

    TxBuffer.clear();
    TxBuffer = QByteArray::fromRawData(buffer, size);
    TxSendingFlag = true;
    TxBufferSize = TxBuffer.size();

    // qDebug() << TxBufferSize;

    //Enviamos el primer byte
    SendByte(TxBuffer.at(0));
    TxBufferIndex = 1;

    return true;
}
bool Serial::SendByte(const char byte) {

    char byte_c[1] = {byte};
    QByteArray byte_a(byte_c, 1);
    m_serial->write(byte_a);

    //    qDebug() << "Enviamos: " << byte;

    return true;
}

bool Serial::SendCommand(const char command) {
    SendByte('#');
    SendByte(command);

    return true;
}

void Serial::Close(void) {
    m_serial->close();
}
