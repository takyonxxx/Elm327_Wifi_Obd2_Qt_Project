#include <serial.h>
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

using namespace std;

Serial::Serial(char* dev, int baud, int parity, int blocking, QObject *parent)
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
}

Serial::~Serial()
{
    closePort();
}

bool Serial::isConnected() const
{
    return _connected;
}

int Serial::initPort()
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

        if (tcsetattr(_fd, TCSANOW, &options) == -1)
            return -1;

        tcflush(_fd, TCIOFLUSH);
        qDebug() << "SERIAL: Connected " << _dev << " at " << _baud << " baud..." << endl;
        _connected = true;
    }
    else
    {
        closePort();
        _connected = false;
    }

    return _fd;
}

void Serial::flushPort()
{
    if (_fd != -1) ioctl(_fd, TCIOFLUSH, 2);
}

int Serial::getData(char* buffer)
{
    if (_fd != -1)
    {
        int  nbytes{0};

        while ((nbytes = read(_fd, buffer, 32)) > 0)
        {

            buffer += nbytes;
            if (buffer[-1] == '\r' || buffer[-1] == '>')
            {
                return nbytes;
            }
        }
    }
    else
    {
        return -1;
    }
}

int Serial::sendData(char* data)
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

void Serial::closePort()
{
    if (_fd != -1)
    {
        close(_fd);
        cout << "SERIAL: Device " << _dev << " is now closed." << endl;
    }
}

int Serial::getBaud() const {
    return _baud;
}

void Serial::setBaud(int baud) {
    this->_baud = baud;
}

int Serial::getBlocking() const {
    return _blocking;
}

void Serial::setBlocking(int blocking) {
    this->_blocking = blocking;
}

char* Serial::getDev() const {
    return _dev;
}

void Serial::setDev(char* dev) {
    this->_dev = dev;
}

int Serial::getParity() const {
    return _parity;
}

void Serial::setParity(int parity) {
    this->_parity = parity;
}
