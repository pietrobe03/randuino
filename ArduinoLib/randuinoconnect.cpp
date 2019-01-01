/* 
 * RanduinoConnect
 * A library for randuino connection and a request-reply client.
 * Also made for participating at Jugend Forscht 2019
 *
 * (c) 2018-2019 Robert Pietsch
 */

#include "randuinoconnect.h"
#include "stdio.h"
#include <QObject>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>
#include <QThread>

/*
 * public Constructor
 * @param parent: Usually a nullptr, required due to being a subclass of QObject
 */
RanduinoConnect::RanduinoConnect(QObject *parent) : QObject (parent)
{
    // Creates the SerialPort connector and configures it for Randuino usage
    sp = new QSerialPort();
    sp->setBaudRate(QSerialPort::Baud9600);
    sp->setDataBits(QSerialPort::Data8);
    sp->setParity(QSerialPort::NoParity);
    sp->setStopBits(QSerialPort::OneStop);
    sp->setFlowControl(QSerialPort::NoFlowControl);

    // Creates the timer for periodic actions
    ti = new QTimer();
    ti->setInterval(1000);
    ti->setSingleShot(false);
    ti->start();
    connect(ti, SIGNAL(timeout()), this, SLOT(CheckConnection()));

    // Sets the reference value for "connection established" to false
    connected = false;

    // Creates the listener for incoming serial data
    QObject::connect(sp, SIGNAL(readyRead()), this, SLOT(ReadSerial()));
}

/*
 * public Destructor
 * Closes an open connection and destroys the QSerialPort instance
 */
RanduinoConnect::~RanduinoConnect() {
    if (sp->isOpen()) {
        sp->close();
    }
    delete sp;
}

/*
 * private CheckConnection
 * Internal method for checking if an established connection is still open
 * Triggers the ConnectionLost signal if connection is closed unexpectedly
 * This method is called periodically.
 */
void RanduinoConnect::CheckConnection() {
    if (connected && !SendCommand("heartbeat", 1).contains("still_alive")) {
        connected = false;
        sp->close();
        ConnectionLost();
    }
}

/*
 * public isConnected
 * Checks if a connection is established
 * @return Connection state
 */
bool RanduinoConnect::isConnected() {
    return sp->isOpen() && sp->isWritable();
}

/*
 * public openConnection
 * Establishes a connection to the given port
 * @param port: System address of the port (e.g. \\.\COM14 or /dev/tty12)
 * @return Connection success
 */
bool RanduinoConnect::openConnection(QString port) {
    sp->setPortName(port);
    if (sp->open(QIODevice::ReadWrite)) {
        connected = true;
        return true;
    }
    return false;
}

/*
 * public closeConnection
 * Closes an open connection
 */
void RanduinoConnect::closeConnection() {
    if (isConnected()) {
        connected = false;
    }
    sp->close();
}

/*
 * public setSlots
 * Connects an event to a method
 * @param t: Trigger (DISCONNECT) [More triggers will be added]
 * @param o: The object instance containing the method to call at the event
 * @param s: The method to call at the event
 */
void RanduinoConnect::setSlots(RanduinoConnect::Trigger t, QObject *o, const char* s) {
    if (t == Trigger::DISCONNECT) {
        connect(this, SIGNAL(ConnectionLost()), o, s);
    }
}

/*
 * private SerialWaitForLine
 * Waits for incoming data terminated by '\n' on the serialport
 * @param timeout: Timeout in seconds
 * @returns: The received data (without \n) or nullptr if timeout is reached
 */
QString RanduinoConnect::SerialWaitForLine(int timeout) {
    qint64 timestamp = QDateTime::currentSecsSinceEpoch();
    while ((timestamp + timeout) > QDateTime::currentSecsSinceEpoch() && !alreadyRead) {
        QThread::msleep(50);
        if (QString(read).contains('\n')) {
            alreadyRead = true;
            return QString(read).remove(QRegExp("[\\n]$"));
        }
    }
    return nullptr;
}

/*
 * public SendCommand
 * Sends a command to the randuino and returns the answer
 * @param command: The command to send to the randuino
 * @param timeout: Timeout in seconds
 * @returns: The answer of the randuino or nullptr if the timeout is reached
 */
QString RanduinoConnect::SendCommand(QString command, int timeout) {
    sp->write(command.toUtf8() + "\n");
    sp->waitForBytesWritten(100);
    sp->waitForReadyRead(100);
    return SerialWaitForLine(timeout);
}

/*
 * private ReadSerial
 * Writes incoming data from randuino to a buffer. If the data ends with '\n' the 
 * alreadyRead flag is removed and the data is copied to read.
 * This method is called periodically.
 */
void RanduinoConnect::ReadSerial() {
    static QByteArray readTemp;
    alreadyRead = false;
    readTemp += sp->readAll();
    if (readTemp.endsWith('\n')) {
        read = readTemp.data();
        readTemp.clear();
    } else {
        read = readTemp.data();
    }
}
