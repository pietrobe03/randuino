#ifndef RANDUINOCONNECT_H
#define RANDUINOCONNECT_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class RanduinoConnect : public QObject
{
    Q_OBJECT

public:
    explicit RanduinoConnect(QObject *parent = nullptr);
    ~RanduinoConnect();

    enum Trigger {DISCONNECT};

    bool isConnected();
    bool openConnection(QString port);
    void closeConnection();
    void setSlots(Trigger t, QObject *o, const char* s);

    QString SendCommand(QString command, int timeout = 2);


private slots:
    QString SerialWaitForLine(int timeout = 2);
    void ReadSerial();
    void CheckConnection();

private:
    bool connected;
    bool alreadyRead;
    QByteArray read;
    QTimer *ti;
    QSerialPort *sp;

signals:
    void ConnectionLost();
};

#endif // ARDUINOCONNECT_H
