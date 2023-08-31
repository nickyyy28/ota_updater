#pragma once

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QIODevice>
#include <QByteArray>
#include <mutex>

#include <memory>
#include <qglobal.h>
#include <qobject.h>
#include <qserialport.h>

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

    bool isOpen() const;

private:
    /**
     * @brief reset serial
     * 
     */
    void reset();

private slots:
    void readData();

private:
    QString m_portName;
    qint32 m_baudrate;

    QSerialPort *m_port;
    QByteArray m_recv;
    std::mutex m_mtx;
};

}