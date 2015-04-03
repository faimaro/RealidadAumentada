#ifndef CLIENTEURL_H
#define CLIENTEURL_H

#include <QNetworkAccessManager>
#include <QImage>

class ClienteURL : public QObject  {
    Q_OBJECT

public:
    ClienteURL();
    void pedirRecurso(QString recurso);
    QImage getImage()  {  return *image;  }

private:
    QNetworkAccessManager * manager;
    QImage * image;

private slots:
    void slot_descargaFinalizada(QNetworkReply * reply);

signals:
    void signal_recursoDescargado();

};

#endif // CLIENTEURL_H
