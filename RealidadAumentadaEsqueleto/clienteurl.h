/* Esta clase se utilizara para descargar recursos de Internet
 * a traves del metodo ClienteUrl::descargar("Url del recurso"); */

#ifndef CLIENTEURL_H
#define CLIENTEURL_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QUrl>

class ClienteUrl : public QNetworkAccessManager
{
    Q_OBJECT

public:
    ClienteUrl();
    void descargar(QString url);

private slots:
    void slot_descargaFinalizada(QNetworkReply *reply);

signals:
    void descargaFinalizada(QByteArray datos);
};

#endif // CLIENTEURL_H
