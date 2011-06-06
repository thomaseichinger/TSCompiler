#ifndef APDUVIEWER_H
#define APDUVIEWER_H

#include <QWidget>

class QTextEdit;

class ApduViewer : public QWidget
{
    Q_OBJECT
public:
    explicit ApduViewer(QWidget *parent = 0);
    ~ApduViewer();

    void setApduText( std::string apdu );

signals:

public slots:
    void copyToClipboard();

private:
    QTextEdit* m_textEdit;

};

#endif // APDUVIEWER_H
