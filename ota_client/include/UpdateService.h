#pragma once

#include <functional>

#include "packet/Device2Client.h"

namespace ota_client {

class UpdateService{
public:
    using onReceiveControlMessageCallback = std::function<void(const FRAME_HEAD& frame_head)>;
    using onReceiveDataMessageCallback = std::function<void(const FRAME_HEAD& frame_head)>;

    UpdateService() = default;
    ~UpdateService() = default;

private:
    
};

}