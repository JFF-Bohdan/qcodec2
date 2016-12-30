#include "qcodec2shared.h"

// **************************************************
// ********** QCodec2VolumeLevelCalculator **********
// **************************************************

QCodec2VolumeLevelCalculator::QCodec2VolumeLevelCalculator(QObject *parent):
    QObject(parent)
{
    //
}

QCodec2VolumeLevelCalculator::~QCodec2VolumeLevelCalculator()
{
    //
}

void QCodec2VolumeLevelCalculator::addData(const char *data, qint64 maxSize)
{
    const short     *ptr = reinterpret_cast<const short *>(data);
    int             maxval=0;
    int             maxLen = maxSize/sizeof(short);

    for(register int i=0; i<maxLen; i++)
    {
        int val=abs((int)(*ptr));
        maxval = qMax(maxval, val);

        ptr++;
    }

    emit updateVolume(100*maxval/SHRT_MAX);
}
