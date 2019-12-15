#include "elm.h"

ELM::ELM()
{
}

QString ELM::get_available_pids()
{
    if (!available_pids_checked) {
        update_available_pids();
    }
    QString data = "";
    bool first = true;
    for (int i = 0; i <= 255; i++) {
        if (available_pids[i]) {
            if (first) {first = false;} else {data += ",";}
            data += i;
        }
    }
    return data;
}

void ELM::update_available_pids()
{
    // initialize supported pid list
    for (int h = 0; h < 256; h++) {
        available_pids[h] = false;
    }
    available_pids[0] = true; // PID0 is always supported and can't be checked for support
    update_available_pidset(1);

    // Check if pid 0x20 is available (meaning next set is supported)
    if ( available_pids[0x20] ) {
        update_available_pidset(2);
        if ( available_pids[0x40] ) {
            update_available_pidset(3);
            if ( available_pids[0x60] ) {
                update_available_pidset(4);
                if ( available_pids[0x80] ) {
                    update_available_pidset(5);
                    if ( available_pids[0xA0] ) {
                        update_available_pidset(6);
                        if ( available_pids[0xC0] ) {
                            update_available_pidset(7);
                        }
                    }
                }
            }
        }
    }

    available_pids_checked = true;
}

void ELM::update_available_pidset(qint8 set)
{

    QString cmd1;

    // Select command
    switch (set) {
            case 1:
                cmd1 = "0100";
                break;
            case 2:
                cmd1 = "0120";
                break;
            case 3:
                cmd1 = "0140";
                break;
            case 4:
                cmd1 = "0160";
                break;
            case 5:
                cmd1 = "0180";
                break;
            case 6:
                cmd1 = "01A0";
                break;
            case 7:
                cmd1 = "01C0";
                break;
            default:
                cmd1 = "0100";
                break;
    }

    // Get first set of pids
    std::string seq1 = AT(cmd1).toStdString();

    if(QString(seq1.c_str()).isEmpty())
        return;

    // trim to continuous 32bit hex string
    std::string part1 = seq1.substr(6,8);
    part1 += seq1.substr(9,11);
    part1 += seq1.substr(12,14);
    part1 += seq1.substr(15,17);

    char p1char[part1.length() + 1];
    strcpy (p1char, part1.c_str());

    // convert to long
    unsigned long l1 = strtoul(p1char,nullptr,16);
    //convert to binary string
    std::string bin1 = long_to_binary(l1);
    int m = (set-1) * 32;

    // fill supported pid list
    for (int i = 0; i < (bin1.length() + 1); i++) {

        if (bin1.at(i) == '0') {
            available_pids[i+m] = false;
        } else {
            available_pids[i+m] = true;
            //Serial.print(String(i+m,DEC) + " ");//DEBUG
        }

    }
}

bool ELM::pid_available(qint8 pid) {
    if (!available_pids_checked) {
        update_available_pids();
    }
    return available_pids[pid];
}

QString ELM::AT(QString& cmd)
{
    auto data = NetworkManager::getInstance()->readData(cmd);
    return data;
}

QString ELM::get_vin() {

    auto cmd = QString("0902");
    return AT(cmd);
}

/*
 * Get the ecu name
 *
 */
QString ELM::get_ecu() {

    auto cmd = QString("090A");
    return AT(cmd);
}

/*
 * Get the cars onboard voltage
 *
 */
QString ELM::get_voltage() {

    auto cmd = QString("ATRV");
    return AT(cmd);
}


QString ELM::get_protocol() { // tested, works
    auto cmd = QString("ATDP");
    QString data = AT(cmd);
    if (data.startsWith("AUTO")) {
        data = QString(data.toStdString().substr(6).c_str());
    }
    return data;
}

QString ELM::get_dtc() {  // tested, no parsing of error codes, works
    auto cmd = QString("03");
    QString data = AT(cmd);
    if (data.startsWith("43")) {
        data = QString(data.toStdString().substr(3).c_str());
    }
    if (data.endsWith(" ")) {
        data = QString(data.toStdString().substr(0,data.length()-1).c_str());
    }
    return data;
}


bool ELM::clear_dtc() { // tested (without present dtc's), works
    auto cmd = QString("04");
    QString data = AT(cmd);
    if (AT(cmd).startsWith("44")) {
        return true;
    } else {
        return false;
    }
}
