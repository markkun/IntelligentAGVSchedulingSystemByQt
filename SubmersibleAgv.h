#ifndef SUBMERSIBLEAGV_H
#define SUBMERSIBLEAGV_H

#include "AgvBase.h"

/*!
 * @class SubmersibleAgv
 * @brief 描述潜入式AGV属性和功能的类
 */
class SubmersibleAgv : public AgvBase
{
    Q_OBJECT
public:
    explicit SubmersibleAgv(const AgvType& _type,const AId_t& _id,
                   const bool& _bClient,const QString& _peerAddr,const unsigned short& _peerPort,
                   const QString& _localAddr = "",const unsigned short& _localPort = 0,
                   QObject *parent = nullptr);

public:
    /*!
     * @brief 升降杆上升
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Up();

    /*!
     * @brief 升降杆下降
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Down();

    /*!
     * @brief 获取动作信息
     * @return string 动作信息
     */
    std::string GetActionName() const;
};

#endif // SUBMERSIBLEAGV_H
