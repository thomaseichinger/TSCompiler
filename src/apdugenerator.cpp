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

void ApduGenerator::append( unsigned char opcode )
{
    //m_apdu.append( opcode );
    return;
}

void ApduGenerator::append( std::string field )
{
    m_apdu.append( QString::fromStdString(field) );
    return;
}

void ApduGenerator::printApdu()
{
    std::cout << m_apdu.toStdString() << std::endl;
    return;
}
