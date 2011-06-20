#include "apduviewer.h"
#include "Parser.h"
#include "Scanner.h"
#include <sys/timeb.h>
#include <wchar.h>
#include <apdugenerator.h>
#include <opcodetable.h>
#include <tscommunicator.h>
#include <QtGui>
#include <QClipboard>
#include <QMessageBox>

ApduViewer::ApduViewer(TSCommunicator* com, QWidget *parent) :
    QWidget(parent),
    m_com(com),
    m_apduIsSet(false)
{
    resize(320, 240);
    setWindowTitle(QApplication::translate("toplevel", "TSCompiler APDU Viewer"));
    QLabel *label = new QLabel(QApplication::translate("windowlayout", "Compiled APDU:"));
    m_textEdit = new QTextEdit( this );
    m_textEdit->setReadOnly( true );

    QHBoxLayout *fileLayout = new QHBoxLayout();
    m_fileSel = new QLineEdit(this);
    QPushButton *fileButton = new QPushButton(this);
    QPushButton *runButton = new QPushButton(QString("Run"), this);
    fileButton->setText(QString("Choose file..."));
    fileLayout->addWidget(m_fileSel);
    fileLayout->addWidget(fileButton);
    fileLayout->addWidget(runButton);

    QPushButton *copyButton = new QPushButton( this );
    copyButton->setText(QApplication::translate("copyButton", "Copy to Clipboard"));
    connect(copyButton,SIGNAL(clicked()),this,SLOT(copyToClipboard()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(fileLayout);
    layout->addWidget(label);
    layout->addWidget(m_textEdit);
    layout->addWidget(copyButton);
    setLayout(layout);

    connect(fileButton,SIGNAL(clicked()),this,SLOT(showFileDialog()));
    connect(runButton,SIGNAL(clicked()),this,SLOT(run()));
}

ApduViewer::~ApduViewer()
{

}

void ApduViewer::showFileDialog()
{
    QFileDialog *d = new QFileDialog(this);
    connect(d,SIGNAL(fileSelected(QString)),m_fileSel,SLOT(setText(QString)));
    d->show();
}

void ApduViewer::setFileName(QString name)
{
    m_fileSel->setText(name);
}

void ApduViewer::run()
{
    wchar_t *fileName = coco_string_create(m_fileSel->text().toStdString().c_str());
    Scanner *scanner = new Scanner(fileName);
    TSData* tsdata = new TSData();
    ApduGenerator* gen = new ApduGenerator( tsdata );
    Parser *parser = new Parser(scanner, tsdata, m_com);

    parser->Parse();

    if (parser->errors->count == 0)
    {
            m_com->out( QString("Valid TrivialScript Syntax") );
            setApduText( gen->apdu() );
            m_com->debugMsg( tsdata->dumpDataStructures() );
            show();
    }
    else
    {
        m_com->error( QString().append("%1 Errors occured")
                             .arg(parser->errors->count)
                   );
        show();
    }

    coco_string_delete(fileName);
    delete gen;
    delete parser;
    delete scanner;
    delete tsdata;
    return;
}

void ApduViewer::setApduText( QString apdu )
{
    ApduViewer::append( apdu );
    m_apduIsSet = true;
    m_apdu = apdu;
    return;
}

void ApduViewer::copyToClipboard()
{
    if (!m_apduIsSet)
    {
        qDebug( "apdu not set -> not copied" );
        QMessageBox msgBox;
        msgBox.setText("No apdu generatred.\nErros occured");
        msgBox.exec();
    }
    else
    {
        qDebug("apdu set and copied to clip");
        QClipboard *clip = QApplication::clipboard();
        clip->setText( m_apdu );
    }
}

void ApduViewer::append(QString er)
{
    m_textEdit->append( er );
}
