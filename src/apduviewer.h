#ifndef APDUVIEWER_H
#define APDUVIEWER_H

#include <QWidget>
#include <QString>

class QTextEdit;

class ApduViewer : public QWidget
{
    Q_OBJECT
public:
    explicit ApduViewer(QWidget *parent = 0);
    ~ApduViewer();

    void setApduText( QString apdu );

signals:

public slots:
    void copyToClipboard();
    void append(QString er);

private:
    QTextEdit* m_textEdit;
    bool m_apduIsSet;
    QString m_apdu;
};

#endif // APDUVIEWER_H
