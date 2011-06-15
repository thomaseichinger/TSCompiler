#ifndef TSOBJECT_H
#define TSOBJECT_H

#include <QMap>
#include <QString>

class TSObject
{
public:
    TSObject();
    TSObject( QString name );

    void setName( QString name ) { m_name = name; }
    QString name() { return m_name; }

    void addMemberWithValue( QString name, QString value ) { m_memberMap.insert( name, value ); }
    bool isMember( QString name ) { return m_memberMap.contains( name ); }
    QString value( QString name ) { return m_memberMap.value( name ); }

    QMap<QString,QString> members() { return m_memberMap; }

private:
    QMap<QString,QString> m_memberMap;
    QString m_name;
};

#endif // TSOBJECT_H
