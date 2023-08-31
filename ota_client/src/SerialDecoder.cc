#include "SerialDecoder.h"
#include <mutex>
#include <qglobal.h>
#include <qmutex.h>
#include <qobject.h>
#include <qserialport.h>

namespace ota_client {

SerialDecoder::SerialDecoder(QObject* paerent)
    : QObject(paerent)
{
    m_port = new QSerialPort(this);
}

SerialDecoder::~SerialDecoder()
{
    if (m_port == nullptr) {
        return;
    }

    reset();
    delete m_port;
    m_port = nullptr;
}

void SerialDecoder::setPort(const QString& portName)
{
    m_portName = portName;
}

void SerialDecoder::setBaudRate(qint32 baudrate)
{
    m_baudrate = baudrate;
}

bool SerialDecoder::open()
{
    if (isOpen()) {
        return true;
    }
    m_port->setPortName(m_portName);
    m_port->setBaudRate(m_baudrate);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    bool ret = m_port->open(QIODevice::ReadWrite);
    if (ret) {
        connect(m_port, &QSerialPort::readyRead, this, &SerialDecoder::readData);
    }

    return ret;
}

void SerialDecoder::close()
{
    if (isOpen()) {
        m_port->close();
    }
}

bool SerialDecoder::isOpen() const
{
    return m_port->isOpen();
}

void SerialDecoder::readData()
{
    std::lock_guard<std::mutex> lock(m_mtx);
    m_recv.append(m_port->readAll());
}

void SerialDecoder::reset()
{
    if (isOpen()) {
        close();
        disconnect(m_port, &QSerialPort::readyRead, this, &SerialDecoder::readData);
    } 
}

}