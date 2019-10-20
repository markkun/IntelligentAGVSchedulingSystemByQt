/*!
 * @file AgvBase
 * @brief 描述AGV基本属性信息与功能的文件
 * @date 2019-10-16
 * @author Fankaiyu
 * @version 2.0
 */
#ifndef AGVBASE_H
#define AGVBASE_H

#include <QObject>
#include <QtNetwork>
#include <QThread>
#include "ProtocolStm32.h"
#include "ProtocolPlc.h"
#include "RfidBase.h"

/*!
 * @brief 描述AGV类型信息的结构体
 * @date 2019-10-16
 */
struct AgvType
{
public:
    AgvType(const std::string& _name,ProtocolBase& _protocol,const unsigned char& _type,const float& _speed,
            const float& _weright = 0.0f,const std::string& _brand = "",const std::string& _version = "");
    AgvType(const AgvType& _type);
    ~AgvType();

public:
    operator bool() const;
    bool operator== (const bool& _bool) const;
    void operator=(const AgvType& _type);
    /*!
     * @brief 判断是否为有效类型
     * @return bool 类型参数有效返回true,否则返回false
     */
    bool IsNull() const;

public:
    std::string m_name;         /*!< 名称 */
    std::string m_brand;        /*!< 品牌 */
    std::string m_version;      /*!< 型号 */
    unsigned char m_type;       /*!< 类型 */
    float m_maxSpeed;           /*!< 最大速度:单位(m/min) */
    float m_maxWeight;          /*!< 最大载重量:单位(kg) */
    ProtocolBase* m_pProtocol;  /*!< 通信协议 */

public:
    /*! @brief 描述AGV功能的枚举 */
    enum AgvAbility
    {
        Type_Transfer = 1,   /*!< 移载式AGV */
        Type_Lifting,        /*!< 举升式AGV */
        Type_Pull,           /*!< 牵引式AGV */
        Type_Submersible,    /*!< 潜入式AGV */
        Type_Arm,            /*!< 机械臂式AGV */
        Type_Fork,           /*!< 叉车式AGV */
    };
};

/*!
 * @class AgvBase
 * @brief 描述AGV基本属性信息与功能的类
 * @date 2019-10-16
 */
class AgvBase : public QObject
{
    Q_OBJECT
public:
    typedef unsigned short AId_t;
    typedef unsigned char AMode_t;
    typedef unsigned char AStatus_t;
    typedef char ASpeed_t;
    typedef unsigned char ABattery_t;
    typedef unsigned char ACargo_t;
    typedef char AError_t;
    typedef unsigned char AAction_t;
    typedef unsigned char AActStatus_t;
    typedef unsigned char CmdErr;

public:
    explicit AgvBase(const AgvType& _type,const AId_t& _id,
                     const bool& _bClient,const QString& _peerAddr,const unsigned short& _peerPort,
                     const QString& _localAddr = "",const unsigned short& _localPort = 0,
                     QObject *parent = nullptr);
    ~AgvBase();

protected:
    const AgvType* m_pType;                             /*!< 类型 */
    AId_t m_id;                                         /*!< 编号 */

protected:
    AMode_t m_mode;                                     /*!< 模式 */
    AStatus_t m_status;                                 /*!< 状态 */
    ASpeed_t m_speed;                                   /*!< 速度:单位(%),正向移动式速度为正数，反向移动时速度为负数 */
    ABattery_t m_battery;                               /*!< 电量:单位(%) */
    ACargo_t m_cargo;                                   /*!< 载货数量 */
    AError_t m_error;                                   /*!< 异常信息,由AGV上传的异常信息 */
    AError_t m_errSelf;                                 /*!< 异常信息，系统自检测出的异常 */

protected:
    AAction_t m_action;                                 /*!< 动作 */
    AActStatus_t m_actStatus;                           /*!< 动作状态 */
    std::chrono::steady_clock::time_point m_actCount;   /*!< 动作计时器 */

protected:
    RfidBase::Rfid_t m_curRfid;                         /*!< 当前RFID地标卡编号 */
    RfidBase::Rfid_t m_oldRfid;                         /*!< 历史RFID地标卡编号 */
    RfidBase::Rfid_t m_endRfid;                         /*!< 终点RFID地标卡编号 */
    RfidBase::Rfid_t m_oldEndRfid;                      /*!< 历史终点RFID地标卡编号 */

protected:
    QTcpSocket* m_pSocket;                              /*!< 连接客户端的Socket对象指针 */
    bool m_bClient;                                     /*!< AGV网络模块的模式 */
    QString m_peerAddr;                                 /*!< AGVIP地址 */
    unsigned short m_peerPort;                          /*!< AGV端口 */
    QString m_localAddr;                                /*!< 本地IP地址 */
    unsigned short m_localPort;                         /*!< 本地端口 */
    QByteArray m_buf;                                   /*!< 接受数据的缓存区 */
    QByteArrayList m_listPacket;                        /*!< 用以储存待处理的报文 */
    QThread m_thread;                                   /*!< 用以发送数据的线程 */
    QByteArrayList m_listSend;                          /*!< 待发送的报文列表 */
    QTimer m_timer;                                     /*!< 发送报文的时间间隔 计时器 */

protected:
    /*!
     * @brief 初始化
     * @param const AgvType& AGV类型
     * @param const unsigned short& AGV编号
     * @param const bool& AGV网络模块的运行模式
     * @param const QString& AGVIP地址
     * @param const unsigned short& AGV端口
     * @param const QString& 本地IP地址
     * @param const unsigned short& 本地端口
     * @throw string 如果AGV类型为空或AGVIP地址为空,则抛出异常
     */
    void Initialize(const AgvType& _type,const AId_t& _id,
                    const bool& _bClient,const QString& _peerAddr,const unsigned short& _peerPort,
                    const QString& _localAddr = "",const unsigned short& _localPort = 0);

    /*!
     * @brief 初始化属性
     */
    void InitAttribute();

public:
    /*!
     * @brief 获取类型信息
     * @return AgvType 类型信息
     */
    AgvType GetType() const;

    /*!
     * @brief 获取编号
     * @return AId_t 编号
     */
    AId_t GetID() const;

public:
    /*!
     * @brief 更新模式状态
     * @param const AMode_t& 新的模式状态
     * @return bool 当模式发生改变时返回true,否则返回false
     */
    bool UpdateMode(const AMode_t& _mode);

    /*!
     * @brief 获取模式状态信息
     * @return AMode_t 模式状态信息
     */
    AMode_t GetMode() const;

    /*!
     * @brief 更新状态信息
     * @param const AStatus_t& 新的状态信息
     * @return bool 当状态发生改变时返回true,否则返回false
     */
    bool UpdateStatus(const AStatus_t& _status);

    /*!
     * @brief 获取状态信息
     * @return AStatus_t 状态信息
     */
    AStatus_t GetStatus() const;

    /*!
     * @brief 获取状态信息的说明
     * @return string 状态信息的说明
     */
    std::string GetStatusText() const;

    /*!
     * @brief 更新速度信息
     * @param const ASpeed_t& 新的速度信息
     * @return bool 当速度发生改变时返回true,否则返回false
     */
    bool UpdateSpeed(const ASpeed_t& _speed);

    /*!
     * @brief 获取速度信息
     * @return ASpeed_t 速度信息
     */
    ASpeed_t GetSpeed() const;

    /*!
     * @breif 获取实际速度
     * @return float 实际速度值:单位m/min
     */
    float GetActualSpeed() const;

    /*!
     * @brief 更新电量信息
     * @param const ABattery_t& 新的电量信息
     * @return bool 当电量信息发生改变时返回true,否则返回false
     */
    bool UpdateBattery(const ABattery_t& _battery);

    /*!
     * @brief 获取电量信息
     * @return ABattery_t 电量信息
     */
    ABattery_t GetBattery() const;

    /*!
     * @brief 更新载货数量信息
     * @param const ACargo_t& 新的载货数量信息
     * @return bool 当载货数量发生改变时返回true,否则返回false
     */
    bool UpdateCargo(const ACargo_t& _cargo);

    /*!
     * @brief 获取载货数量信息
     * @return ACargo_t 载货数量信息
     */
    ACargo_t GetCargo() const;

    /*!
     * @brief 更新异常信息
     * @param const AError_t& 新的异常信息
     * @return bool 当异常信息发生改变时返回true,否则返回false
     */
    bool UpdateError(const AError_t& _error);

    /*!
     * @brief 获取异常信息
     * @return AError_t 异常信息
     */
    AError_t GetError() const;

    /*!
     * @brief 获取异常信息说明
     * @param const AError_t& 异常信息
     * @return string 异常信息说明
     */
    std::string GetErrorText(const AError_t& _error);
    std::string GetErrorText();
    std::string GetSlefErrorText();

    /*!
     * @brief 更新系统自检测的异常信息
     * @Signal 当检测到异常时发出ThrowError信号
     */
    void UpdateErrorSelf(const AError_t& _error);

public:
    /*!
     * @brief 更新动作信息
     * @param const AAction_t& 新的动作信息
     * @param const AActStatus_t& 新的动作状态信息
     * @return bool 当动作信息发生改变时返回true,否则返回true
     */
    bool UpdateAction(const AAction_t& _action,const AActStatus_t& _actStatus);

    /*!
     * @brief 获取动作信息
     * @return AAction_t 动作信息
     */
    AAction_t GetAction() const;

    /*!
     * @brief 获取动作状态信息
     * @return AActStatus_t 动作状态信息
     */
    AActStatus_t GetActionStatus() const;

    /*!
     * @brief 获取动作已经执行的时间
     * @return size_t 动作执行时间:单位(ms)
     */
    size_t GetActionExecutionTime() const;

    /*!
     * @breif 设置动作已经执行的时间
     * @param size_t 动作执行时间:单位(ms)
     */
    void SetActionExecutionTime(const size_t& _time);

    /*!
     * @brief 获取动作信息
     * @param AAction_t& 动作信息
     * @param AActStatus_t& 动作状态信息
     * @return size_t 动作执行时间:单位(ms)
     */
    size_t GetAction(AAction_t& _action,AActStatus_t& _actStatus) const;

public:
    /*!
     * @brief 更新当前RFID地标卡信息
     * @param const Rfid_t& 新的当前RFID地标卡信息
     * @return bool 当RFID地标卡发生改变时返回true,否则返回false
     */
    bool UpdateCurRfid(const RfidBase::Rfid_t& _rfid);

    /*!
     * @brief 获取当期RFID地标卡信息
     * @return Rfid_t 当前RFID地标卡信息
     */
    RfidBase::Rfid_t GetCurRfid() const;

    /*!
     * @brief 获取历史RFID地标卡信息
     * @return Rfid_t 历史RFID地标卡信息
     */
    RfidBase::Rfid_t GetOldRfid() const;

    /*!
     * @brief 更新终点RFID地标卡信息
     * @param const Rfid_t& 新的终点RFID地标卡信息
     * @return bool 当RFID地标卡发生改变时返回true,否则返回false
     */
    bool UpdateEndRfid(const RfidBase::Rfid_t& _rfid);

    /*!
     * @brief 获取终点RFID地标卡信息
     * @return unsigned shortRfid_t 终点RFID地标卡信息
     */
    RfidBase::Rfid_t GetEndRfid() const;

    /*!
     * @brief 获取历史终点RFID地标卡信息
     * @return Rfid_t 历史终点RFID地标卡信息
     */
    RfidBase::Rfid_t GetOldEndRfid() const;

public:
    /*!
     * @brief 将休眠的AGV唤醒
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr WakeUp();

    /*!
     * @brief 将处于急停状态的AGV复位至待机状态
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Reset();

    /*!
     * @brief 将AGV恢复初始状态
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Restart();

    /*!
     * @brief 将AGV紧急停止
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Scream();

    /*!
     * @brief 将AGV休眠
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Sleep();

    /*!
     * @brief 将AGV正在进行的动作暂停
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Pause();

    /*!
     * @brief 将AGV恢复之前未完成的动作
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Continue();

    /*!
     * @brief 通过接入调度系统的其他设备,使AGV紧急停止(全线急停)
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr RemoteScream();

    /*!
     * @brief 将AGV关机
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Shutdown();

    /*!
     * @brief 使AGV移动至指定的RFID地标卡
     * @param const unsigned short& 指定的目的地RFID地标卡
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Move(const unsigned short& _rfid);

    /*!
     * @brief 允许AGV通过交通管制点
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr TrafficPass();

    /*!
     * @brief 调整AGV最大速度
     * @param const char& AGV的最大速度
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr SetSpeed(const char& _speed);

    /*!
     * @brief 获取指令错误的说明
     * @param const CmdErr& 错误码
     * @return string 指令的错误说明
     */
    std::string GetCmdError(const CmdErr& _err);

    /*!
     * @brief 停止动作
     * @return CmdErr 指令发生成功返回0
     */
    CmdErr StopAction();

protected:    
    /*!
     * @brief 发生心跳报文至AGV
     */
    void Heartbeat();

    /*!
     * @brief 发送状态控制报文
     * @param const unsigned char& 状态控制码
     */
    CmdErr SetStatus(const unsigned char& _cmd);

    /*!
     * @brief 使AGV在指定的位置执行动作
     * @param const unsigned char& 动作码
     * @return CmdErr 指令发送成功返回0
     */
    CmdErr Action(const unsigned char& _act);

    /*!
     * @brief 处理通信报文
     * @param const QByteArray& 待处理的报文
     */
    void ProcessPacket(const QByteArray& _packet);

protected:
    /*!
     * @brief 连接AGV
     *
     * 此时AGV网络以服务端模式运行
     */
    void Connect();

public:
    /*!
     * @brief 连接AGV
     *
     * 此时AGV网络以客户端模式运行
     * @param const QTcpSocket& 客户端Socket对象
     * @return bool 连接成功返回true,否则返回false
     */
    bool Connect(QTcpSocket& _socket);

    /*!
     * @brief 是否已连接AGV
     * @return bool 已连接返回true,否则返回false
     */
    bool IsConnected() const;

signals:
    /*!
     * @brief 当与AGV通信中断时发出此信号
     */
    void LinkBreak();

    /*!
     * @brief 当AGV更新时发出此信号
     */
    void Update();

    /*!
     * @breif 当AGV发生异常时发出此信息
     */
    void ThrowError();

protected slots:

    /*!
     * @brief 当连接服务端成功时触发的槽函数
     */
    void Connected();

    /*!
     * @brief 当服务端中断时触发的槽函数
     */
    void DisConnected();

    /*!
     * @brief 当客户端连接失败时触发的槽函数
     */
    void Error();

    /*!
     * @brief 有数据读取时触发的槽函数
     */
    void ReadData();

    /*!
     * @brief 发送报文的槽函数
     */
    void SendPacket();

protected:
    /*! @brief 描述AGV报文功能码的枚举 */
    enum AgvFunc
    {
        Func_Heartbeat = 0x1F,  /*!< 心跳 */
        Func_Move = 0x2F,       /*!< 移动控制 */
        Func_Action = 0x3F,     /*!< 动作控制 */
        Func_Traffic = 0x4F,    /*!< 交通管制控制 */
        Func_Status = 0x5F,     /*!< 状态控制 */
        Func_Speed = 0x6F,      /*!< 速度控制 */
    };

    /*! @brief 描述状态控制报文的状态码的枚举 */
    enum CmdStatus
    {
        CmdSta_Wakeup,              /*!< 唤醒 */
        CmdSta_Reset,               /*!< 复位 */
        CmdSta_Restart,             /*!< 重置 */
        CmdSta_Scream,              /*!< 急停 */
        CmdSta_Sleep,               /*!< 休眠 */
        CmdSta_Pause,               /*!< 暂停 */
        CmdSta_Continue,            /*!< 继续 */
        CmdSta_RmtScream,           /*!< 远程急停 */
        CmdSta_Shutdown = 0xFF,     /*!< 关机 */
    };
public:
    /*! @brief 描述AGV状态的枚举 */
    enum AgvStatus
    {
        Sta_Wait,           /*!< 待机状态 */
        Sta_Run,            /*!< 运行状态 */
        Sta_Stop,           /*!< 停止状态 */
        Sta_Scream,         /*!< 急停状态:人为按下AGV上的急停按键时,AGV回复此状态 */
        Sta_Find,           /*!< 寻磁状态 */
        Sta_ObsDonw,        /*!< 非接触式避障减速 */
        Sta_TrafficStop,    /*!< 交通管制停止 */
        Sta_Sleep,          /*!< 休眠状态 */
        Sta_Charging,       /*!< 充电状态 */
        Sta_RemoteScream,   /*!< 远程急停:系统通过指令使AGV急停时,AGV回复此状态 */
        Sta_AllScream,      /*!< 全线急停:使用其他设备上的急停按键使AGV急停时,AGV回复此状态 */
        Sta_SpeedUp,        /*!< 加速状态 */
        Sta_SpeedDown,      /*!< 减速状态 */
        Sta_Pause,          /*!< 暂停状态 */
    };

    /*! @brief 描述AGV异常信息的枚举 */
    enum AgvError
    {
        Err_Arm = -4,   /*!< 机械臂异常 */
        Err_Roller,     /*!< 辊筒异常 */
        Err_Lifter,     /*!< 升降杆异常 */
        Err_Net,        /*!< 网络异常 */
        Err_None,       /*!< 无异常 */
        Err_Miss,       /*!< 脱磁异常 */
        Err_Obs,        /*!< 非接触式避障停止 */
        Err_Mobs,       /*!< 接触式避障停止 */
    };

    /*! @brief 描述AGV模式的枚举 */
    enum AgvMode
    {
        Mode_Hand,  /*!< 手动模式 */
        Mode_Auto,  /*!< 自动模式 */
    };

    /*! @brief 描述AGV动作状态的枚举 */
    enum AgvActStatus
    {
        ActSta_None,    /*!< 无动作/未执行动作/动作停止 */
        ActSta_Exe,     /*!< 正在执行动作 */
        ActSta_Fin,     /*!< 动作执行完成 */
    };

    /*! @brief 描述AGV指令返回值的枚举 */
    enum CmdError
    {
        Cmd_Success,    /*!< 指令发送成功 */
        Cmd_StatusErr,  /*!< 状态不符合 */
        Cmd_NetErr,     /*!< 网络错误 */
        Cmd_ActionErr,  /*!< 动作错误 */
        Cmd_ParamErr,   /*!< 参数错误 */
        //Cmd_SpeedErr,   /*!< 速度错误 */
    };
};

#endif // AGVBASE_H
