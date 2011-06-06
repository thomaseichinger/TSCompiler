#include "tsdata.h"

TSData::TSData()
{
}

TSData::~TSData()
{

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
