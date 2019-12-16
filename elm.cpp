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

    std::replace(str.begin(),str.end(),'>',' ');
    size_t pos = str.find("\r\r",0);
    if(pos!=std::string::npos)
        str.resize(pos);
    if(str[0]=='A' && str[1]=='T'){
        result.push_back(response_str);
    }

    std::replace(str.begin(),str.end(),'\r',' ');
    if(str.find("NO DATA",0)!=std::string::npos){
        //std::cout<<"NODATA";
        result.push_back(QString("NO DATA"));
        return result;
    }

    pos=str.find("SEARCHING",0);
    if(pos!=std::string::npos){
        //usunac searching
        str=str.substr(pos+9,str.length()-pos);
        pos=str.find_last_of(".",0);
        if(pos!=std::string::npos){
            str=str.substr(pos);
        }
        std::replace(str.begin(),str.end(),'.',' ');
    }

    int counter=0;
    size_t start=0;
    while(str.length()>=2){
        //std::cout<<str[0]<<std::endl;
        if(str[0]!=' '){
            std::string hex_str=str.substr(0,2);
            result.push_back(QString(hex_str.c_str()));
            //std::cout<<"hexstr:"<<hex_str<<std::endl;
            int i_hex = std::stoi(hex_str,nullptr,16);
            //  std::cout<<"hexval:"<<i_hex<<std::endl;
        }
        start=str.find(" ",0);
        if(start==std::string::npos)
            str="";
        else
            str=str.substr(start+1);
        //std::cout<<"new str: "<<str<<std::endl;
        ++counter;
        if(counter>100)
            break;
    }
    return result;
}

std::vector<QString> ELM::decodeDTC(const std::vector<QString> &hex_vals)
{
    /*
43 12 29 03 80 00 00

12 29 03 80=>
P1229
P0	380
*/
    std::vector<QString> dtc_codes;
    QString dtc_code;
    if(hex_vals.size()>0 && !hex_vals[0].compare("43", Qt::CaseInsensitive)){
        QString tmp_code;
        for(int it=1;it<hex_vals.size();++it){
            if(it%2!=0){
                tmp_code.append(hex_vals[it]);
                char x=tmp_code.toStdString()[0];
                auto itMap = this->dtcPrefix.find(x);
                dtc_code.append((*itMap).second);
                dtc_code.append(tmp_code[1]);
            }
            else{
                dtc_code.append(hex_vals[it]);
                if(dtc_code.compare("P0000", Qt::CaseInsensitive)!=0)
                    dtc_codes.push_back(dtc_code);
                tmp_code.clear();
                dtc_code.clear();
            }
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
