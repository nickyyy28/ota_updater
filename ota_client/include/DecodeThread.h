#pragma once

#include <QThread>
#include <qobject.h>
#include <qthread.h>

#include <mutex>

#include "SerialDecoder.h"


namespace ota_client {

class DecodeThread : public QThread{
public:
    explicit DecodeThread(QObject* parent, SerialDecoder::ptr decoder);
    ~DecodeThread();


protected:
    void run() override;

private:
    SerialDecoder::ptr m_decoder;
};

}