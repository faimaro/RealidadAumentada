#include<iostream>
#include <QString>
#include <QFile>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtGlobal>
#include <QDebug>
#include "qlogging.h"
void readJson();

int main()
{
    readJson();
    return 0;
}
/*
Parsea el Json obteniendo solo "title","price",picture:"url".
*/
void readJson()
   {
      QString val;
      QFile file;
      file.setFileName("/home/fran/dest.json");
      file.open(QIODevice::ReadOnly | QIODevice::Text);
      val = file.readAll();
      file.close();

      //Toma como parametro un documento json valido.
      QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
      //Pide todos los array que existan en ese json.
      QJsonArray a = d.array();//solo 1 array en este json
      for(int i=0; i<a.size();i++){
          QJsonValue value = a.at(i);
          //Obtiene el objeto de posicion "i" del array "a"
          QJsonObject obj = value.toObject();
          //obtiene el objeto llamado "picture"
          QJsonObject pic = obj.take("picture").toObject();
          qDebug()<<"Titulo:"<<obj.take("title").toString()<<"$"<<obj.take("price").toDouble()<<"url:"<<pic.take("url").toString();
      }

   }
