#ifndef APDUVIEWER_H
#define APDUVIEWER_H

#include <QWidget>
#include <QString>

class QTextEdit;
class QLineEdit;
class TSCommunicator;

class ApduViewer : public QWidget
{
    Q_OBJECT
public:
    explicit ApduViewer(TSCommunicator* com, QWidget *parent = 0);
    ~ApduViewer();

    void setApduText( QString apdu );

signals:

public slots:
    void copyToClipboard();
    void append(QString er);
    void showFileDialog();
    void run();
    void setFileName( QString );

private:
    QTextEdit* m_textEdit;
    QLineEdit* m_fileSel;
    TSCommunicator* m_com;
    bool m_apduIsSet;
    QString m_apdu;
};

#endif // APDUVIEWER_H
