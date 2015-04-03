#include "clienteURL.h"
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>

ClienteURL::ClienteURL()  {
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_descargaFinalizada(QNetworkReply*)));

    image = new QImage;
}

void ClienteURL::pedirRecurso(QString recurso)  {
    manager->get(QNetworkRequest(QUrl(recurso)));
}

void ClienteURL::slot_descargaFinalizada(QNetworkReply * reply)  {
    image = new QImage(QImage::fromData(reply->readAll()));

    // Hacemos este chequeo para que no emita senal si la imagen no esta
    if (!image->isNull())
        emit signal_recursoDescargado();

    image->save("streetview.png");
}
