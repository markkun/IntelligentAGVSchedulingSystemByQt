#ifndef FORKAGV_H
#define FORKAGV_H

#include "LiftingAgv.h"


class ForkAgv : public LiftingAgv
{
    Q_OBJECT
public:
   explicit ForkAgv(const AgvType& _type,const AId_t& _id,
                    const bool& _bClient,const QString& _peerAddr,const unsigned short& _peerPort,
                    const QString& _localAddr = "",const unsigned short& _localPort = 0,
                    QObject *parent = nullptr);

public:
    /*!
     * @brief 获取动作信息
     * @return string 动作信息
     */
    std::string GetActionName() const;
};

#endif // FORKAGV_H
