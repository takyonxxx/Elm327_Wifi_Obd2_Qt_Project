#ifndef ELM_H
#define ELM_H
#include <QtCore>
#include <string>
#include "networkmanager.h"

class ELM
{
public:
    ELM();
    QString get_available_pids();
    QString AT(QString&);
    bool pid_available(qint8 pid);
    QString get_vin();
    QString get_ecu();
    QString get_voltage();
    QString get_protocol();
    QString get_dtc();
    bool clear_dtc();

private:    
    void update_available_pids();
    void update_available_pidset(qint8 set);
    bool available_pids[256];
    bool available_pids_checked = false;
    char* long_to_binary(unsigned long k)
    {
        static char c[65];
        c[0] = '\0';

        unsigned long val;
        for (val = ULONG_MAX; val > 0; val >>= 1)
        {
            strcat(c, ((k & val) == val) ? "1" : "0");
        }
        return c;
    }
};

#endif // ELM_H
