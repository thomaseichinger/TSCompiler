#ifndef APDUGENERATOR_H
#define APDUGENERATOR_H

#include <string>

class OpcodeTable;

class ApduGenerator
{
public:
    ApduGenerator( OpcodeTable* table );
    ~ApduGenerator();
    
    void append( unsigned char opcode );
    void append( std::string );

    void printApdu();
    std::string apdu();
    
private:
    OpcodeTable* m_table;
    std::string m_apdu;
};


#endif // APDUGENERATOR_H
