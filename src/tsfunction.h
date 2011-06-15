#ifndef TSFUNCTION_H
#define TSFUNCTION_H

#include <QString>
#include <QStringList>

class TSFunction
{
public:
    TSFunction();

    void setName( QString name ) { m_name = name; }
    void setParameters( QStringList paras ) { m_paras = paras; }
    void setBody( QStringList body ) { m_body = body; }
    QString name() { return m_name; }
    QStringList parameters() { return m_paras; }
    QStringList body() { return m_body; }

private:
    QString m_name;
    QStringList m_paras;
    QStringList m_body;
};

#endif // TSFUNCTION_H
