#include <elmserialport.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <paths.h>
#include <termios.h>
#include <sysexits.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <ctype.h>
#include <QDebug>

using namespace std;

ElmSerialPort::ElmSerialPort(QObject *parent)
{

}

ElmSerialPort::~ElmSerialPort()
{
    disconnectSerial();
    delete _dev;
    delete _buffer;
    delete _temp;
}

bool ElmSerialPort::connectSerial(const QString &port)
{
    QString msg{};
    msg.append("Connecting to Serial " + port);
    emit stateChanged(msg);

    return doConnect((char*)port.toStdString().c_str(), 38400, PARITY_8N1, 0);
}

bool ElmSerialPort::doConnect(char* dev, int baud, int parity, int blocking)
{
    _dev = dev;
    _baud = baud;
    _parity = parity;
    _blocking = blocking;
    _dataBits = 8;
    _bufferSize = 1000;
    _buffer = new char[_bufferSize];
    _temp = new char [_bufferSize];
    _bufferIndex = 0;

    return initPort();
}

void ElmSerialPort::disconnectSerial()
{
    if (_fd != -1)
    {
        close(_fd);
        emit serialDisConnected();
    }
}

bool ElmSerialPort::send(const QString &command)
{
    QByteArray cmd = QString(command + "\r\0").toLocal8Bit();
    char *sendCmd = cmd.data();
    if(isConnected())
        {
            return sendData(sendCmd);
        }
        else
            return false;
}

QString ElmSerialPort::readData(const QString &command)
{
    QString strData{};

    if(isConnected() && send(command))
    {
        strData = getData();
        strData.remove("\r");
        strData.remove(">");
        strData = strData.trimmed()
                .simplified()
                .remove(QRegExp("[\\n\\t\\r]"))
                .remove(QRegExp("[^a-zA-Z0-9]+"));

        // Some of these look like errors that ought to be handled..
        strData.replace("?","");
        strData.replace(",","");

        if(strData.isEmpty())
            strData = QString("NODATA");

        emit dataReceived(strData);
    }

    return strData;
}


int ElmSerialPort::initPort()
{
    _fd = open(_dev, O_RDWR | O_NOCTTY | O_NDELAY | O_SYNC);

    if (_fd != -1)
    {
        fcntl(_fd, F_SETFL, FNDELAY);
        struct termios options;
        tcgetattr (_fd, &options);

        switch (_baud)
        {
        case 4800:
            cfsetispeed(&options, B4800);
            cfsetospeed(&options, B4800);
            break;
        case 9600:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            break;
        case 38400:
            cfsetispeed(&options, B38400);
            cfsetospeed(&options, B38400);
            break;
        case 57600:
            cfsetispeed(&options, B57600);
            cfsetospeed(&options, B57600);
            break;
        case 115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            break;
        default:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            break;
        }

        /* 8 data bits */
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;

        /* No parity */
        options.c_cflag &= ~PARENB;

        /* 1 stop bit */
        options.c_cflag &= ~CSTOPB;

        /* Turn off flow control */
        options.c_iflag &= ~(IXON | IXOFF);

        /* Enable newline as carriage return */
        options.c_iflag |= INLCR;

        /* Disable implementation defined output processing */
        options.c_oflag &= ~OPOST;

        /* Do not echo input */
        options.c_lflag &= ~ECHO;

        options.c_cc[VTIME] = 0; 	/* Byte and byte timeout is 0s, when first byte is read  */
        options.c_cc[VMIN] = 0;    /* At least have 0 byte data then return or will block call read process */

        tcflush(_fd, TCIOFLUSH);

        if (tcsetattr(_fd, TCSANOW, &options) == -1)
            return -1;

        _connected = true;
        usleep(10000);
        emit serialConnected();
    }
    else
    {
        disconnectSerial();
        _connected = false;
        emit serialDisConnected();
    }

    return _fd;
}

void ElmSerialPort::flushPort()
{
    if (_fd != -1) ioctl(_fd, TCIOFLUSH, 2);
}

int ElmSerialPort::sendData(char* data)
{
    int result;
    if (_fd != -1)
    {
        auto rt = write(_fd, data, strlen(data));
        usleep ((strlen(data) + 25) * 100);
        return rt;
    }
    else
        return -1;
}

int ElmSerialPort::getBaud() const {
    return _baud;
}

void ElmSerialPort::setBaud(int baud) {
    this->_baud = baud;
}

int ElmSerialPort::getBlocking() const {
    return _blocking;
}

void ElmSerialPort::setBlocking(int blocking) {
    this->_blocking = blocking;
}

char* ElmSerialPort::getDev() const {
    return _dev;
}

void ElmSerialPort::setDev(char* dev) {
    this->_dev = dev;
}

int ElmSerialPort::getParity() const {
    return _parity;
}

void ElmSerialPort::setParity(int parity) {
    this->_parity = parity;
}

bool ElmSerialPort::isConnected() const
{
    return _connected;
}

QString ElmSerialPort::getData()
{
    QString returnData{};

    int                    char_idx;
    char                   c, prev, buf[32] = {0};
    fd_set                 recv_fds;
    struct timeval         timeout;

    /* Wait until we find some data on the line */
    if (elm327_timeout_seconds > 0)
    {
        FD_ZERO(&recv_fds);
        FD_SET(_fd, &recv_fds);
        timeout = (struct timeval){elm327_timeout_seconds, 0};
        if (select(_fd + 1, &recv_fds, nullptr, nullptr, &timeout) <= 0)
          return returnData;
    }

    /* Recieve the data */
    prev = 0;
    char_idx = 0;
    while ((read(_fd, &c, 1) > 0) && (char_idx < sizeof(buf)))
    {
        if (c == '>')
          break;
        else if ((prev == '\n') && (c == '\n'))
          break;

        buf[char_idx++] = c;
        prev = c;
    }

    returnData = QString(buf);
    flushPort();
    return returnData;
}
