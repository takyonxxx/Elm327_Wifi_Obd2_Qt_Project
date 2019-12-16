#include "elm.h"

ELM::ELM()
{
}

QString ELM::get_available_pids()
{   
    QString data = "";
    return data;
}

QString ELM::AT(QString& cmd)
{
    auto data = NetworkManager::getInstance()->readData(cmd);
    return data;
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
