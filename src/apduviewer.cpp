#include "apduviewer.h"
#include <QtGui>
#include <QClipboard>

ApduViewer::ApduViewer(QWidget *parent) :
    QWidget(parent)
{
    resize(320, 240);
    setWindowTitle(QApplication::translate("toplevel", "TSCompiler APDU Viewer"));
    QLabel *label = new QLabel(QApplication::translate("windowlayout", "Compiled APDU:"));
    QTextEdit *textEdit = new QTextEdit( this );
    m_textEdit = textEdit;
    m_textEdit->setReadOnly( true );

    QPushButton *copyButton = new QPushButton( this );
    copyButton->setText(QApplication::translate("copyButton", "Copy to Clipboard"));
    connect(copyButton,SIGNAL(clicked()),this,SLOT(copyToClipboard()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(label);
    layout->addWidget(textEdit);
    layout->addWidget(copyButton);
    setLayout(layout);
}

ApduViewer::~ApduViewer()
{

}

void ApduViewer::setApduText( std::string apdu )
{
    m_textEdit->setPlainText( QString(apdu.c_str()) );
    return;
}

void ApduViewer::copyToClipboard()
{
    QClipboard *clip = QApplication::clipboard();
    clip->setText( m_textEdit->toPlainText() );
}
