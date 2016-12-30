#ifndef QCODEC2SHARED_H
#define QCODEC2SHARED_H

#include <QtCore/QtCore>

//! Holds information about last error
class QLastErrorHolder
{
public:
    //! Empty constructor
    QLastErrorHolder() :
        m_ErrorDetected(false)
    {
        //
    }

    //! Construct by error text
    QLastErrorHolder(const QString &errorText) :
        m_ErrorDetected(true),
        m_ErrorString(errorText)
    {
        //
    }

    //! Copy constructor
    QLastErrorHolder(const QLastErrorHolder &other) :
        m_ErrorDetected(other.m_ErrorDetected),
        m_ErrorString(other.m_ErrorString)
    {
        //
    }

    virtual ~QLastErrorHolder()
    {
        //stub
    }

    //! @returns last error string
    QString errorString() const
    {
        return m_ErrorString;
    }

    //! @returns true when error happens, otherwise returns false
    bool isError() const
    {
        return m_ErrorDetected;
    }

    //! the same as isError()
    bool errorDetected() const
    {
        return isError();
    }

    //! the same as isError()
    bool isErrorDetected() const
    {
        return isError();
    }

    //! clears information about last error
    void clearError()
    {
        m_ErrorDetected = false;
        m_ErrorString.clear();
    }

protected:
    //! Sets last error and error flag
    virtual bool setLastError(const QString &errText) const
    {
        m_ErrorDetected = true;
        m_ErrorString   = errText;

        return false;
    }

    bool setLastError(const char* errText) const
    {
        return setLastError(QLatin1String(errText));
    }

private:
    //! True when last error detected (even when text is empty)
    mutable bool        m_ErrorDetected;

    //! Last error text
    mutable QString     m_ErrorString;
};

class QCodec2VolumeLevelCalculator : public QObject
{
    Q_OBJECT

public:
    QCodec2VolumeLevelCalculator(QObject  *parent = Q_NULLPTR);

    virtual ~QCodec2VolumeLevelCalculator();

public slots:
    void addData(const char *data, qint64 maxSize);

signals:
    //! emit max volume in %
    void updateVolume(const int currentVolume);
};

#endif //QCODEC2SHARED_H
