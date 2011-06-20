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
    
    void append( QChar opcode );
    void append( QString );

    void printApdu();
    QString apdu();
    
private:
    TSData* m_data;
    QString m_apdu;
};


#endif // APDUGENERATOR_H
