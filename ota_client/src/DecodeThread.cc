#include "DecodeThread.h"

namespace ota_client {

DecodeThread::DecodeThread(QObject* parent, SerialDecoder::ptr decoder)
    : QThread(parent)
    , m_decoder(decoder)
{

}

DecodeThread::~DecodeThread()
{

}

void DecodeThread::run()
{
    
}

}