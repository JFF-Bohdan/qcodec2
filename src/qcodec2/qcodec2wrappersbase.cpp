#include "qcodec2wrappersbase.h"

// **************************************
// ********** QCodec2BaseClass **********
// **************************************
QCodec2BaseClass::QCodec2BaseClass(QIODevice *device, QObject *parent):
    QIODevice(parent),
    m_ExternalDevice(device)
{
    m_VolumeCalculator.reset(new QCodec2VolumeLevelCalculator(this));

    connect
    (
        m_VolumeCalculator.data(),
        &QCodec2VolumeLevelCalculator::updateVolume,
        this,
        &QCodec2BaseClass::updateVolume
    );
}

QCodec2BaseClass::~QCodec2BaseClass()
{
    cleanUp();
}

void QCodec2BaseClass::setCodecMode(const int codecMode)
{
    m_CodecMode = codecMode;
}

int QCodec2BaseClass::codecMode() const
{
    return m_CodecMode;
}

void QCodec2BaseClass::setNaturalEncoding(const bool naturalEncoding)
{
    m_NaturalEncoding = naturalEncoding;
}

bool QCodec2BaseClass::naturalEncoding() const
{
    return m_NaturalEncoding;
}

QAudioFormat QCodec2BaseClass::codec2AudioFormat()
{
    QAudioFormat    format;

    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec(QLatin1Literal("audio/pcm"));
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    return format;
}

qint64 QCodec2BaseClass::framesCount() const
{
    return (payloadSize() / m_BytesPerFrame);
}

int QCodec2BaseClass::framesPerSecond() const
{
    return (8000 / m_SamplesPerFrame);
}

int QCodec2BaseClass::bytesPerFrame() const
{
    return m_BytesPerFrame;
}

qint64 QCodec2BaseClass::msecsAvail() const
{
    return (framesCount() * msecsPerFrame());
}

int QCodec2BaseClass::msecsPerFrame() const
{
    return (1000 / framesPerSecond());
}

bool QCodec2BaseClass::init()
{
    if(m_Initialized)
        return true;

    if(!initCodec2())
        return false;

    m_Initialized = true;
    return true;
}

bool QCodec2BaseClass::initCodec2()
{
    m_Codec              = codec2_create(m_CodecMode);
    m_SamplesPerFrame    = codec2_samples_per_frame(m_Codec);
    m_BitsPerFrame       = codec2_bits_per_frame(m_Codec);

    m_CodecBuffer        = (short*)malloc(m_SamplesPerFrame*sizeof(short));
    m_BytesPerFrame      = (m_BitsPerFrame + 7) / 8;
    m_CodecsBitsBuffer   = (unsigned char*)malloc(m_BytesPerFrame*sizeof(char));

    int iNaturalEncoding =  m_NaturalEncoding ? 1 : 0;

    codec2_set_natural_or_gray(m_Codec, !iNaturalEncoding);

    return true;
}

void QCodec2BaseClass::cleanUp()
{
    if(m_Codec) {
        codec2_destroy(m_Codec);
        m_Codec = Q_NULLPTR;
    }

    if(m_CodecBuffer) {
        free(m_CodecBuffer);
        m_CodecBuffer = Q_NULLPTR;
    }

    if(m_CodecsBitsBuffer) {
        free(m_CodecsBitsBuffer);
        m_CodecsBitsBuffer = Q_NULLPTR;
    }
}

qint64 QCodec2BaseClass::payloadSize() const
{
    return m_ExternalDevice->size();
}
