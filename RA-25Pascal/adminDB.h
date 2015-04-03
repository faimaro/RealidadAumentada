#ifndef ADMINDB_H
#define ADMINDB_H

#include <QSqlDatabase>
#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVector>
#include <QVariant>

class AdminDB: public QObject {
    Q_OBJECT

public:
    AdminDB();
    ~AdminDB();
    bool conectar(QString base);
    QVector<QStringList> ejecutarComandoSelect(QString comandoSelect);
    void desconectar ();
    QSqlDatabase getDB();

private:
    QSqlDatabase db;

};

#endif // ADMINDB_H
