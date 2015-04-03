#ifndef VIDEO
#define VIDEO

#include <QWidget>
#include <QBoxLayout>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QImage>
#include "capturador.h"

class Video : public QWidget  {
    Q_OBJECT

public:
    explicit Video(QString nombreArchivoVideo, QWidget *parent = 0);
    ~Video();
    QVideoFrame getImagenVideo();

    void reproducir();

private:
    QVideoWidget* videoWidget;
    QMediaPlayer mediaPlayer;

    QPixmap originalPixmap;

    Capturador * capturador;

};

#endif // VIDEO

