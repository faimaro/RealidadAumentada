#include "clienteurl.h"

ClienteUrl::ClienteUrl()
{
    connect(this, SIGNAL(finished(QNetworkReply*)), SLOT(slot_descargaFinalizada(QNetworkReply*)));
}

void ClienteUrl::descargar(QString url)
{
    this->get(QNetworkRequest(QUrl(url)));
}

void ClienteUrl::slot_descargaFinalizada(QNetworkReply *reply)
{
    // Terminamos de descargar y mandamos los datos

    emit descargaFinalizada(reply->readAll());
}
