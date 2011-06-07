#include "stdio.h"

//#include "SymbolTable.h"

#include "Parser.h"
#include "Scanner.h"
#include <sys/timeb.h>
#include <wchar.h>

#include <apdugenerator.h>
#include <opcodetable.h>
#include <apduviewer.h>
#include <QtGui>
#include <iostream>
#include <tscommunicator.h>

int main(int argc, char *argv[])
{
        std::string apdu = "b8 aa 00 00 Opcode Hexes come here";
        QApplication app( argc, argv );

        ApduViewer* viewer = new ApduViewer();
        TSCommunicator* com = new TSCommunicator( viewer );

        if (argc == 2) {
		wchar_t *fileName = coco_string_create(argv[1]);
		Scanner *scanner = new Scanner(fileName);
                ApduGenerator* gen = new ApduGenerator( new OpcodeTable );
                TSData* tsdata = new TSData();
                Parser *parser = new Parser(scanner, tsdata, com);
		
		parser->Parse();
		
		if (parser->errors->count == 0) 
		{
                        com->out( QString("-- Valid TrivialScript Syntax") );
                        viewer->setApduText( apdu );
                        viewer->show();
		}
                else
                {
                    com->error( QString().append(parser->errors->count).append("Errors occured") );
                    viewer->show();
                }

                coco_string_delete(fileName);
                delete gen;
		delete parser;
		delete scanner;
                delete tsdata;
	} 
	else
		
	{
                com->error( QString("No source file specified\n") );
	}

        delete com;
        delete viewer;

    return app.exec();
}
