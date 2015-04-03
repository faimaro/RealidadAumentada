#include "video.h"
#include "configuracion.h"
#include <QRect>



// nombreArchivoVideo es el nombre del archivo incluyendo la extension. Por ejemplo: 25UBP.mpg
Video::Video(QString nombreArchivoVideo, QWidget *parent) : QWidget(parent)  {

//    this->setWindowFlags(Qt :: Window | Qt::FramelessWindowHint );
//    this->setAttribute(Qt::WA_TranslucentBackground, true);
//    this->setMinimumSize(300,200);

    capturador = new Capturador;

//    videoWidget = new QVideoWidget(this);

//    QBoxLayout *controlLayout = new QHBoxLayout;
//    controlLayout->setMargin(0);
//    controlLayout->addWidget(videoWidget);
//    this->setLayout(controlLayout);

    mediaPlayer.setVideoOutput(capturador);
    mediaPlayer.setMedia(QUrl::fromLocalFile(Configuracion::carpetaDeTrabajo + "/videos/" + nombreArchivoVideo));
    // Convertir con Video Freemake con:
    // Codec video MPEG-1 / 29.97 fps / 640x480

    mediaPlayer.play();

}

Video::~Video()  {
}

QVideoFrame Video::getImagenVideo()  {

    return capturador->getFrameActual();

}

void Video::reproducir()  {
    mediaPlayer.play();
}


