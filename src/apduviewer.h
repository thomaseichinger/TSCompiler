#ifndef APDUVIEWER_H
#define APDUVIEWER_H

#include <QWidget>

class QTextEdit;
class QLineEdit;

class ApduViewer : public QWidget
{
    Q_OBJECT
public:
    explicit ApduViewer(QWidget *parent = 0);
    ~ApduViewer();

    void setApduText( std::string apdu );

signals:
   // void fileSelected(QString file);

public slots:
    void copyToClipboard();
    void append(QString er);
    void openFileDialog();

private:
    QTextEdit* m_textEdit;
    QLineEdit* m_fileSel;
    bool m_apduIsSet;
    QString m_apdu;
};

#endif // APDUVIEWER_H
