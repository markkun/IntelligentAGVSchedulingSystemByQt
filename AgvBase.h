/*!
 * @file AgvBase.h
 * @brief 描述AGV基本属性以及功能的文件
 * @date 2019-10-15
 * @author FanKaiyu
 * @version 1.0
 */
#ifndef AGVBASE_H
#define AGVBASE_H

#include <QObject>
#include <QtNetwork>
#include <chrono>
#include <string>
#include <AgvLocation.h>

/*! @brief 命令错误码枚举 */
enum CmdError
{
    CMD_SUCCESS,    /*!< 指令执行成功 */
};

/*! @brief AGV状态枚举 */
enum AgvStatus
{
    STA_WAIT,       /*!< 待机状态 */
    STA_RUN,        /*!< 运行状态 */
    STA_STOP,       /*!< 停止状态 */
    STA_SCREAM,     /*!< 急停状态 */
    STA_OBSDOWN,    /*!< 非接触式避障减速 */
    STA_TRASTOP,    /*!< 交通管制停止 */
    STA_SLEEP,      /*!< 休眠状态 */
    STA_CHARGE,     /*!< 充电状态 */
    STA_RMTSCREAM,  /*!< 远程急停状态 */
    STA_ALLSCREAM,  /*!< 全线急停状态 */
    STA_SPEEDUP,    /*!< 加速状态 */
    STA_SPEEDDOWN,  /*!< 减速状态 */
    STA_PAUSE,      /*!< 暂停状态 */
};

/*! @brief AGV异常码枚举 */
enum AgvError
{
    ERR_NONE,           /*!< 无异常 */
    ERR_MISS,           /*!< 脱磁异常 */
    ERR_OBS,            /*!< 非接触式避障停止 */
    ERR_MOBS,           /*!< 接触式避障停止 */
    ERR_NET = -1,       /*!< 网络异常 */
    ERR_LIFTER = -2,    /*!< 升降杆异常 */
    ERR_ROLLER = -3,    /*!< 辊筒异常 */
    ERR_ARM = -4,       /*!< 机械臂异常 */
};

/*! @brief AGV类型枚举 */
enum AgvType
{
    TYPE_TRANSFER,      /*!< 移载式AGV */
    TYPE_LIFTING,       /*!< 举升式AGV */
    TYPE_PULL,          /*!< 牵引式AGV */
    TYPE_SUBMERSIBLE,   /*!< 潜入式AGV */
    TYPE_ARM,           /*!< 机械臂式AGV */
    TYPE_FORK,          /*!< 叉车式AGV */
};

/*! @brief AGV动作状态枚举 */
enum AgvActStaus
{
    ACT_NONE,   /*!< 动作未执行 */
    ACT_EXE,    /*!< 动作正在执行 */
    ACT_FIN,    /*!< 动作完成 */
};

/*! @brief AGV载货数量枚举 */
enum AgvCargo
{
    CRG_EMTPY,          /*!< 空载 */
    CRG_FULL = 0xFF,    /*!< 满载 */
};

/*! @brief AGV模式状态枚举 */
enum AgvMode
{
    MODE_HAND,  /*!< 手动模式 */
    MODE_AUTO,  /*!< 自动模式 */
};

typedef unsigned short AGVID;
typedef unsigned char CMDERR;
typedef unsigned char AGVSTA;
typedef char AGVERR;
typedef unsigned char AGVTYPE;
typedef unsigned char AGVBATTERY;
typedef char AGVSPEED;
typedef unsigned char AGVACT;
typedef unsigned char AGVACTSTA;
typedef unsigned char AGVCRG;
typedef unsigned char AGVMODE;

/*!
 * @class AgvBase
 * @brief 描述AGV基本属性以及功能的类
 * @date 2019-10-16
 */
class AgvBase
        :public QObject
{
    Q_OBJECT
public:
    AgvBase(const AGVTYPE& _type,const AGVID& _id);

protected:
    AGVTYPE m_byType;           /*!< 类型 */
    AGVID m_usID;               /*!< 编号 */

protected:
    AGVSTA m_byStatus;          /*!< 状态 */
    AGVBATTERY m_byBattery;     /*!< 电量 */
    AGVSPEED m_chSpeed;         /*!< 速度 */
    AGVERR m_chError;           /*!< 异常 */
    AGVACT m_byAct;             /*!< 动作 */
    AGVACTSTA m_byActSta;       /*!< 动作状态 */
    AGVCRG m_byCargo;           /*!< 载货数量 */
    AGVMODE m_byMode;           /*!< 模式状态 */

protected:
    std::chrono::steady_clock::time_point m_actCount; /*!< 动作计时器 */

protected:
    AgvLocation m_curLocat;     /*!< 当前坐标 */
    AgvLocation m_endLocat;     /*!< 终点坐标 */
    AgvLocation m_oldLocat;     /*!< 历史坐标 */
    AgvLocation m_oldEndLocat;  /*!< 历史终点坐标 */

protected:
    QHostAddress m_addPeer;     /*!< 远端IP地址/设备IP地址:用以描述AGV IP地址的属性 */
    quint16 m_peerPort;         /*!< 远端端口/设备端口:用以描述AGV 端口的属性 */
    QHostAddress m_addLocal;    /*!< 本地IP地址：用以绑定指定的网卡,通过此网卡连接AGV */
    quint16 m_localPort;        /*!< 本地端口:用以绑定指定的端口,通过此端口连接AGV */
    bool m_bClient;             /*!< 网络模式:用于描述AGV采用的连接方式(服务端/客户端) */
    bool m_bAutoPort;           /*!< 随机端口 */
    QTcpSocket* m_socket;       /*!< socket对象指针 */

protected:
    /*!
     * @brief 初始化AGV属性
     * @date 2019-10-15
     * @param AGVTYPE 类型
     * @param AGVID 编号
     */
    void Initialize(const AGVTYPE& _type,const AGVID& _id);

    /*!
     * brief 初始化AGV状态
     * @date 2019-10-16
     */
    void InitStatus();

public:
    /*!
     * @brief 获取类型
     * @date 2019-10-15
     * @return AGVTYPE 类型
     */
    AGVTYPE GetType() const;

    /*!
     * @brief 获取编号
     * @date 2019-10-15
     * @return AGVID 编号
     */
    AGVID GetID() const;

public:
    /*!
     * @brief 更新状态信息
     * @date 2019-10-15
     * @param const AGVSTA 状态信息
     * @return bool 状态发生改变返回true,否则返回false
     */
    bool UpdateStatus(const AGVSTA& _sta);

    /*!
     * @brief 获取状态信息
     * @date 2019-10-15
     * @return AGVSTA 状态信息
     */
    AGVSTA GetStatus() const;

    /*!
     * @brief 获取状态信息文本
     * @date 2019-10-15
     * @return string 状态信息文本
     */
    std::string GetStatusText() const;

    /*!
     * @brief 更新电量信息
     * @date 2019-10-15
     * @param AGVBATTERY 电量信息
     * @return bool 电量发生改变时返回true,否则返回false
     */
    bool UpdateBattery(const AGVBATTERY& _battery);

    /*!
     * @brief 获取电量信息
     * @date 2019-10-15
     * @return AGVBATTERY 电量信息
     */
    AGVBATTERY GetBattery() const;

    /*!
     * @brief 更新异常信息
     * @date 2019-10-15
     * @param AGVERR 异常信息
     * @return bool 异常信息发生改变时返回true,否则返回false
     */
    bool UpdateError(const AGVERR& _error);

    /*!
     * @brief 获取异常信息
     * @date 2019-10-15
     * @return AGVERR 异常信息
     */
    AGVERR GetError() const;

    /*!
     * @brief 获取异常信息文本
     * @date 2019-10-15
     * @return string 异常信息文本
     */
    std::string GetErrorText() const;

    /*!
     * @brief 更新速度信息
     * @date 2019-10-15
     * @param AGVSPEED 速度信息
     * @return bool 速度发生改变时返回true,否则返回false
     */
    bool UpdateSpeed(const AGVSPEED& _speed);

    /*!
     * @brief 获取速度信息
     * @date 2019-10-15
     * @return AGVSPEED 速度信息
     */
    AGVSPEED GetSpeed() const;

    /*!
     * @brief 更新动作信息
     * @date 2019-10-15
     * @param AGVACT 动作信息
     * @param AGVACTSTA 动作状态信息
     * @return bool 动作发生改变返回true,否则返回false
     */
    bool UpdateAction(const AGVACT& _act,const AGVACTSTA& _sta);

    /*!
     * @brief 获取动作信息
     * @date 2019-10-15
     * @return AGVACT 动作信息
     */
    AGVACT GetAction() const;

    /*!
     * @brief 获取动作状态信息
     * @date 2019-10-15
     * @return AGVACTSTA 动作状态
     */
    AGVACTSTA GetActStatus() const;

    /*!
     * @brief 获取动作执行时间
     * @date 2019-10-15
     * @return size_t 动作执行时间ms
     */
    size_t GetActExeTime() const;

    /*!
     * @brief 获取动作信息
     * @date 2019-10-15
     * @param AGVACT& 返回动作信息
     * @param AGVACTSTA& 返回动作状态
     * @return size_t 返回动作的执行时间
     */
    size_t GetAction(AGVACT& _act,AGVACTSTA _sta) const;

    /*!
     * @brief 更新载货数量
     * @date 2019-10-15
     * @param AGVCRG 载货数量
     * @return bool 载货数量发生改变时返回true,否则返回false
     */
    bool UpdateCargo(const AGVCRG& _cargo);

    /*!
     * @brief 获取载货数量
     * @date 2019-10-15
     * @return AGVCRG 载货数量
     */
    AGVCRG GetCargo() const;

    /*!
     * @brief 更新模式状态
     * @date 2019-10-15
     * @param AGVMODE 模式状态
     * @return bool 当模式状态发生改变时返回true,否则返回false
     */
    bool UpdateMode(const AGVMODE& _mode);

    /*!
     * @brief 获取模式状态
     * @date 2019-10-15
     * @return unsigned char 模式状态
     */
    AGVMODE GetMode() const;

public:
    /*!
     * @brief 更新当前坐标
     * @date 2019-10-16
     * @param AgvLocation& 当前坐标
     * @return bool 当坐标发生改变时返回true,否则返回false
     */
    bool UpdateCurLocation(const AgvLocation& _locat);

    /*!
     * @brief 获取当前坐标
     * @date 2019-10-16
     * @return AgvLocation 当前坐标
     */
    AgvLocation GetCurLocation() const;

    /*!
     * @brief 获取历史坐标
     * @date 2019-10-16
     * @return AgvLocation 历史坐标
     */
    AgvLocation GetOldLocation() const;

    /*!
     * @brief 更新终点坐标
     * @date 2019-10-16
     * @param AgvLocation& 终点坐标
     * @return bool 当终点坐标发生改变时返回true,否则返回true
     */
    bool UpdateEndLocation(const AgvLocation& _locat);

    /*!
     * @brief 获取终点坐标
     * @date 2019-10-16
     * @return AgvLocation 终点坐标
     */
    AgvLocation GetEndLocation() const;

    /*!
     * @brief 获取历史终点坐标
     * @date 2019-10-16
     * @return AgvLocation 历史终点坐标
     */
    AgvLocation GetOldEndLocation() const;

protected:
    /*!
     * @brief 连接远端设备
     * @date 2019-10-16
     * @param QHostAddress 远端IP地址(必要)
     * @param quint16 远端端口(必要)
     * @param QHostAddress 本地IP地址(非必要)
     * @param quint16 本地端口(非必要)
     */
    void Connect(const QHostAddress& _preeIP,const quint16& _preePort);
    void Connect(const QHostAddress& _preeIP,const quint16& _preePort,const quint16& _localPort);
    void Connect(const QHostAddress& _preeIP,const quint16& _preePort,const QHostAddress& _localIP,const quint16& _localPort);

    /*!
     * @brief 连接远端设备
     * @date 2019-10-16
     * @param QTcpSocket* 远端socket对象指针
     * @param QHostAddress& _preeIP 远端IP地址(必要)
     * @param quint16& 远端端口(非必要)
     * @return bool 连接成功返回true,否则返回false
     */
    bool Connect(const QTcpServer* _socket,const QHostAddress& _peerIP);
    bool Connect(const QTcpServer* _socket,const QHostAddress& _peerIP,const quint16& _peerPort);

signals:

protected slots:
    /*!
     * @brief 连接中断
     *
     * 服务端关闭引起的连接中断
     */
    void disConnected();
};

#endif // AGVBASE_H
