#include "tsdata.h"
#include "tsfunction.h"

TSData::TSData()
{
}

TSData::~TSData()
{
    QList<TSFunction*>::iterator iter = m_functionList.begin();
    while( iter != m_functionList.constEnd() )
    {
        delete *iter;
        ++iter;
    }
}

void TSData::generate()
{
    QList<TSFunction*>::const_iterator iter = m_functionList.constBegin();
    QList<TSFunction*>::const_iterator end = m_functionList.constEnd();
    TSFunction* f;
    while (iter != end )
    {
        f = *iter;
        if (f->name() == QString("PROZESS"))
            break;
        ++iter;
    }
    QStringList inflated = inflate(f->body());

}

QStringList TSData::inflate( QStringList body )
{
    return body;
}

bool TSData::variable(QString name)
{
    return m_variableMap.contains( name );
}

void TSData::addVariableWithValue(QString name, QString value)
{
    m_variableMap.insert( name, value );
}

QString TSData::variableValue(QString name)
{
    return m_variableMap.value( name );
}

void TSData::addFunction(TSFunction *f)
{
    m_functionList.append( f );
}

void TSData::addObject(TSObject *o)
{
    m_objectList.append( o );
}

QString TSData::dumpDataStructures()
{
    QString print;
    print.append("Data strucuture dump =>\n");
    print.append("Variables: ").append("\n");
    QMap<QString,QString>::const_iterator iter = m_variableMap.constBegin();
    while( iter != m_variableMap.constEnd() )
    {
        print.append(iter.key()).append(" : ").append(iter.value()).append("\n");
        ++iter;
    }
    print.append("\n\n-- Functions: \n");
    QList<TSFunction*>::const_iterator liter = m_functionList.constBegin();
    while( liter != m_functionList.constEnd() )
    {
        print.append((*liter)->name()).append( " (" );
        QStringList paras = (*liter)->parameters();
        QStringList::iterator piter = paras.begin();
        QStringList::iterator pend = paras.end();
        while( piter != pend )
        {
            print.append(*piter).append(" _ ");
            ++piter;
        }
        print.append(") ->\n");

        QStringList body = (*liter)->body();
        QStringList::const_iterator biter = body.constBegin();
        QStringList::const_iterator bend = body.constEnd();
        while( biter != bend )
        {
            print.append("\t");
            print.append(*biter);
            print.append("\n");
            ++biter;
        }
        print.append("\n");
        ++liter;
    }

    print.append("\n\n-- Objects: \n");
    QList<TSObject*>::const_iterator oiter = m_objectList.constBegin();
    while ( oiter != m_objectList.constEnd() )
    {
        print.append((*oiter)->name()).append(" {\n");

        QMap<QString,QString> members = (*oiter)->members();
        QMap<QString,QString>::const_iterator miter = members.constBegin();
        QMap<QString,QString>::const_iterator mend = members.constEnd();
        while ( miter != mend )
        {
            print.append("\t");
            print.append(miter.key()).append(" : ").append(miter.value());
            print.append("\n");
            ++miter;
        }

        print.append("}\n\n");

        ++oiter;
    }

    return print;
}
