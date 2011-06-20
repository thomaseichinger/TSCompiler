#ifndef TSDATA_H
#define TSDATA_H

#include <QMap>
#include <QList>
#include <QString>
#include <QStringList>
#include <stdlib.h>
#include "tsfunction.h"
#include "tsobject.h"

class TSData
{
public:
    TSData();
    ~TSData();
    QString version() { return QString("01"); }
    void quit() { exit(10); }

    bool variable( QString name );
    void addVariableWithValue( QString name, QString value );
    QString variableValue( QString name );

    void addFunction( TSFunction* f );
    QList<TSFunction*> functions() { return m_functionList; }

    void addObject( TSObject* o );
    QList<TSObject*> objects() { return m_objectList; }

    QString dumpDataStructures();

    void generate();

    QString m_currentIdent;

private:
    QStringList inflate(QStringList);
    QMap<QString,QString> m_variableMap;
    QList<TSFunction*> m_functionList;
    QList<TSObject*> m_objectList;
};

#endif // TSDATA_H
