#include "qcodec2encoder.h"

QCodec2Encoder::QCodec2Encoder(QIODevice *outputDevice, QObject *parent) :
    QCodec2BaseClass(outputDevice, parent)
{
    //
}

QCodec2Encoder::~QCodec2Encoder()
{
    //
}

qint64 QCodec2Encoder::readData(char *data, qint64 maxSize)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);

    return 0;
}

qint64 QCodec2Encoder::writeData(const char *data, qint64 maxSize)
{
    if(!m_Initialized) {
        this->setLastError(tr("Not initialized"));
        return -1;
    }

    //fill "buf" till we have enough to decode then write to file
    for(register int pBufferIndx=0; pBufferIndx<maxSize; )
    {
        int bytesToRead = qMin(
            ((qint64)(sizeof(short)*m_SamplesPerFrame-m_BufRemPtr)),
            maxSize-pBufferIndx
        );//how much can we read?

        if(bytesToRead != ((qint64)(sizeof(short)*m_SamplesPerFrame-m_BufRemPtr)))//not enough? then save for later
        {
            memcpy(m_CodecBuffer+m_BufRemPtr/sizeof(short), data+pBufferIndx,bytesToRead);
            m_BufRemPtr += bytesToRead;
            pBufferIndx += bytesToRead;
            break;
        }
        else {//decoding when have enought data
            memcpy (m_CodecBuffer+m_BufRemPtr/sizeof(short), data+pBufferIndx, bytesToRead);
            codec2_encode(m_Codec, m_CodecsBitsBuffer, m_CodecBuffer);

            //TODO: catch error!
            m_ExternalDevice->write(reinterpret_cast<char*>(m_CodecsBitsBuffer), m_BytesPerFrame);
            m_BufRemPtr=0;
        }

        pBufferIndx+=bytesToRead;
    }

    emit lengthUpdated(msecsAvail());

    m_VolumeCalculator->addData(data, maxSize);
    return maxSize;
}

bool QCodec2Encoder::start(const QIODevice::OpenMode additionalFlags)
{
    if(!open(QIODevice::WriteOnly | additionalFlags))
        return false;

    if(!m_Initialized) {
        if(!init())
            return false;
    }

    return true;
}

void QCodec2Encoder::stop()
{
    this->close();
    this->cleanUp();
}
