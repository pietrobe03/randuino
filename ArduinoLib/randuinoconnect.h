#ifndef RANDUINOCONNECT_H
#define RANDUINOCONNECT_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class RanduinoConnect : public QObject
{
    Q_OBJECT

public:
    // Constructor, Destructor
    explicit RanduinoConnect(QObject *parent = nullptr);
    ~RanduinoConnect();

    // Options for event triggers [More will be added soon]
    enum Trigger {DISCONNECT};

    // Public methods for randuino communication
    bool isConnected();
    bool openConnection(QString port);
    void closeConnection();
    void setSlots(Trigger t, QObject *o, const char* s);
    QString SendCommand(QString command, int timeout = 2);

private slots:
    
    QString SerialWaitForLine(int timeout = 2);
    
    // Periodically called methods
    void ReadSerial();
    void CheckConnection();

private:
    bool connected;     // Flag showing if a connection should currently be established
    bool alreadyRead;   // Flag showing if the contents stored in read are new or already read
    QByteArray read;    // Incoming data until last terminating character ('\n')
    QTimer *ti;         // Timer for periodical actions
    QSerialPort *sp;    // Handle for serial port connection

signals:
    // Event signals
    void ConnectionLost();
};

#endif // RANDUINOCONNECT_H
