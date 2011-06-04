#ifndef APDUGENERATOR_H
#define APDUGENERATOR_H
#include <string>

class ApduGenerator
{
public:
    ApduGenerator();
    ~ApduGenerator();
    ApduGenerator( &ApduGenerator );
    
    void append( unsigned char opcode );
    void append( 
    
private:
    std::string m_apdu;
};


#endif // APDUGENERATOR_H
