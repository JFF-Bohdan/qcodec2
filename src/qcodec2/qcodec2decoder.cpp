#include "qcodec2decoder.h"

QCodec2Decoder::QCodec2Decoder(QIODevice *inputDevice, QObject *parent):
    QCodec2BaseClass(inputDevice, parent)
{
    //
}

QCodec2Decoder::~QCodec2Decoder()
{
    //
}

qint64 QCodec2Decoder::posInMsecs() const
{
    return ((totalBytesRead() / m_BytesPerFrame) * msecsPerFrame());
}

qint64 QCodec2Decoder::framesCount() const
{
    if(m_TotalFramesCount != -1)
        return m_TotalFramesCount;

    m_TotalFramesCount = QCodec2BaseClass::framesCount();
    return m_TotalFramesCount;
}

qint64 QCodec2Decoder::readData(char *data, qint64 maxSize)
{
    int buffPtrIndex=0;

    //(len/2) rather than (len) allows smaller sound card buffer
    while((buffPtrIndex+sizeof(short)*m_SamplesPerFrame)<(maxSize/2))
    {
        qint64 bytesread=m_ExternalDevice->read((char*)m_CodecsBitsBuffer, m_BytesPerFrame);

        if(m_ExternalDevice->size()) {
            qint64  currentFrameNumber = (totalBytesRead() / m_BytesPerFrame);
            emit updatePlayingProgress
            (
                100*currentFrameNumber/framesCount(),
                currentFrameNumber,
                framesCount()
            );
        }

        if(bytesread == m_BytesPerFrame)
        {
            codec2_decode(m_Codec, m_CodecBuffer, m_CodecsBitsBuffer);
            memcpy (data + buffPtrIndex, m_CodecBuffer, sizeof(short)*m_SamplesPerFrame);
        }
        else
        {
            buffPtrIndex=0;
            break;
        }
        buffPtrIndex += sizeof(short)*m_SamplesPerFrame;
    }

    m_VolumeCalculator->addData(data, buffPtrIndex);
    return buffPtrIndex;
}

qint64 QCodec2Decoder::writeData(const char *data, qint64 maxSize)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);

    return 0;
}

qint64 QCodec2Decoder::totalBytesRead() const
{
    return m_ExternalDevice->pos();
}

bool QCodec2Decoder::start()
{
    if(!m_Initialized) {
        if(!init())
            return false;
    }

    return this->open(QIODevice::ReadOnly);
}

void QCodec2Decoder::stop()
{
    this->close();
    this->cleanUp();
}

