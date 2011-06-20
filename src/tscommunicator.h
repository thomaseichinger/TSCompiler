#ifndef TSCOMMUNICATOR_H
#define TSCOMMUNICATOR_H

#include <QObject>

class ApduViewer;

class TSCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit TSCommunicator(ApduViewer* viewer = 0, bool debug = false, QObject *parent = 0);
    ~TSCommunicator();

    void error( std::string er );
    void error( QString er );
    void out( std::string o );
    void out( QString o );

    void debugMsg( QString s );

    void setGui( ApduViewer* viewer );

    bool isDebug();
    void setDebug(bool d = true);

signals:
    void toAppend( QString );

public slots:

private:
    ApduViewer* m_viewer;
    bool m_debug;
};

#endif // TSCOMMUNICATOR_H
