#include "apdugenerator.h"
#include "tsdata.h"
#include <iostream>

ApduGenerator::ApduGenerator( TSData* data )
    : m_data( data )
{
}

ApduGenerator::~ApduGenerator()
{
}

void ApduGenerator::generate()
{

}

QString ApduGenerator::apdu()
{
    return m_apdu;
}

void ApduGenerator::append( QChar opcode )
{
    m_apdu.append( opcode );
    return;
}

void ApduGenerator::append( QString field )
{
    m_apdu.append( field );
    return;
}

void ApduGenerator::printApdu()
{
    std::cout << m_apdu.toStdString() << std::endl;
    return;
}
