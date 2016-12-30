#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcodec2wrappers.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();
    checkTools();
}

void MainWindow::onPositionChanged(int newPosition)
{
    if(!m_PlayingNow)
        return;

    qint64  needFulSec = 0;

    if(ui->slProgress->maximum() == 100) {
        needFulSec = static_cast<qint64>((static_cast<float>(newPosition) / 100.0) * m_TotalSecsAvailForPlaying);
    } else {
        needFulSec = newPosition;
    }

    qint64  needFulPos = needFulSec * m_Codec2Reader->bytesPerFrame() * m_Codec2Reader->framesPerSecond();

    m_Reader->seek(needFulPos);
}

void MainWindow::updateTotalLength(const qint64 msecsAvail)
{
    qint64      totalMsecs = msecsAvail / 1000;
    qint64      totalMinutes =  totalMsecs / 60;
    int         secs = totalMsecs % 60;

    ui->lbTotalSecs->setText(lat1("%1:%2")
            .arg(totalMinutes, 2, 10, QLatin1Char('0'))
            .arg(secs, 2, 10, QLatin1Char('0'))
    );
}

MainWindow::~MainWindow()
{
    delete ui;
    ui = Q_NULLPTR;
}

void MainWindow::refreshDevicesList
    (
        QComboBox                       *combobox,
        const QList<QAudioDeviceInfo>   &availDevices,
        const QAudioDeviceInfo          &defaultDevice,
        const QList<QAudioFormat>       &needfulFormats
    )
{
    bool        formatSupported = false;
    combobox->clear();

    //adding devices to combobox
    for(const QAudioDeviceInfo &device : availDevices) {
        if(device.isNull())
            continue;

        formatSupported = false;
        for(const QAudioFormat &format : needfulFormats) {
            if(device.isFormatSupported(format)) {
                formatSupported = true;
                break;
            }
        }

        if(!formatSupported)
            continue;

        combobox->addItem(device.deviceName(), device.deviceName());
    }

    //selecting default or first device
    int index = findIndexInComboboxByData(combobox, defaultDevice.deviceName());
    if(index != -1) {
        combobox->setCurrentIndex(index);
    } else {
        if(combobox->count()) {
            combobox->setCurrentIndex(0);
        }
    }
}

QList<QAudioFormat> MainWindow::audioFormats() const
{
    QList<QAudioFormat>     ret;
    ret.append(QCodec2BaseClass::codec2AudioFormat());
    return ret;
}

void MainWindow::init()
{
    on_actionRefresh_triggered();

    this->setWindowTitle(tr("QCodec2 GUI"));
    this->setWindowIcon(QIcon(QLatin1Literal(":/MainIcon")));


    initializeStatusBar();

    ui->cbBitrate->clear();

    //adding bitrates
    for(const QPair<QString, int> &bitrate : getSupportedCodec2Bitrates()) {
        ui->cbBitrate->addItem(bitrate.first, bitrate.second);
    }

    int index = findIndexInComboboxByData(ui->cbBitrate, CODEC2_MODE_3200);
    if(index != -1) {
        ui->cbBitrate->setCurrentIndex(index);
    } else {
        if(ui->cbBitrate->count()) {
            ui->cbBitrate->setCurrentIndex(0);
        }
    }

    //adding gray selector
    ui->cbEncodingType->clear();
    ui->cbEncodingType->addItem(QLatin1Literal("Natural"), false);
    ui->cbEncodingType->addItem(QLatin1Literal("Gray"), true);

    ui->cbEncodingType->setCurrentIndex(0);

    connect
    (
        ui->slProgress,
        &QSlider::valueChanged,
        this,
        &MainWindow::onPositionChanged
    );
}

QList<QPair<QString, int>> MainWindow::getSupportedCodec2Bitrates() const
{
    QList<QPair<QString, int>>      ret;

    ret
        << QPair<QString, int>(QLatin1Literal("3200"),  CODEC2_MODE_3200)
        << QPair<QString, int>(QLatin1Literal("2400"),  CODEC2_MODE_2400)
        << QPair<QString, int>(QLatin1Literal("1600"),  CODEC2_MODE_1600)
        << QPair<QString, int>(QLatin1Literal("1400"),  CODEC2_MODE_1400)
        << QPair<QString, int>(QLatin1Literal("1300"),  CODEC2_MODE_1300)
        << QPair<QString, int>(QLatin1Literal("1200"),  CODEC2_MODE_1200)
        << QPair<QString, int>(QLatin1Literal("700"),   CODEC2_MODE_700)
        << QPair<QString, int>(QLatin1Literal("700B"),  CODEC2_MODE_700B);

    return ret;
}

QAudioDeviceInfo MainWindow::getDevice(const QList<QAudioDeviceInfo> &devices, const QString &deviceName) const
{
    for(const QAudioDeviceInfo &device : devices) {
        if(device.deviceName() == deviceName)
            return device;
    }

    return QAudioDeviceInfo();
}

QString MainWindow::lat1(const char *value) const
{
    return QLatin1Literal(value);
}

void MainWindow::on_actionRefresh_triggered()
{
    refreshDevicesList(
        ui->cbAudioInputDevice,
        QAudioDeviceInfo::availableDevices(QAudio::AudioInput),
        QAudioDeviceInfo::defaultInputDevice(),
        audioFormats()
    );

    refreshDevicesList(
        ui->cbAudioOutputDevice,
        QAudioDeviceInfo::availableDevices(QAudio::AudioOutput),
        QAudioDeviceInfo::defaultOutputDevice(),
        audioFormats()
    );
}

void MainWindow::on_action_Close_triggered()
{
    close();
}

void MainWindow::on_actionSpecify_file_name_triggered()
{
    QString  fileName = QFileDialog::getSaveFileName(
        this,
        tr("Specify file name"),
        QDir(m_WorkingFileName).dirName(),
        tr("Codec2 files (*.c2 *.cc2);;All files (*.*)"),
        Q_NULLPTR,
        QFileDialog::DontConfirmOverwrite
    );

    if(fileName.isEmpty())
        return;

    m_WorkingFileName = fileName;
    checkTools();
}

void MainWindow::updatePlayingTimeAvail()
{
    qint64      totalMsecs = m_Codec2Reader->msecsAvail();

    m_TotalSecsAvailForPlaying = totalMsecs / 1000;

    qint64      totalMinutes =  m_TotalSecsAvailForPlaying / 60;
    int         secs = m_TotalSecsAvailForPlaying % 60;

    ui->lbTotalSecs->setText(lat1("%1:%2")
            .arg(totalMinutes, 2, 10, QLatin1Char('0'))
            .arg(secs, 2, 10, QLatin1Char('0'))
    );

    ui->slProgress->setMinimum(0);
    if(m_TotalSecsAvailForPlaying <= 100)
        ui->slProgress->setMaximum(m_TotalSecsAvailForPlaying);
    else
        ui->slProgress->setMaximum(100);
}

void MainWindow::on_btPlay_clicked()
{
    if(m_WorkingFileName.isEmpty()) {
        ui->actionSpecify_file_name->trigger();
        if(m_WorkingFileName.isEmpty())
            return;
    }

    if(m_AudioOutput) {
        m_AudioOutput->stop();
        delete m_AudioOutput;
        m_AudioOutput = Q_NULLPTR;
    }

    if(m_Codec2Reader) {
        m_Codec2Reader->stop();
        delete m_Codec2Reader;
        m_Codec2Reader = Q_NULLPTR;
    }

    if(m_Reader) {
        m_Reader->close();
        delete m_Reader;
        m_Reader = Q_NULLPTR;
    }

    QAudioDeviceInfo    deviceInfo = getDevice(
        QAudioDeviceInfo::availableDevices(QAudio::AudioOutput),
        ui->cbAudioOutputDevice->currentData().toString()
    );

    if(QAudioDeviceInfo() == deviceInfo) {
        QMessageBox::warning(this, tr("Error"), tr("Can't find needful output device"));
        return;
    }

    m_Reader = new QFile(m_WorkingFileName);
    m_Reader->open(QIODevice::ReadOnly);

    m_Codec2Reader = new QCodec2Decoder(m_Reader, this);
    m_Codec2Reader->setCodecMode(ui->cbBitrate->currentData().toInt());
    m_Codec2Reader->setNaturalEncoding(ui->cbEncodingType->currentData().toBool());
    m_Codec2Reader->start();

    updatePlayingTimeAvail();

    connect
    (
        m_Codec2Reader,
        &QCodec2BaseClass::updateVolume,
        this,
        &MainWindow::updateVolume
    );

    connect
    (
        m_Codec2Reader,
        &QCodec2Decoder::updatePlayingProgress,
        this,
        &MainWindow::updatePlayingProgress
    );

    QAudioFormat    format = QCodec2BaseClass::codec2AudioFormat();

    m_AudioOutput = new QAudioOutput(deviceInfo, format, this);

    connect
    (
        m_AudioOutput,
        &QAudioOutput::stateChanged,
        this,
        &MainWindow::audioDeviceChanged
    );

    m_AudioOutput->setBufferSize(format.sampleRate()*format.channelCount()*(format.sampleSize()/8));

    m_AudioOutput->start(m_Codec2Reader);

    m_PlayingNow = true;
    checkTools();
}

void MainWindow::audioDeviceChanged(const QAudio::State state)
{
    if(QAudio::StoppedState == state) {
        m_PlayingNow = false;
        m_RecordingNow = false;
    }

    if(QAudio::IdleState == state) {
        m_PlayingNow = false;
        m_RecordingNow = false;
    }

    qDebug() << Q_FUNC_INFO << state;

    checkTools();
}

void MainWindow::updateVolume(const int newValue)
{
    m_VolumeProgressBar->setValue(newValue);
}

void MainWindow::updatePlayingProgress
    (
        const int currentPerc,
        const qint64 currentFrame,
        const qint64 totalFramesCount
    )
{
    QSignalBlocker  blocker(ui->slProgress);
    Q_UNUSED(blocker);

    Q_UNUSED(totalFramesCount);

    if(currentPerc == m_OldPercs)
        return;

    m_OldPercs = currentPerc;

    qint64      currentTotalMsecs = currentFrame * m_Codec2Reader->msecsPerFrame();
    qint64      currentSec  = currentTotalMsecs / 1000;

    if(m_PrevPlayingSecsTotal == currentSec)
        return;

    qint64      totalMinutes =  currentSec / 60;
    int         secs = currentSec % 60;

    ui->lbCurrentSec->setText(lat1("%1:%2")
            .arg(totalMinutes, 2, 10, QLatin1Char('0'))
            .arg(secs, 2, 10, QLatin1Char('0'))
    );

    if(ui->slProgress->maximum() == 100)
        ui->slProgress->setValue(currentPerc);
    else
        ui->slProgress->setValue(currentSec);
}

void MainWindow::on_btRec_clicked()
{
    if(m_WorkingFileName.isEmpty()) {
        ui->actionSpecify_file_name->trigger();
        if(m_WorkingFileName.isEmpty())
            return;
    }

    if(m_AudioInput) {
        m_AudioInput->stop();
        delete m_AudioInput;
        m_AudioInput = Q_NULLPTR;
    }

    if(m_Codec2Writer) {
        m_Codec2Writer->stop();
        delete m_Codec2Writer;
        m_Codec2Writer = Q_NULLPTR;
    }

    if(m_Writer) {
        m_Writer->flush();
        m_Writer->close();

        delete m_Writer;
        m_Writer = Q_NULLPTR;
    }

    QAudioDeviceInfo    deviceInfo = getDevice(
        QAudioDeviceInfo::availableDevices(QAudio::AudioInput),
        ui->cbAudioInputDevice->currentData().toString()
    );

    if(QAudioDeviceInfo() == deviceInfo) {
        QMessageBox::warning(this, tr("Error"), tr("Can't find needful input device"));
        return;
    }

    m_Writer = new QFile(m_WorkingFileName);
    m_Writer->open(QIODevice::WriteOnly | QIODevice::Truncate);

    m_Codec2Writer = new QCodec2Encoder(m_Writer, this);
    m_Codec2Writer->setCodecMode(ui->cbBitrate->currentData().toInt());
    m_Codec2Writer->setNaturalEncoding(ui->cbEncodingType->currentData().toBool());
    m_Codec2Writer->start(QIODevice::Truncate);

    connect
    (
        m_Codec2Writer,
        &QCodec2BaseClass::updateVolume,
        this,
        &MainWindow::updateVolume
    );

    connect
    (
        m_Codec2Writer,
        &QCodec2Encoder::lengthUpdated,
        this,
        &MainWindow::updateTotalLength
    );

    QAudioFormat    format = QCodec2BaseClass::codec2AudioFormat();

    m_AudioInput = new QAudioInput(deviceInfo, format, this);

    connect
    (
        m_AudioInput,
        &QAudioInput::stateChanged,
        this,
        &MainWindow::audioDeviceChanged
    );

    m_AudioInput->setBufferSize(format.sampleRate()*format.channelCount()*(format.sampleSize()/8));

    m_AudioInput->start(m_Codec2Writer);

    m_RecordingNow = true;
    checkTools();
}

void MainWindow::on_btStop_clicked()
{
    if(m_RecordingNow) {
        if(m_AudioInput) {
            m_AudioInput->stop();
            delete m_AudioInput;
            m_AudioInput = Q_NULLPTR;
        }

        if(m_Codec2Writer) {
            m_Codec2Writer->stop();
            delete m_Codec2Writer;
            m_Codec2Writer = Q_NULLPTR;
        }

        if(m_Writer) {
            m_Writer->flush();
            m_Writer->close();

            delete m_Writer;
            m_Writer = Q_NULLPTR;
        }
    }

    if(m_PlayingNow) {
        if(m_AudioOutput) {
            m_AudioOutput->stop();
            delete m_AudioOutput;
            m_AudioOutput = Q_NULLPTR;
        }

        if(m_Codec2Reader) {
            m_Codec2Reader->stop();
            delete m_Codec2Reader;
            m_Codec2Reader = Q_NULLPTR;
        }

        if(m_Codec2Reader) {
            m_Codec2Reader->stop();
            delete m_Codec2Reader;
            m_Codec2Reader = Q_NULLPTR;
        }

        if(m_Reader) {
            m_Reader->close();
            delete m_Reader;
            m_Reader = Q_NULLPTR;
        }
    }
}

void MainWindow::checkTools()
{
    if(!ui)
        return;

    ui->btRec->setEnabled((!m_RecordingNow) && (!m_PlayingNow));
    ui->btPlay->setEnabled((!m_RecordingNow) && (!m_PlayingNow));
    ui->btPause->setEnabled((m_RecordingNow || m_PlayingNow) && (!m_Paused) );
    ui->btStop->setEnabled(m_RecordingNow || m_PlayingNow);

    QString  title = tr("QCodec2 GUI [%1]").arg(QFileInfo(m_WorkingFileName).baseName());

    if(m_RecordingNow) {
        title.append(tr(" recording..."));
        m_StatusLabel->setText(tr("recording"));
    }

    if(m_PlayingNow) {
        title.append(tr(" playing..."));
        m_StatusLabel->setText(tr("playing"));
    }

    setWindowTitle(title);
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

int MainWindow::findIndexInComboboxByData(
        QComboBox               *combobox,
        const QVariant          &data,
        const Qt::ItemDataRole  Role
    )
{
    for(int i=0;i<combobox->count();i++) {
        if(combobox->itemData(i,Role) == data) {
            return i;
        }
    }
    return -1;
}

void MainWindow::initializeStatusBar()
{
    m_VolumeProgressBar = new QProgressBar();
    m_VolumeProgressBar->setMinimum(0);
    m_VolumeProgressBar->setMaximum(100);
    m_VolumeProgressBar->setValue(0);

    m_StatusLabel = new QLabel(tr("waiting"));

    ui->statusBar->addPermanentWidget(new QLabel(tr("Status:")));
    ui->statusBar->addPermanentWidget(m_StatusLabel);

    QWidget             *spacerWidget = new QWidget(this);
    QHBoxLayout         *spacerLayout = new QHBoxLayout(spacerWidget);
    QSpacerItem         *spacer =
       new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);

    spacerLayout->addSpacerItem(spacer);
    spacerWidget->setLayout(spacerLayout);
    ui->statusBar->addPermanentWidget(spacerWidget);

    ui->statusBar->addPermanentWidget(new QLabel(tr("Volume:")));
    ui->statusBar->addPermanentWidget(m_VolumeProgressBar);
}


