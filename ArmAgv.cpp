#include "ArmAgv.h"

ArmAgv::ArmAgv(const AgvType& _type,const AId_t& _id,
               const bool& _bClient,const QString& _peerAddr,const unsigned short& _peerPort,
               const QString& _localAddr,const unsigned short& _localPort,
               QObject *parent)
    : AgvBase(_type,_id,_bClient,_peerAddr,_peerPort,_localAddr,_localPort,parent)
{

}
