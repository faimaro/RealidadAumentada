#ifndef CAPTURADOR
#define CAPTURADOR

#include <QAbstractVideoSurface>

class Capturador : public QAbstractVideoSurface  {
    Q_OBJECT

public:
    Capturador(QObject *parent = 0);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    bool present(const QVideoFrame &frame);

    QVideoFrame getFrameActual()  {  return frameActual;  }

private:
    QVideoFrame frameActual;
};


#endif // CAPTURADOR

