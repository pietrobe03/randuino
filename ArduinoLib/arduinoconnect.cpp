#include "arduinoconnect.h"
#include "stdio.h"
#include <QObject>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>
#include <QThread>

ArduinoConnect::ArduinoConnect(QObject *parent) : QObject (parent)
{
    // Creates the SerialPort connector and configures it for arduino usage
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

ArduinoConnect::~ArduinoConnect() {
    if (sp->isOpen()) {
        sp->close();
    }
    delete sp;
}

void ArduinoConnect::CheckConnection() {
    if (connected && !SendCommand("heartbeat", 1).contains("still_alive")) {
        connected = false;
        sp->close();
        ConnectionLost();
    }
}

bool ArduinoConnect::isConnected() {
    return sp->isOpen() && sp->isWritable();
}

bool ArduinoConnect::openConnection(QString port) {
    sp->setPortName(port);
    if (sp->open(QIODevice::ReadWrite)) {
        connected = true;
        return true;
    }
    return false;
}

void ArduinoConnect::closeConnection() {
    connected = false;
    sp->close();
}

void ArduinoConnect::setSlots(ArduinoConnect::Trigger t, QObject *o, const char* s) {
    if (t == Trigger::DISCONNECT) {
        connect(this, SIGNAL(ConnectionLost()), o, s);
    }
}

QString ArduinoConnect::SerialWaitForLine(int timeout) {
    qint64 timestamp = QDateTime::currentSecsSinceEpoch();
    while ((timestamp + timeout) > QDateTime::currentSecsSinceEpoch() && !alreadyRead) {
        QThread::msleep(50);
        if (QString(read).contains('\n')) {
            alreadyRead = true;
            return QString(read).remove(QRegExp("[\\n\\t\\r]$"));
        }
    }
    return nullptr;
}

QString ArduinoConnect::SendCommand(QString command, int timeout) {
    sp->write(command.toUtf8() + "\n");
    sp->waitForBytesWritten(100);
    sp->waitForReadyRead(100);
    return SerialWaitForLine(timeout);
}

void ArduinoConnect::ReadSerial() {
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
