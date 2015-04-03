#ifndef PERSONA_H
#define PERSONA_H

#include <QString>
#include "cara.h"

class Persona  {
public:
    Persona()  {

    }

    Cara getCara() const  {  return cara;  }
    QString getNombre() const  {  return nombre;  }
    QString getLinkedin() const  {  return urlLinkedin;  }
    QString getMail() const  {  return mail;  }
    void setCara(Cara cara)  {  this->cara = cara;  }
    void setNombre(QString nombre)  {  this->nombre = nombre;  }
    void setLinkedin(QString urlLinkedin)  {  this->urlLinkedin = urlLinkedin;  }
    void setMail(QString mail)  {  this->mail = mail;  }

private:
    QString nombre;
    Cara cara;
    QString mail;
    QString urlLinkedin;
};

#endif // PERSONA_H
