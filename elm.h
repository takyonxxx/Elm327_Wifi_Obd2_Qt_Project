#ifndef ELM_H
#define ELM_H
#include <QtCore>
#include <string>
#include "networkmanager.h"
#include "bluetoothmanager.h"

class ELM
{
public:
    ELM();
    QString get_available_pids();    
    QString AT(QString &);
    std::vector<QString> decodeDTC(const std::vector<QString> &hex_vals);
    std::pair<int,bool> decodeNumberOfDtc(const std::vector<QString> &hex_vals);
    std::vector<QString> prepareResponseToDecode(const QString &response_str);

    std::string DecimalToBinaryString(unsigned long a)
    {
        uint b = static_cast<uint>(a);
        std::string binary = "";
        uint mask = 0x80000000u;
        while (mask > 0)
        {
            binary += ((b & mask) == 0) ? '0' : '1';
            mask >>= 1;
        }
        return binary;
    }

private:
    bool available_pids[256];
    bool available_pids_checked = false;
    void update_available_pids();
    void update_available_pidset(quint8 set);
    std::map<char,QString> dtcPrefix={{'0',QString("P0")},{'1',QString("P1")},{'2',QString("P2")},{'3',QString("P3")},
                                      {'4',QString("C0")},{'5',QString("C1")},{'6',QString("C2")},{'7',QString("C3")},
                                      {'8',QString("B0")},{'9',QString("B1")},{'A',QString("B2")},{'B',QString("B3")},
                                      {'C',QString("U0")},{'D',QString("U1")},{'E',QString("U2")},{'F',QString("U3")}
                                     };

};

#endif // ELM_H
