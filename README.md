# qcodec2
qcodec2 is Qt wrappers for codec2 speech codec. You can find more information about codec2 here:
http://www.rowetel.com/?page_id=452

It contains two main classes QCodec2Encoder and QCodec2Decoder which wraps codec2 calls with QAudioOutput and QAudioInput standard classes and Qt audio structure.

Also it contains complete example which uses these wrappers for player implementation. Player can write and read RAW codec2 encoded speech data.

# For Windows users

You can find binary in codec2-lib/ folder. If you want to build codec2 library under Windows OS you can read more:
http://bohdan-danishevsky.blogspot.com/2016/12/building-codec2-for-windows-using-cmake.html

# Usage for recording voice (encoding)

First of all you need open output file:
```
    outputFile = new QFile(fileName);
    outputFile->open(QIODevice::WriteOnly | QIODevice::Truncate);
```	
Then you need create encoder and attach output file to the encoder:
```
    encoder = new QCodec2Encoder(outputFile, this);
    encoder->setCodecMode(ui->cbBitrate->currentData().toInt());
    encoder->setNaturalEncoding(true);
    encoder->start(QIODevice::Truncate);
```

Finally, you need initialize QAudioInput:
```
    QAudioFormat    format = QCodec2BaseClass::codec2AudioFormat();
    audioInput = new QAudioInput(deviceInfo, format, this);

    connect
    (
        audioInput,
        &QAudioInput::stateChanged,
        this,
        &MainWindow::audioDeviceChanged
    );

    audioInput->setBufferSize(format.sampleRate()*format.channelCount()*(format.sampleSize()/8));

    audioInput->start(m_Codec2Writer);
```

At the end do not forget flush buffers and close file.

```
    audioInput->stop();
	
    encoder->stop();

    outputFile->flush();
    outputFile->close();
```

# Usage for playing voice
First of all you need open output file:

```
    audioInput = new QFile(m_WorkingFileName);
    audioInput->open(QIODevice::ReadOnly);
```
	
Next you need create decoder:

```
	decoder = new QCodec2Decoder(audioInput, this);
    decoder->setCodecMode(ui->cbBitrate->currentData().toInt());
    decoder->setNaturalEncoding(ui->cbEncodingType->currentData().toBool());
    decoder->start();
```
	
Finally, you need create audio QAudioOutput for plyaing decoded data:

```
    QAudioFormat    format = QCodec2BaseClass::codec2AudioFormat();

    audioOutput = new QAudioOutput(deviceInfo, format, this);

    connect
    (
        audioOutput,
        &QAudioOutput::stateChanged,
        this,
        &MainWindow::audioDeviceChanged
    );

    audioOutput->setBufferSize(format.sampleRate()*format.channelCount()*(format.sampleSize()/8));

    audioOutput->start(decoder);
```

At the end do not forget flush buffers and close file.

```
    audioOutput->stop();
	
    decoder->stop();

    audioInput->flush();
    audioInput->close();
```

	
# Documentation

You can build documentation using Doxygen which can be found at:
http://www.stack.nl/~dimitri/doxygen

using Doxyfile in main folder