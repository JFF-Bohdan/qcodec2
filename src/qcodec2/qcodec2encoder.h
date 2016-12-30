#ifndef QCODEC2ENCODER_H
#define QCODEC2ENCODER_H

#include <QtCore/QIODevice>

#include "qcodec2wrappersbase.h"

class QCodec2Encoder : public QCodec2BaseClass
{
    Q_OBJECT
public:
    /*!
     * \brief QCodec2Encoder constructor for encoder
     * \param outputDevice pointer to output device
     * \param parent
     */
    QCodec2Encoder(QIODevice *outputDevice, QObject *parent = Q_NULLPTR);

    virtual ~QCodec2Encoder() Q_DECL_OVERRIDE;
protected:
    //! just stub
    virtual qint64 readData(char *data, qint64 maxSize);

    //! encodes and writes encoded data to output device
    virtual qint64 writeData(const char *data, qint64 maxSize);

public slots:

    //! must be called before encoding started
    bool start(const QIODevice::OpenMode additionalFlags = 0);

    //! must be called when no more encoding needed
    void stop();
signals:

    //! will be emitted when written length changed
    void lengthUpdated(const qint64 msecsAvail);
};


#endif //QCODEC2ENCODER_H
