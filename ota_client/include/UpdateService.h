#pragma once

#include <functional>
#include <qserialport.h>

#include "packet/Device2Client.h"

namespace ota_client {

class UpdateService{
public:
    UpdateService() = default;
    ~UpdateService() = default;

    void onReceiveControlMessage(const DATA_HEAD& data_head, void* data_body, QSerialPort* serial);
    void onReceiveDataMessage(const DATA_HEAD& data_head, void* data_body, QSerialPort* serial);
private:

};

}