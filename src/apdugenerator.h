#ifndef APDUGENERATOR_H
#define APDUGENERATOR_H

#include <string>

class ApduGenerator
{
public:
    ApduGenerator();
    ~ApduGenerator();
    
    void append( unsigned char opcode );
    void append( std::string );
    
private:
    std::string m_apdu;
};


#endif // APDUGENERATOR_H
