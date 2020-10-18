#include "Network.h"

bool IpV4NetworkParticipant::Ping(IpV4NetworkParticipant participantToPing)
{
    unsigned int thisParticipantNetworkIP = ipAddress.ToInt() >> (32 - subnetMaskBits);
    unsigned int remoteParticipantNetworkIP = participantToPing.ipAddress.ToInt() >> (32 - subnetMaskBits);
    return thisParticipantNetworkIP==remoteParticipantNetworkIP;
}

bool IpV6NetworkParticipant::Ping(IpV6NetworkParticipant participantToPing)
{
    if(subnetIdBits!=participantToPing.subnetIdBits)
    {
        return false;
    }
    unsigned int HextetsToCheck = subnetIdBits / 16;
    unsigned int MaskLeftoverBits = subnetIdBits % 16;
    for(int i = 0;i<HextetsToCheck;i++)
    {
        if(ipAddress.Hextets[i] != participantToPing.ipAddress.Hextets[i])
        {
            return false;
        }
    }
    unsigned int p1Leftover = ipAddress.Hextets[HextetsToCheck] >> (16 - MaskLeftoverBits);
    unsigned int p2Leftover = participantToPing.ipAddress.Hextets[HextetsToCheck] >> (16 - MaskLeftoverBits);
    return p1Leftover==p2Leftover;
}
std::string DecimalToBinaryString(unsigned int decimal)
{
    std::string result;
    while(decimal)
    {
        result.insert(result.begin(),(decimal&1)+'0');
        decimal>>=1;
    }
    return result;
}

std::string DecimalToBinaryString(unsigned int decimal,unsigned int bitsAmount)
{
    std::string result;
    while(bitsAmount--)
    {
        result.insert(result.begin(),(decimal&1)+'0');
        decimal>>=1;
    }
    return result;
}