#include <sstream>
#include <array>
#include <regex>

#ifndef OOPLAB1_NETWORK_H
#define OOPLAB1_NETWORK_H



std::string DecimalToBinaryString(unsigned int decimal);
std::string DecimalToBinaryString(unsigned int decimal,unsigned int bitsAmount);

struct IpV4
{
    unsigned char Octets[4];

    unsigned int ToInt()
    {
        unsigned int result = 0;
        for(unsigned char Octet : Octets)
        {
            result<<=8;
            result+=Octet;
        }
        return result;
    }

    IpV4()
    {

    }

    explicit IpV4(const std::string& address)
    {
        std::regex expression("([0-9]{1,3}\\.){3}[0-9]{1,3}");
        if(!std::regex_match(address, expression))
        {
            for(auto& octet:Octets)
            {
                octet = 0;
            }
        }
        else
        {
            std::string addressCopy = address;
            int currOctetPos = 0;
            std::size_t currDelimPos;
            std::string delimiter = ".";
            std::string currOctet;
            while ((currDelimPos = addressCopy.find(delimiter)) != std::string::npos) {
                currOctet = addressCopy.substr(0, currDelimPos);
                addressCopy.erase(0, currDelimPos + delimiter.length());
                Octets[currOctetPos++] = std::stoi(currOctet);
            }
            Octets[currOctetPos] = std::stoi(addressCopy);
        }
    }
};

struct IpV6
{
    unsigned int Hextets[8];

    IpV6()
    {
        for(auto& hextet:Hextets)
        {
            hextet = 0;
        }
    }

    explicit IpV6(const std::string& address)
    {
        std::regex expression("([a-zA-Z0-9]{4}:){7}[a-zA-Z0-9]{4}");
        std::array<std::string,8> result;
        if(!std::regex_match(address, expression))
        {
            for(auto& hextet:Hextets)
            {
                hextet = 0;
            }
        }
        else
        {
            std::string addressCopy = address;
            int currHextetPos = 0;
            std::size_t currDelimPos;
            std::string delimiter = ":";
            std::string currHextet;
            while ((currDelimPos = addressCopy.find(delimiter)) != std::string::npos) {
                currHextet = addressCopy.substr(0, currDelimPos);
                addressCopy.erase(0, currDelimPos + delimiter.length());
                result[currHextetPos++] = currHextet;
            }
            result[currHextetPos] = addressCopy;
        }
        IpV6 temp = stringsArrayToIPv6(result);
        std::copy(std::begin(temp.Hextets),std::end(temp.Hextets),Hextets);
    }

    explicit IpV6(const std::array<std::string,8>& HextetsData)
    {
        IpV6 temp = stringsArrayToIPv6(HextetsData);
        std::copy(std::begin(temp.Hextets),std::end(temp.Hextets),Hextets);
    }

private:
    IpV6 stringsArrayToIPv6(const std::array<std::string,8>& array)
    {
        IpV6 result;
        std::stringstream ss;
        for(int i = 0;i<8;i++)
        {
            unsigned int x;
            ss<<std::hex<<array[i];
            ss>>x;
            result.Hextets[i] = static_cast<int>(x);
            ss.clear();
        }
        return result;
    }
};


class IpV4NetworkParticipant
{
public:
    bool Ping(IpV4NetworkParticipant participantToPing);
    explicit IpV4NetworkParticipant(std::string cidrAddress)
    {
        std::regex expression("([0-9]{1,3}\\.){3}[0-9]{1,3}/[0-9]{1,2}");
        if(std::regex_match(cidrAddress, expression))
        {
            size_t cidrNotationStart = cidrAddress.find('/');
            ipAddress = IpV4(cidrAddress.substr(0,cidrNotationStart));
            subnetMaskBits = stoi(cidrAddress.substr(cidrNotationStart+1,
                                                   cidrAddress.size()-cidrNotationStart));
        }
    }
    IpV4NetworkParticipant()
    {
        ipAddress = IpV4{{0,0,0,0}};
        subnetMaskBits = 0;
    }
    IpV4NetworkParticipant(IpV4 address, int subnetBits): ipAddress(address), subnetMaskBits(subnetBits)
    {
    }
private:
    IpV4 ipAddress;
    int subnetMaskBits;
};

class IpV6NetworkParticipant
{
public:
    IpV6NetworkParticipant()
    {
        ipAddress = IpV6();
        subnetIdBits = 0;
    }
    IpV6NetworkParticipant(const std::string& cidrAddress)
    {
        std::regex expression("([a-zA-Z0-9]{4}:){7}[a-zA-Z0-9]{4}/[0-9]{1,3}");
        if(std::regex_match(cidrAddress, expression))
        {
            size_t cidrNotationStart = cidrAddress.find('/');
            ipAddress = IpV6(cidrAddress.substr(0,cidrNotationStart));
            subnetIdBits = stoi(cidrAddress.substr(cidrNotationStart+1,
                                                   cidrAddress.size()-cidrNotationStart));
        }
    }
    IpV6NetworkParticipant(const std::array<std::string,8>& Hextets,int subnetPrefixSize):subnetIdBits(subnetPrefixSize)
    {
        ipAddress = IpV6(Hextets);
    }
    bool Ping(IpV6NetworkParticipant participantToPing);
private:
    IpV6 ipAddress;
    int subnetIdBits;
};


#endif //OOPLAB1_NETWORK_H
