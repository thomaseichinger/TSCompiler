#ifndef APDUGENERATOR_H
#define APDUGENERATOR_H

#include <QString>

class TSData;

class ApduGenerator
{
public:
    ApduGenerator( TSData* data );
    ~ApduGenerator();

    void generate();
    
    void append( unsigned char opcode );
    void append( std::string );

    void printApdu();
    QString apdu();

    
private:
    TSData* m_data;
    QString m_apdu;
};


#endif // APDUGENERATOR_H
