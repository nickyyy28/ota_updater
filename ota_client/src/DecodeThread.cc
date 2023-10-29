#include "DecodeThread.h"
#include "packet/Device2Client.h"
#include <QDebug>
#include <qdebug.h>
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
    while (1) {
        m_decoder->wait();
        qDebug() << "test.....";
        DATA_HEAD head;
        PacketBuffer buffer;
        bool ret = false;
        do {
            ret = m_decoder->decodeOnce(head, buffer);
            if (ret) {
                qDebug() << "msg id: " << head.msg_id << " sequence: " << head.SEQ;
            }
        } while (ret == true);
    }
}

}