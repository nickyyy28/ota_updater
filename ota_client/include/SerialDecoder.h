#pragma once

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QIODevice>
#include <QByteArray>
#include <mutex>

#include <atomic>
#include "packet/Device2Client.h"

#include <condition_variable>

#include <memory>
#include <qglobal.h>
#include <qobject.h>
#include <qserialport.h>
#include <stdint.h>

namespace ota_client {

class SerialDecoder : public QObject{
public:
    using ptr = std::shared_ptr<SerialDecoder>;
    explicit SerialDecoder(QObject *parent);
    ~SerialDecoder();

    void setPort(const QString& portName);
    void setBaudRate(qint32 baudrate);

    bool open();
    void close();

    int size();

    bool isOpen() const;

    bool decodeOnce(DATA_HEAD& head, PacketBuffer& buffer);

    void post();
    void wait();

private:
    /**
     * @brief reset serial
     * 
     */
    void reset();

    class Semaphore{
    public:
        Semaphore() : flag(false){}
        ~Semaphore() = default;

        void post();
        void post_all();
        void wait();
    private:
        std::mutex m_mtx;
        std::condition_variable m_cond;
        bool flag;
    };

private slots:
    void readData();

public slots:
    void testPost();

private:
    QString m_portName;
    qint32 m_baudrate;

    QSerialPort *m_port;
    QByteArray m_recv;
    std::mutex m_mtx;

    Semaphore m_sem;
};

}