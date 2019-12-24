#include "elm.h"

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

ELM* ELM::theInstance_ = nullptr;

ELM *ELM::getInstance()
{
    if (theInstance_ == nullptr)
    {
        theInstance_ = new ELM();
    }
    return theInstance_;
}

ELM::ELM()
{
}

std::vector<QString> ELM::prepareResponseToDecode(const QString &response_str)
{   
    std::vector<QString> result;
    result.reserve(8);
    std::string str(response_str.toStdString());

    size_t start=0;
    while(start < str.length()){
        std::string hex_str=str.substr(start,2);
        result.push_back(QString(hex_str.c_str()));
        start +=2;
    }
    return result;
}

std::vector<QString> ELM::decodeDTC(const std::vector<QString> &hex_vals)
{
    std::vector<QString> dtc_codes;
    QString dtc_code;
    QString tmp_code;
    for(int it=0;it<hex_vals.size();++it)
    {
        if(it%2!=0){
            dtc_code.append(hex_vals[it]);
            if(dtc_code.compare("P0000", Qt::CaseInsensitive)!=0)
                dtc_codes.push_back(dtc_code);
            tmp_code.clear();
            dtc_code.clear();
        }
        else{
            tmp_code.append(hex_vals[it]);
            char x=tmp_code.toStdString()[0];
            auto itMap = this->dtcPrefix.find(x);
            dtc_code.append((*itMap).second);
            dtc_code.append(tmp_code[1]);
        }
    }

    return dtc_codes;
}

std::pair<int,bool> ELM::decodeNumberOfDtc(const std::vector<QString> &hex_vals)
{
    int dtcNumber=0;
    bool milOn= false;
    int number= std::stoi(hex_vals[0].toStdString(),nullptr,16);
    if(number-128 < 0) {
        dtcNumber=number;
    }
    else{
        dtcNumber=number-128;
        milOn=true;
    }
    return std::make_pair(dtcNumber,milOn);
}

void ELM::resetPids()
{
    // initialize supported pid list
    for (int h = 0; h < 256; h++) {
        available_pids[h] = false;
    }
}

QString ELM::get_available_pids()
{
    update_available_pids();

    QString data = "";
    bool first = true;
    for (int i = 1; i <= 255; i++)
    {
        if (available_pids[i-1])
        {
            if (first)
            {
                first = false;
            }
            else
            {
                data.append(",");
            }

            QString hexvalue = QString("01") + QString("%1").arg(i, 2, 16, QLatin1Char( '0' ));
            data.append(hexvalue.toUpper());
        }
    }
    return data;
}

void ELM::update_available_pids()
{
    available_pids[0] = false; // PID0 is always supported and can't be checked for support
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

void ELM::update_available_pidset(quint8 set)
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
    QString flags{};
    // Get first set of pids
    //QString cmd = "4100983B0011410080108000"; jeep
    //QString cmd = "4100983B0011"; dacia
    QString cmd{};
    while(cmd.isEmpty())
    {
        cmd = ConnectionManager::getInstance()->readData(cmd1);
    }

    if(cmd.isEmpty() || cmd.contains("UNABLETOCONNECT") || cmd.contains("NODATA"))
    {
        available_pids[3]  = true;  //04  Calculated engine load
        available_pids[4]  = true;  //05  Engine coolant temperature
        available_pids[10] = true;  //0B  Intake manifold absolute pressure
        available_pids[11] = true;  //0C  Engine RPM
        available_pids[12] = true;  //0D  Vehicle speed
        available_pids[15] = true;  //0F  Maf air flow rate
        return;
    }

    auto list = cmd.split("41");
    for(auto &item: list)
    {
        QString setPart = item.mid(0,2);

        // trim to continuous 32bit hex string
        QString dataPart = item.mid(2, item.size());
        const char *str;
        QByteArray byteArray{};
        byteArray = dataPart.toLatin1();
        str = byteArray.data();
        unsigned long longData = strtoul(str,nullptr,16);
        auto binaryString = DecimalToBinaryString(longData);
        int m = (set-1) * 32;
        // fill supported pid list, ignor 0101
        for (int i = 1; i < binaryString.length(); i++)
        {
            if (binaryString[i] == '1') {
                available_pids[i+m] = true;
            }
        }
    }
}

