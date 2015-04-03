/*
Copyright (c) 2013 Raivis Strogonovs

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "smtp.h"

Smtp::Smtp( const QString &user, const QString &pass, const QString &host, int port, int timeout )
{    
    socket = new QSslSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected() ) );
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(errorReceived(QAbstractSocket::SocketError)));   
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(disconnected()), this,SLOT(disconnected()));


    this->user = user;
    this->pass = pass;

    this->host = host;
    this->port = port;
    this->timeout = timeout;


}

void Smtp::sendMail(const QString &from, const QString &to, const QString &subject, QString &body, QFile * archivoAdjunto)
{
    const QString TEXT_SEPARATOR =  "bcaec54312c6b3757704fa7atext";
    const QString FILE_SEPARATOR = "bcaec54312c6b3757704fa7afile";
    QString encoding = "UTF-8";

    message = "To: " + to + "\n";
    message.append("From: Proyecto TUBot <" + from + ">\n");
    message.append("Subject: " + subject + "\r\n");
    message.append("MIME-Version: 1.0\r\n");

    if ( archivoAdjunto == 0 )  // Si no hay archivo que adjuntar, entonces esto
    {
        message.append("Content-Type: text/plain; charset=" + encoding + "\r\n");
        message.append("Content-Transfer-Encoding: 8bit\r\n");

        message.append(body);
    }
    else  {
        message.append(QString("Content-Type: multipart/mixed; boundary=\"%1\"\r\n").arg(FILE_SEPARATOR));

        // prepend plain text
        message.append(QString("--%1\r\n"
                      "Content-Type: multipart/alternative; boundary=%4\r\n\r\n"
                      "--%4\r\n"
                      "Content-Type: text/plain; charset=%2\r\n"
                      "%3\r\n\r\n"
                      "--%4\r\n"
                      "Content-Type: text/html; charset=%2\r\n"
                      "<div dir=\"ltr\">%3</div>\r\n\r\n"
                      "--%4--").arg(FILE_SEPARATOR, encoding, body, TEXT_SEPARATOR));

        message.append(QString("\r\n--%1\r\n").arg(FILE_SEPARATOR));
        message.append(QString(
                "Content-Type: image/png; name=\"p_fotocopia.png\"\r\n"
                "Content-Transfer-Encoding: base64\r\n"
                "Content-Disposition: attachment; filename=\"p_fotocopia.png\"\r\n\r\n"));

        QDataStream dataStream(archivoAdjunto);    // read the data serialized from the file

        if (!archivoAdjunto->open(QIODevice::ReadOnly)) {
            qDebug() << "No se pudo abrir el archivo para lectura";
            return;
        }

        char *dataImagen = new char[archivoAdjunto->size()];
        dataStream.readRawData(dataImagen, archivoAdjunto->size());

        message.append(QByteArray(dataImagen, archivoAdjunto->size()).toBase64() + "\r\n");
        message.append(QString("--%1--").arg(FILE_SEPARATOR));

        message.append("\r\n");
    }

    message.replace( QString::fromLatin1( "\n" ), QString::fromLatin1( "\r\n" ) );
    message.replace( QString::fromLatin1( "\r\n.\r\n" ),
    QString::fromLatin1( "\r\n..\r\n" ) );
    this->from = from;
    rcpt = to;
    state = Init;
    socket->connectToHostEncrypted(host, port); //"smtp.gmail.com" and 465 for gmail TLS
    if (!socket->waitForConnected(timeout)) {
         qDebug() << socket->errorString();
     }

    t = new QTextStream( socket );
}

Smtp::~Smtp()
{
    delete t;
    delete socket;
}
void Smtp::stateChanged(QAbstractSocket::SocketState socketState)
{

    qDebug() <<"stateChanged " << socketState;
}

void Smtp::errorReceived(QAbstractSocket::SocketError socketError)
{
    qDebug() << "error " <<socketError;
}

void Smtp::disconnected()
{

    qDebug() <<"disconneted";
    qDebug() << "error "  << socket->errorString();
}

void Smtp::connected()
{    
    qDebug() << "Connected ";
}

void Smtp::readyRead()
{

     qDebug() <<"readyRead";
    // SMTP is line-oriented

    QString responseLine;
    do
    {
        responseLine = socket->readLine();
        response += responseLine;
    }
    while ( socket->canReadLine() && responseLine[3] != ' ' );

    responseLine.truncate( 3 );

    qDebug() << "Server response code:" <<  responseLine;
    qDebug() << "Server response: " << response;

    if ( state == Init && responseLine == "220" )
    {
        // banner was okay, let's go on
        *t << "EHLO localhost" <<"\r\n";
        t->flush();

        state = HandShake;
    }
    //No need, because I'm using socket->startClienEncryption() which makes the SSL handshake for you
    /*else if (state == Tls && responseLine == "250")
    {
        // Trying AUTH
        qDebug() << "STarting Tls";
        *t << "STARTTLS" << "\r\n";
        t->flush();
        state = HandShake;
    }*/
    else if (state == HandShake && responseLine == "250")
    {
        socket->startClientEncryption();
        if(!socket->waitForEncrypted(timeout))
        {
            qDebug() << socket->errorString();
            state = Close;
        }


        //Send EHLO once again but now encrypted

        *t << "EHLO localhost" << "\r\n";
        t->flush();
        state = Auth;
    }
    else if (state == Auth && responseLine == "250")
    {
        // Trying AUTH
        qDebug() << "Auth";
        *t << "AUTH LOGIN" << "\r\n";
        t->flush();
        state = User;
    }
    else if (state == User && responseLine == "334")
    {
        //Trying User        
        qDebug() << "Username";
        //GMAIL is using XOAUTH2 protocol, which basically means that password and username has to be sent in base64 coding
        //https://developers.google.com/gmail/xoauth2_protocol
        *t << QByteArray().append(user).toBase64()  << "\r\n";
        t->flush();

        state = Pass;
    }
    else if (state == Pass && responseLine == "334")
    {
        //Trying pass
        qDebug() << "Pass";
        *t << QByteArray().append(pass).toBase64() << "\r\n";
        t->flush();

        state = Mail;
    }
    else if ( state == Mail && responseLine == "235" )
    {
        // HELO response was okay (well, it has to be)

        //Apperantly for Google it is mandatory to have MAIL FROM and RCPT email formated the following way -> <email@gmail.com>
        qDebug() << "MAIL FROM:<" << from << ">";
        *t << "MAIL FROM:<" << from << ">\r\n";
        t->flush();
        state = Rcpt;
    }
    else if ( state == Rcpt && responseLine == "250" )
    {
        //Apperantly for Google it is mandatory to have MAIL FROM and RCPT email formated the following way -> <email@gmail.com>
        *t << "RCPT TO:<" << rcpt << ">\r\n"; //r
        t->flush();
        state = Data;
    }
    else if ( state == Data && responseLine == "250" )
    {

        *t << "DATA\r\n";
        t->flush();
        state = Body;
    }
    else if ( state == Body && responseLine == "354" )
    {

        *t << message << "\r\n.\r\n";
        t->flush();
        state = Quit;
    }
    else if ( state == Quit && responseLine == "250" )
    {

        *t << "QUIT\r\n";
        t->flush();
        // here, we just close.
        state = Close;
        emit status( tr( "Message sent" ) );
    }
    else if ( state == Close )
    {
        deleteLater();
        return;
    }
    else
    {
        // something broke.
        QMessageBox::warning( 0, tr( "Qt Simple SMTP client" ), tr( "Unexpected reply from SMTP server:\n\n" ) + response );
        state = Close;
        emit status( tr( "Failed to send message" ) );
    }
    response = "";
}
