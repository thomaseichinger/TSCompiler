#include "apduviewer.h"
#include <QtGui>
#include <QClipboard>
#include <QMessageBox>

ApduViewer::ApduViewer(QWidget *parent) :
    QWidget(parent),
    m_apduIsSet(false)
{
    resize(320, 240);
    setWindowTitle(QApplication::translate("toplevel", "TSCompiler APDU Viewer"));
    QLabel *label = new QLabel(QApplication::translate("windowlayout", "Compiled APDU:"));
    m_textEdit = new QTextEdit( this );
    m_textEdit->setReadOnly( true );

    QPushButton *copyButton = new QPushButton( this );
    copyButton->setText(QApplication::translate("copyButton", "Copy to Clipboard"));
    connect(copyButton,SIGNAL(clicked()),this,SLOT(copyToClipboard()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(label);
    layout->addWidget(m_textEdit);
    layout->addWidget(copyButton);
    setLayout(layout);
}

ApduViewer::~ApduViewer()
{

}

void ApduViewer::setApduText( std::string apdu )
{
    ApduViewer::append( QString().fromStdString(apdu) );
    m_apduIsSet = true;
    m_apdu = apdu.c_str();
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
