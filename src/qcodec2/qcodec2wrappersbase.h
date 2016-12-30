#ifndef QCODEC2WRAPPERSBASE_H
#define QCODEC2WRAPPERSBASE_H

#include <climits>
#include <QtCore/QSharedPointer>
#include <QtCore/QIODevice>

#include <QtMultimedia/QAudioFormat>

#include "qcodec2shared.h"
#include "codec2.h"

class QCodec2BaseClass : public QIODevice, public QLastErrorHolder
{
    Q_OBJECT
public:
    QCodec2BaseClass(QIODevice *device, QObject *parent = Q_NULLPTR);

    virtual ~QCodec2BaseClass();

    void setCodecMode(const int codecMode);
    int codecMode() const;

    void setNaturalEncoding(const bool naturalEncoding);

    bool naturalEncoding() const;

    static QAudioFormat codec2AudioFormat();

    //! returns total frames count
    virtual qint64 framesCount() const;

    //! returns msecs count per single frame
    int msecsPerFrame() const;

    //! returns count of frames per second
    int framesPerSecond() const;

    //! return bytes per frame
    int bytesPerFrame() const;

    //! returns total msecs avail count
    virtual qint64 msecsAvail() const;
protected:
    int                 m_CodecMode = CODEC2_MODE_3200;
    bool                m_NaturalEncoding = true;
    bool                m_Initialized = false;

    CODEC2              *m_Codec = Q_NULLPTR;
    int                 m_SamplesPerFrame = 0;
    int                 m_BitsPerFrame = 0;
    short               *m_CodecBuffer = Q_NULLPTR;
    int                 m_BytesPerFrame = 0;
    unsigned char       *m_CodecsBitsBuffer = 0;
    int                 m_BufRemPtr = 0;
    QIODevice           *m_ExternalDevice = Q_NULLPTR;

    QSharedPointer<QCodec2VolumeLevelCalculator>    m_VolumeCalculator;

    virtual bool init();

    //! initializes codec2
    virtual bool initCodec2();

    //! closes all buffers and clears memory needful for codec2 allocated
    virtual void cleanUp();

    /*!
     * \brief payloadSize returns all payload size, must be re-implemented when
     * using non-raw files
     * \return payload bytes cout
     */
    virtual qint64 payloadSize() const;

signals:
    void updateVolume(const int currentVolume);
};

#endif //QCODEC2WRAPPERSBASE_H
