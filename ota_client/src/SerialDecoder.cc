#include "SerialDecoder.h"
#include "packet/Device2Client.h"
#include <mutex>
#include <qglobal.h>
#include <qmutex.h>
#include <qobject.h>
#include <qserialport.h>
#include <QDateTime>
#include <stdint.h>
#include <cstring>

#include "bsp_crc.h"

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

void SerialDecoder::wait()
{
    m_sem.wait();
}

void SerialDecoder::post()
{
    m_sem.post();
}

bool SerialDecoder::decodeOnce(DATA_HEAD& head, PacketBuffer& buffer)
{
    std::lock_guard<std::mutex> lock(m_mtx);

    qint64 cur_time = QDateTime::currentMSecsSinceEpoch();

    while (size() >= sizeof(FRAME_HEAD) + sizeof(DATA_HEAD) + sizeof(DATA_TAIL)) {
        uint8_t head1 = m_recv.at(0);
        uint8_t head2 = m_recv.at(1);
        if (head1 != 0xAB && head2 != 0xCD) {
            m_recv.remove(0, 1);
            continue;
        }
        if (size() <= sizeof(FRAME_HEAD) + sizeof(DATA_HEAD) + sizeof(DATA_TAIL)) {
            //Data not receive complete
            return false;
        }
        FRAME_HEAD frame_head;
        frame_head.header1 = head1;
        frame_head.header2 = head2;
        frame_head.transport_type = m_recv.at(2);
        frame_head.data_length = *(uint32_t*)(m_recv.begin() + 4);
        frame_head.crc8 = m_recv.at(5);

        uint8_t head_crc8 = CRC8_calculate((uint8_t*)&frame_head, sizeof(frame_head), 0xFF);
        if (head_crc8 != frame_head.crc8) {
            //head crc8 error
            m_recv.remove(0, sizeof(frame_head));
            continue;
        }

        if (size() <= sizeof(FRAME_HEAD) + frame_head.data_length) {
            //Data not receive complete
            return false;
        }

        head.SEQ = *(uint16_t*)(m_recv.begin() + sizeof(frame_head));
        head.msg_id = *(uint16_t*)(m_recv.begin() + sizeof(frame_head) + 2);

        uint32_t body_length = frame_head.data_length - sizeof(DATA_HEAD) - sizeof(DATA_TAIL);
        std::memcpy((void*)&buffer, (m_recv.begin() + sizeof(FRAME_HEAD) + sizeof(DATA_HEAD)), body_length);

        DATA_TAIL tail;
        tail.tail1 = m_recv.at(sizeof(FRAME_HEAD) + sizeof(DATA_HEAD) + body_length);
        tail.tail1 = m_recv.at(sizeof(FRAME_HEAD) + sizeof(DATA_HEAD) + body_length + 1);
        tail.crc16 = *(uint16_t*)(m_recv.begin() + sizeof(FRAME_HEAD) + sizeof(DATA_HEAD) + body_length + 2);

        uint16_t data_crc16 = CRC16_calculate((uint8_t*)&frame_head, sizeof(frame_head), 0xFFFF);
        data_crc16 = CRC16_calculate((uint8_t*)&head, sizeof(head), data_crc16);
        data_crc16 = CRC16_calculate((uint8_t*)&buffer, body_length, data_crc16);
        data_crc16 = CRC16_calculate((uint8_t*)&tail, 2, data_crc16);
        if (data_crc16 == tail.crc16 && tail.tail1 == 0xEC && tail.tail2 == 0xED) {
            //Decode Success
            m_recv.remove(0, sizeof(FRAME_HEAD) + frame_head.data_length);
            return true;
        } else {
            m_recv.remove(0, sizeof(FRAME_HEAD) + frame_head.data_length);
            continue;
        }
    }

    return false;
}

int SerialDecoder::size()
{
    return m_recv.size();
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
    post();
}

void SerialDecoder::testPost()
{
    post();
}

void SerialDecoder::reset()
{
    if (isOpen()) {
        close();
        disconnect(m_port, &QSerialPort::readyRead, this, &SerialDecoder::readData);
    } 
}

void SerialDecoder::Semaphore::post()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    if (!flag) {
        flag = true;
        m_cond.notify_one();
    }
}

void SerialDecoder::Semaphore::post_all()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    if (!flag) {
        flag = true;
        m_cond.notify_all();
    }
}

void SerialDecoder::Semaphore::wait()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    while (flag == false) {
        m_cond.wait(lock);
    }
    flag = false;
}

}