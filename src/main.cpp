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

int main(int argc, char *argv[])
{
        std::string apdu = "b8 aa 00 00 Opcode Hexes come here";
        QApplication app( argc, argv );
        ApduViewer* viewer = new ApduViewer();
        viewer->show();
        viewer->setApduText( apdu );

        if (argc == 2) {
		wchar_t *fileName = coco_string_create(argv[1]);
		Scanner *scanner = new Scanner(fileName);
                ApduGenerator* gen = new ApduGenerator( new OpcodeTable );
                Parser *parser = new Parser(scanner);
		
		parser->Parse();
		
		if (parser->errors->count == 0) 
		{
			
                        printf("-- Valid TrivialScript Syntax\n");
                        //ApduViewer* viewer = new ApduViewer();
                        //viewer->show();
		}

                coco_string_delete(fileName);
                delete gen;
		delete parser;
		delete scanner;
	} 
	else
		
	{
		printf("-- No source file specified\n");
	}

    return app.exec();
}
