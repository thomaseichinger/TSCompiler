#include "tscommunicator.h"
#include "apduviewer.h"
#include "iostream"

TSCommunicator::TSCommunicator(ApduViewer* viewer, QObject *parent) :
    QObject(parent),
    m_viewer(viewer)
{
    connect(this,SIGNAL(toAppend(QString)),m_viewer,SLOT(append(QString)));
}

TSCommunicator::~TSCommunicator()
{
}

void TSCommunicator::error(std::string er)
{
    TSCommunicator::error( QString(er.c_str()) );
}

void TSCommunicator::error(QString er)
{
    QString error;
    error.append( "-- ERROR: " ).append( er );
    std::cout << error.toStdString() << std::endl;
    emit TSCommunicator::toAppend( error );
}

void TSCommunicator::out(std::string o)
{
    TSCommunicator::out( QString(o.c_str()) );
}

void TSCommunicator::out(QString o)
{
    QString out;
    out.append( "-- " ).append( o );
    std::cout << out.toStdString() << std::endl;
    emit TSCommunicator::toAppend( out );
}
