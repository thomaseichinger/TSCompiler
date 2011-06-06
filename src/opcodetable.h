#ifndef OPCODETABLE_H
#define OPCODETABLE_H

typedef unsigned char uchar;

class OpcodeTable
{
public:
    OpcodeTable();
    virtual ~OpcodeTable();

    /* Here come the functions returning the opcode byte for the
      according Interpreter function*/
    uchar send() { return 0x32; }
    uchar echo() { return 0x33; }
};

#endif // OPCODETABLE_H
