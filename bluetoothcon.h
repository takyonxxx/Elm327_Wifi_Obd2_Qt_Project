#ifndef BLUETOOTHCON_H
#define BLUETOOTHCON_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

using namespace std;

class BluetoothCon
{
public:
    BluetoothCon();
};

#endif // BLUETOOTHCON_H
