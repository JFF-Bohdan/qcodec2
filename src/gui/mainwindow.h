#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia/QAudio>

namespace Ui {
    class MainWindow;
}

class QComboBox;
class QAudioDeviceInfo;
class QAudioFormat;
class QProgressBar;
class QFile;
class QLabel;
class QAudioInput;
class QAudioOutput;
class QCodec2Encoder;
class QCodec2Decoder;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QString             m_WorkingFileName;
    QProgressBar        *m_VolumeProgressBar = Q_NULLPTR;
    QLabel              *m_StatusLabel = Q_NULLPTR;

    bool                m_RecordingNow = false;
    bool                m_PlayingNow = false;
    bool                m_Paused = false;

    QAudioInput         *m_AudioInput = Q_NULLPTR;
    QCodec2Encoder      *m_Codec2Writer = Q_NULLPTR;
    QFile               *m_Writer = Q_NULLPTR;

    QAudioOutput        *m_AudioOutput = Q_NULLPTR;
    QCodec2Decoder      *m_Codec2Reader = Q_NULLPTR;
    QFile               *m_Reader = Q_NULLPTR;

    qint64              m_PrevPlayingSecsTotal = -1;
    qint64              m_TotalSecsAvailForPlaying = 0;
    int                 m_OldPercs = -1;

    /*!
     * \brief refreshDevicesList refreshes avail audio devices list (may be
     * useful when using USB sound cards)
     * \param combobox pointer to combobox which must be filled with devices list
     * \param availDevices list of avail devices (inputs or outputs)
     * \param defaultDevice device which must be selected in combobox
     * \param needfulFormats list of formats one of them which must be supported by audio device
     */
    void refreshDevicesList(
        QComboBox                       *combobox,
        const QList<QAudioDeviceInfo>   &availDevices,
        const QAudioDeviceInfo          &defaultDevice,
        const QList<QAudioFormat>       &needfulFormats
    );

    //! returns all formats which must be supported by audio device
    QList<QAudioFormat> audioFormats() const;

    //! initialized main window
    void init();

    //! returns all bitrates supported by codec2 library
    QList<QPair<QString, int>> getSupportedCodec2Bitrates() const;

    QAudioDeviceInfo getDevice(const QList<QAudioDeviceInfo> &devices, const QString &deviceName) const;

    //! converts const char* pointer value to QString
    QString lat1(const char* value) const;

    /*!
     * \brief findIndexInComboboxByData can find index in QComboBox by data (by default uses Qt::UserRole)
     * \param ComboBox pointer to combobox
     * \param data data for searching
     * \param Role role (default - Qt::UserRole)
     * \return index (>=0) if something found, otherwise returns -1
     */
    int findIndexInComboboxByData(
        QComboBox                   *ComboBox,
        const QVariant              &data,
        const Qt::ItemDataRole      Role = Qt::UserRole
    );

    void initializeStatusBar();

private slots:
    void on_actionRefresh_triggered();

    void on_action_Close_triggered();

    void on_btRec_clicked();

    void on_btStop_clicked();

    void checkTools();

    void on_btPlay_clicked();

    void audioDeviceChanged(const QAudio::State state);

    void updateVolume(const int newValue);

    void updatePlayingProgress
    (
        const int currentPerc,
        const qint64 currentFrame,
        const qint64 totalFramesCount
    );

    void on_actionSpecify_file_name_triggered();

    void on_actionAbout_Qt_triggered();

    void updatePlayingTimeAvail();

    void onPositionChanged(int newPosition);

    void updateTotalLength(const qint64 msecsAvail);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
