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

int main(int argc, char *argv[])
{
        std::string apdu = "b8 aa 00 00 Opcode Hexes come here";
        QApplication app( argc, argv );

        if (argc == 2) {
		wchar_t *fileName = coco_string_create(argv[1]);
		Scanner *scanner = new Scanner(fileName);
                ApduGenerator* gen = new ApduGenerator( new OpcodeTable );
                TSData* tsdata = new TSData();
                Parser *parser = new Parser(scanner, tsdata);
		
		parser->Parse();
		
		if (parser->errors->count == 0) 
		{
			
                        printf("-- Valid TrivialScript Syntax\n");
                        ApduViewer* viewer = new ApduViewer();
                        viewer->setApduText( apdu );
                        viewer->show();
		}
                else
                {
                    std::cout << "-- " << parser->errors->count << " Errors occured. Fix them!" << std::endl;
                    ApduViewer* viewer = new ApduViewer();
                    viewer->setApduText( QString("-- ").append(QString::number(parser->errors->count))
                                        .append(" Errors occured.").toStdString() );
                    viewer->show();
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
