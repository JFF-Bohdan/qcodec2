#ifndef QCODEC2DECODER_H
#define QCODEC2DECODER_H

#include <QtCore/QIODevice>
#include "qcodec2wrappersbase.h"

class QCodec2Decoder : public QCodec2BaseClass
{
    Q_OBJECT
public:

    /*!
     * \brief QCodec2Decoder constructor for decoder
     * \param inputDevice pointer for device contains encoded data
     * \param parent
     */
    QCodec2Decoder(QIODevice *inputDevice, QObject *parent = Q_NULLPTR);

    virtual ~QCodec2Decoder() Q_DECL_OVERRIDE;

    //! returns current position in msecs
    qint64 posInMsecs() const;

    //! returns total frames count
    virtual qint64 framesCount() const Q_DECL_OVERRIDE;

protected:
    //! reads and decods data on-the-fly
    virtual qint64 readData(char *data, qint64 maxSize);

    //! just stub, no real implementation
    virtual qint64 writeData(const char *data, qint64 maxSize);

    /*!
     * \brief totalBytesRead returns total bytes read count. This implementation
     * can be used when reading raw codec2 files. When you read data from container
     * file you will need to re-implement this method
     * \return bytes read count
     */
    virtual qint64 totalBytesRead() const;

private:
    mutable qint64  m_TotalFramesCount = -1;

public slots:
    //! must be called before decoding
    bool start();

    //! must be called to stop decoding
    void stop();

signals:
    /*!
     * \brief updatePlayingProgress will be emitted when need update for playing progress update
     * \param currentPerc current progress in percents
     * \param currentFrame current frame number
     * \param totalFramesCount total frames count
     */
    void updatePlayingProgress
    (
        const int       currentPerc,
        const qint64    currentFrame,
        const qint64    totalFramesCount
    );
};

#endif //QCODEC2DECORER_H
