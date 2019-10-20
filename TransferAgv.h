#ifndef TRANSFERAGV_H
#define TRANSFERAGV_H

#include "AgvBase.h"

/*!
 * @class TransferAgv
 * @brief 描述移载式AGV属性与功能的类
 */
class TransferAgv : public AgvBase
{
    Q_OBJECT
public:
    explicit TransferAgv(const AgvType& _type,const AId_t& _id,
                           const bool& _bClient,const QString& _peerAddr,const unsigned short& _peerPort,
                           const QString& _localAddr = "",const unsigned short& _localPort = 0,
                           QObject *parent = nullptr);

public:
    /*!
     * @brief 电机正转
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Reversals();

    /*!
     * @brief 电机反转
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Corotation();

    /*!
     * @brief 获取动作信息
     * @return string 动作信息
     */
    std::string GetActionName() const;
};

#endif // TRANSFERAGV_H
