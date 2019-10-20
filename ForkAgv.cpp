#include "ForkAgv.h"

ForkAgv::ForkAgv(const AgvType& _type,const AId_t& _id,
                   const bool& _bClient,const QString& _peerAddr,const unsigned short& _peerPort,
                   const QString& _localAddr,const unsigned short& _localPort,
                   QObject *parent)
    : LiftingAgv(_type,_id,_bClient,_peerAddr,_peerPort,_localAddr,_localPort,parent)
{

}

std::string ForkAgv::GetActionName() const
{
    std::string _act= "unknown-",_actStatus = "unknown";

    switch(m_action)
    {
    case 0:
        _act= "无动作-";
        break;
    case 1:
        _act= "载货-";
        break;
    case 2:
        _act= "卸货-";
        break;
    default:
        _act= "unknown-";
        break;
    }

    switch (m_actStatus)
    {
    case ActSta_None:
        _actStatus = "未执行";
        break;
    case ActSta_Exe:
        _actStatus = "正在执行";
        break;
    case ActSta_Fin:
        _actStatus = "已执行完成";
        break;
    default:
        _actStatus = "unknown";
        break;
    }

    return _act + _actStatus;
}
