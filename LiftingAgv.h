#ifndef LIFTINGAGV_H
#define LIFTINGAGV_H

#include "SubmersibleAgv.h"

class LiftingAgv : public SubmersibleAgv
{
    Q_OBJECT
public:
    explicit LiftingAgv(const AgvType& _type,const AId_t& _id,
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

#endif // LIFTINGAGV_H
