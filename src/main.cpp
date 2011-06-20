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
        QApplication app( argc, argv );

        TSCommunicator com;
        ApduViewer viewer( &com );
        com.setGui( &viewer );
        com.setDebug(false);

        if (argc == 2) {
                viewer.setFileName(QString(argv[1]));
                viewer.run();
                viewer.show();
	} 
        else
	{
                com.error( QString("No source file specified\n") );
                viewer.show();
        }

    return app.exec();
}
