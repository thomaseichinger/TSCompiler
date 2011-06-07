#ifndef TSCOMMUNICATOR_H
#define TSCOMMUNICATOR_H

#include <QObject>

class ApduViewer;

class TSCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit TSCommunicator(ApduViewer* viewer, QObject *parent = 0);
    ~TSCommunicator();

    void error( std::string er );
    void error( QString er );
    void out( std::string o );
    void out( QString o );

signals:
    void toAppend( QString );

public slots:

private:
    ApduViewer* m_viewer;

};

#endif // TSCOMMUNICATOR_H
