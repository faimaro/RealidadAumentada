#include "adminDB.h"


AdminDB::AdminDB()  {
//    db = QSqlDatabase::addDatabase("QMYSQL");
    db = QSqlDatabase::addDatabase("QODBC");

    /**
      * Me sucedio cargando el controlador para MYSQL que encontraba el driver pero no lo podia cargar.
      * Buscando y buscando, encontre que algunas versiones de Qt utilizan distintos drivers de QMYSQL, estos drivers son
      * los que se encuentran en la carpeta /Qt5.3.1/5.3/gcc/plugins/sqldrivers (que son archivos .so).
      * Este driver utiliza una libreria llamada libmysqlclient_r.so. Pero distintas versiones de Qt utilizan distintas
      * versiones de esta libreria libmysqlclient_r.so (libmysqlclient_r.so.16 o libmysqlclient_r.so.18)
      * Como lo solucione? Yo tenia la libmysqlclient_r.so.18 y me hacia falta la libmysqlclient_r.so.16:
      *     sudo ln -s libmysqlclient_r.so.18 libmysqlclient_r.so.16
      *         el archivo libmysqlclient_r.so.16 es el archivo (link) que se crea.
      *
      * Como averigue cual version hace falta?
      *         ldd /Qt5.3.1/5.3/gcc/plugins/sqldrivers/libqsqlmysql.so
      *
      *     y me tira como resultado todas las dependencias, donde aparece:
      *         libmysqlclient_r.so.16 => not found
      */

}

bool AdminDB::conectar(QString base)  {
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=" + base);

//    db.setHostName ("localhost");
//    db.setDatabaseName (base);
//    db.setUserName ("root");
//    db.setPassword ("y3rb44m4nd4");

    return db.open();
}

// Este metodo ejecuta un SELECT y devuelve un QVector con los registros
// La cantidad de elementos dentro del QVector es la cantidad de registros obtenidos
// Cada uno de los QString dentro del QStringList son los campos de cada registro
QVector<QStringList> AdminDB::ejecutarComandoSelect(QString comandoSelect)  {
    if (!db.isOpen())
        return QVector<QStringList>();  // Si no esta abierta la conexion a la base, devolvemos un QVector vacio.

    QStringList campos;
    QVector<QStringList> registros;

    QSqlQuery query(db);
    query.exec(comandoSelect);

    while(query.next())  {
        QSqlRecord record = query.record();

        // Cada ciclo es un campo para ir almacenando en cada elemento del QStringList
        for (int i=0 ; i<record.count() ; i++)  {
            campos << record.value(i).toString();  // Agregamos el campo i
        }

        registros.push_back(campos);  // Agregamos un nuevo registro con todos los campos obtenidos con el SELECT

        campos.clear();  // Vaciamos el QStringList para almacenar los campos del siguiente registro.
    }

    return registros;
}

void AdminDB::desconectar()  {
    db.close();
}

QSqlDatabase AdminDB::getDB()  {
    return db;
}

AdminDB::~AdminDB()  {
    this->desconectar();
}
