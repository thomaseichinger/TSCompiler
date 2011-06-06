#include "apdugenerator.h"
#include "opcodetable.h"
#include <iostream>

ApduGenerator::ApduGenerator( OpcodeTable* table )
    : m_table( table )
{
}

ApduGenerator::~ApduGenerator()
{
}

void ApduGenerator::append( unsigned char opcode )
{
    //m_apdu.append( opcode );
    return;
}

void ApduGenerator::append( std::string field )
{
    m_apdu.append( field );
    return;
}

void ApduGenerator::printApdu()
{
    std::cout << m_apdu << std::endl;
    return;
}

std::string ApduGenerator::apdu()
{
    return m_apdu;
}
