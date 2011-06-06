#ifndef TSDATA_H
#define TSDATA_H

#include <QMap>
#include <QString>
#include <stdlib.h>

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

private:
    QMap<QString,QString> m_variableMap;
};

#endif // TSDATA_H
