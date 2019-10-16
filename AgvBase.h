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
#include <chrono>

/*!
 * @brief 描述AGV类型信息的结构体
 * @date 2019-10-16
 */
struct AgvType
{
public:
    AgvType();
    AgvType(const std::string& _name,const unsigned char& _type,const float& _speed,const float& _weright = 0.0f,const std::string& _brand = "",const std::string& _version = "");
    AgvType(const AgvType& _type);

public:
    operator bool();

public:
    std::string m_name;         /*!< 名称 */
    std::string m_brand;        /*!< 品牌 */
    std::string m_version;      /*!< 型号 */
    unsigned char m_type;       /*!< 类型 */
    float m_maxSpeed;           /*!< 最大速度:单位(m/min) */
    float m_maxWeight;          /*!< 最大载重量:单位(kg) */
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
    explicit AgvBase(const AgvType* _type,const unsigned short& _id,QObject *parent = nullptr);

protected:
    const AgvType* m_pType;                             /*!< 类型 */
    unsigned short m_id;                                /*!< 编号 */

protected:
    unsigned char m_mode;                               /*!< 模式 */
    unsigned char m_status;                             /*!< 状态 */
    char m_speed;                                       /*!< 速度:单位(%),正向移动式速度为正数，反向移动时速度为负数 */
    unsigned char m_battery;                            /*!< 电量:单位(%) */
    unsigned char m_cargo;                              /*!< 载货数量 */

protected:
    unsigned char m_action;                             /*!< 动作 */
    unsigned char m_actStatus;                          /*!< 动作状态 */
    std::chrono::steady_clock::time_point m_actCount;   /*!< 动作计时器 */

protected:
    unsigned short m_curRfid;                           /*!< 当前RFID地标卡编号 */
    unsigned short m_oldRfid;                           /*!< 历史RFID地标卡编号 */
    unsigned short m_endRfid;                           /*!< 终点RFID地标卡编号 */
    unsigned short m_oldEndRfid;                        /*!< 历史终点RFID地标卡编号 */

protected:
    /*!
     * @brief 初始化
     * @param const AgvType& AGV类型
     * @param const unsigned short& AGV编号
     */
    void Initialize(const AgvType* _type,const unsigned short& _id);

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
     * @return unsigned short 编号
     */
    unsigned short GetID() const;

public:
    /*!
     * @brief 更新模式状态
     * @param const unsigned char& 新的模式状态
     * @return bool 当模式发生改变时返回true,否则返回false
     */
    bool UpdateMode(const unsigned char& _mode);

    /*!
     * @brief 获取模式状态信息
     * @return unsigned char 模式状态信息
     */
    unsigned char GetMode() const;

    /*!
     * @brief 更新状态信息
     * @param const unsigned char& 新的状态信息
     * @return bool 当状态发生改变时返回true,否则返回false
     */
    bool UpdateStatus(const unsigned char& _status);

    /*!
     * @brief 获取状态信息
     * @return unsigned char 状态信息
     */
    unsigned char GetStatus() const;

    /*!
     * @brief 获取状态信息的说明
     * @return string 状态信息的说明
     */
    std::string GetStatusText() const;

    /*!
     * @brief 更新速度信息
     * @param const char& 新的速度信息
     * @return bool 当速度发生改变时返回true,否则返回false
     */
    bool UpdateSpeed(const char& _speed);

    /*!
     * @brief 获取速度信息
     * @return char 速度信息
     */
    char GetSpeed() const;

    /*!
     * @breif 获取实际速度
     * @return float 实际速度值:单位m/min
     */
    float GetActualSpeed() const;

    /*!
     * @brief 更新电量信息
     * @param const unsigned char& 新的电量信息
     * @return bool 当电量信息发生改变时返回true,否则返回false
     */
    bool UpdateBattery(const unsigned char& _battery);

    /*!
     * @brief 获取电量信息
     * @return unsigned char 电量信息
     */
    unsigned char GetBattery() const;

    /*!
     * @brief 更新载货数量信息
     * @param const unsigned char& 新的载货数量信息
     * @return bool 当载货数量发生改变时返回true,否则返回false
     */
    bool UpdateCargo(const unsigned char& _cargo);

    /*!
     * @brief 获取载货数量信息
     * @return unsigned char 载货数量信息
     */
    unsigned char GetCargo() const;

public:
    /*!
     * @brief 更新动作信息
     * @param const unsigend char& 新的动作信息
     * @param const unsigned char& 新的动作状态信息
     * @return bool 当动作信息发生改变时返回true,否则返回true
     */
    bool UpdateAction(const unsigned char& _action,const unsigned char& _actStatus);

    /*!
     * @brief 获取动作信息
     * @return unsigned char 动作信息
     */
    unsigned char GetAction() const;

    /*!
     * @brief 获取动作状态信息
     * @return unsigned har 动作状态信息
     */
    unsigned char GetActionStatus() const;

    /*!
     * @brief 获取动作执行的实际
     * @return size_t 动作执行时间:单位(ms)
     */
    size_t GetActionExecutionTime() const;

    /*!
     * @brief 获取动作信息
     * @param unsigned char& 动作信息
     * @param unsigned char& 动作状态信息
     * @return size_t 动作执行时间:单位(ms)
     */
    size_t GetAction(unsigned char& _action,unsigned char& _actStatus) const;

public:
    /*!
     * @brief 更新当前RFID地标卡信息
     * @param const unsigned short& 新的当前RFID地标卡信息
     * @return bool 当RFID地标卡发生改变时返回true,否则返回false
     */
    bool UpdateCurRfid(const unsigned short& _rfid);

    /*!
     * @brief 获取当期RFID地标卡信息
     * @return unsigned short 当前RFID地标卡信息
     */
    unsigned short GetCurRfid() const;

    /*!
     * @brief 获取历史RFID地标卡信息
     * @return unsigned short 历史RFID地标卡信息
     */
    unsigned short GetOldRfid() const;

    /*!
     * @brief 更新终点RFID地标卡信息
     * @param const unsigned short& 新的终点RFID地标卡信息
     * @return bool 当RFID地标卡发生改变时返回true,否则返回false
     */
    bool UpdateEndRfid(const unsigned short& _rfid);

    /*!
     * @brief 获取终点RFID地标卡信息
     * @return unsigned short 终点RFID地标卡信息
     */
    unsigned short GetEndRfid() const;

    /*!
     * @brief 获取历史终点RFID地标卡信息
     * @return unsigned short 历史终点RFID地标卡信息
     */
    unsigned short GetOldEndRfid() const;

signals:

public slots:
};

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

#endif // AGVBASE_H
