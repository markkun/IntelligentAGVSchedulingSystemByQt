#include "AgvBase.h"

AgvBase::AgvBase(const AgvType& _type, const AId_t& _id,
                 const bool &_bClient, const QString &_peerAddr, const unsigned short &_peerPort,
                 const QString &_localAddr, const unsigned short &_localPort,
                 QObject *parent): QObject(parent)
{
    Initialize(_type,_id,_bClient,_peerAddr,_peerPort,_localAddr,_localPort);
}

AgvBase::~AgvBase()
{
    if(m_pSocket)
    {
        m_pSocket->close();

        m_pSocket->waitForDisconnected(-1);
    }

    m_thread.quit();
    m_thread.wait();
}

void AgvBase::Initialize(const AgvType& _type, const AId_t &_id,
                         const bool &_bClient, const QString &_peerAddr, const unsigned short &_peerPort,
                         const QString &_localAddr, const unsigned short &_localPort)
{
    if(_type)
    {
        throw("The AGV type cannot be empty");
    }

    if(_peerAddr.isNull() || _peerAddr.isEmpty())
    {
        throw("The AGV ip address cannot be empty");
    }

    this->m_pType = &_type;
    this->m_id = _id;

    this->m_pSocket = nullptr;
    this->m_bClient = _bClient;
    this->m_peerAddr = _peerAddr;
    this->m_peerPort = _peerPort;
    this->m_localAddr = _localAddr;
    this->m_localPort = _localPort;

    InitAttribute();

    if(m_bClient == false)
    {
        Connect();
    }

    moveToThread(&m_thread);
    m_thread.start();

    connect(&m_timer,SIGNAL(timeout()),this,SLOT(sendPacket));
    m_timer.setInterval(100);
}

void AgvBase::InitAttribute()
{
    this->m_mode = Mode_Hand;
    this->m_status = Sta_Wait;
    this->m_battery = 0;
    this->m_speed = 0;
    this->m_cargo = 0;
    this->m_error = Err_None;

    this->m_action = 0;
    this->m_actStatus = 0;
    this->m_actCount = std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());

    this->m_curRfid = 0;
    this->m_endRfid = 0;
    this->m_oldRfid = m_curRfid;
    this->m_oldEndRfid = m_endRfid;
}

AgvType AgvBase::GetType() const
{
    return *m_pType;
}

AgvBase::AId_t AgvBase::GetID() const
{
    return m_id;
}

bool AgvBase::UpdateMode(const AMode_t &_mode)
{
    if(m_mode != _mode)
    {
        m_mode = _mode;
        return true;
    }

    return false;
}

AgvBase::AMode_t AgvBase::GetMode() const
{
    return m_mode;
}

bool AgvBase::UpdateStatus(const AStatus_t &_status)
{
    if(m_status != _status)
    {
        m_status = _status;
        return true;
    }

    return false;
}

AgvBase::AStatus_t AgvBase::GetStatus() const
{
    return m_status;
}

std::string AgvBase::GetStatusText() const
{
    std::string _str = "unknown";

    switch (m_status)
    {
    case Sta_Wait:
        _str = "待机";
        break;
    case Sta_Run:
        _str = "运行";
        break;
    case Sta_Stop:
        _str = "停止";
        break;
    case Sta_Scream:
        _str = "急停";
        break;
    case Sta_ObsDonw:
        _str = "检测到障碍物,减速行驶";
        break;
    case Sta_Find:
        _str = "寻磁中";
        break;
    case Sta_Pause:
        _str = "暂停";
        break;
    case Sta_Sleep:
        _str = "休眠";
        break;
    case Sta_SpeedUp:
        _str = "加速行驶";
        break;
    case Sta_SpeedDown:
        _str = "减速行驶";
        break;
    case Sta_Charging:
        _str = "充电中";
        break;
    case Sta_AllScream:
        _str = "急停";
        break;
    case Sta_TrafficStop:
        _str = "交通管制中";
        break;
    case Sta_RemoteScream:
        _str = "急停";
        break;
    default:
        _str = "unknown";
        break;
    }

    return _str;
}

bool AgvBase::UpdateSpeed(const ASpeed_t &_speed)
{
    if(m_speed != _speed)
    {
        m_speed = _speed;
        return true;
    }

    return false;
}

AgvBase::ASpeed_t AgvBase::GetSpeed() const
{
    return m_speed;
}

float AgvBase::GetActualSpeed() const
{
    if(m_pType == nullptr)
    {
        return 0.0f;
    }

    return m_speed * m_pType->m_maxSpeed;
}

bool AgvBase::UpdateBattery(const ABattery_t& _battery)
{
    if(m_battery != _battery)
    {
        m_battery = _battery;
        return true;
    }

    return false;
}

AgvBase::ABattery_t AgvBase::GetBattery() const
{
    return m_battery;
}

bool AgvBase::UpdateCargo(const ACargo_t &_cargo)
{
    if(m_cargo != _cargo)
    {
        m_cargo = _cargo;
        return true;
    }

    return false;
}

AgvBase::ACargo_t AgvBase::GetCargo() const
{
    return m_cargo;
}

bool AgvBase::UpdateError(const AgvBase::AError_t& _error)
{
    if(m_error != _error)
    {
        m_error = _error;

        if(m_error != Err_None)
        {
            emit ThrowError();
        }

        return true;
    }

    return false;
}

AgvBase::AError_t AgvBase::GetError() const
{
    return m_error;
}

std::string AgvBase::GetErrorText(const AgvBase::AError_t &_error)
{
    std::string _str = "unknown";
    switch(_error)
    {
    case Err_Arm:
        _str = "机械臂超时未完成动作";
        break;
    case Err_Net:
        _str = "与AGV的连接异常中断";
        break;
    case Err_Obs:
        _str = "检测到移动路线上存在未知障碍物造成AGV停止移动";
        break;
    case Err_Miss:
        _str = "寻磁超时造成AGV停止移动";
        break;
    case Err_Mobs:
        _str = "碰撞到未知障碍物造成AGV停止移动";
        break;
    case Err_None:
        _str = "无异常";
        break;
    case Err_Lifter:
        _str = "升降杆超时未完成动作";
        break;
    case Err_Roller:
        _str = "辊筒超时未完成动作";
        break;
    default:
        _str = "unknown";
        break;
    }

    return _str;
}

std::string AgvBase::GetErrorText()
{
    return GetErrorText(m_error);
}

std::string AgvBase::GetSlefErrorText()
{
    return GetErrorText(m_errSelf);
}

void AgvBase::UpdateErrorSelf(const AgvBase::AError_t &_error)
{
    if(m_errSelf != _error)
    {
        m_errSelf = _error;

        if(m_errSelf != Err_None)
        {
            emit ThrowError();
        }

        return;
    }

    return;
}

bool AgvBase::UpdateAction(const AAction_t &_action, const AActStatus_t &_actStatus)
{
    bool _update = false;

    if(m_action != _action)
    {
        m_action = _action;

        _update = true;
    }

    if(m_actStatus != _actStatus)
    {
        m_actStatus = _actStatus;

        _update = true;
    }

    if(m_actStatus == ActSta_Exe)
    {
        m_actCount = std::chrono::steady_clock::now();
    }
    else
    {
        m_actCount = std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());
    }

    return _update;
}

AgvBase::AAction_t AgvBase::GetAction() const
{
    return m_action;
}

AgvBase::AActStatus_t AgvBase::GetActionStatus() const
{
    return m_actStatus;
}

size_t AgvBase::GetActionExecutionTime() const
{
    if(m_actStatus == ActSta_Exe)
    {
        auto _dis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_actCount);

        long long _dis_t = _dis.count();

        if(_dis_t <= 0)
        {
            return 0;
        }

        return static_cast<size_t>(_dis_t);
    }

    return 0;
}

void AgvBase::SetActionExecutionTime(const size_t& _time)
{
    m_actCount = std::chrono::steady_clock::now() - std::chrono::milliseconds(_time);

    return;
}

size_t AgvBase::GetAction(AAction_t &_action, AActStatus_t &_actStatus) const
{
    _action = m_action;
    _actStatus = m_actStatus;

    return GetActionExecutionTime();
}

bool AgvBase::UpdateCurRfid(const RfidBase::Rfid_t &_rfid)
{
    if(m_curRfid != _rfid)
    {
        m_oldRfid = m_curRfid;
        m_curRfid = _rfid;

        return true;
    }

    return false;
}

RfidBase::Rfid_t AgvBase::GetCurRfid() const
{
    return m_curRfid;
}

RfidBase::Rfid_t AgvBase::GetOldRfid() const
{
    return m_oldRfid;
}

bool AgvBase::UpdateEndRfid(const RfidBase::Rfid_t &_rfid)
{
    if(m_endRfid != _rfid)
    {
        m_oldEndRfid = m_endRfid;
        m_endRfid = _rfid;

        return true;
    }

    return false;
}

RfidBase::Rfid_t AgvBase::GetEndRfid() const
{
    return m_endRfid;
}

RfidBase::Rfid_t AgvBase::GetOldEndRfid() const
{
    return m_oldEndRfid;
}

AgvBase::CmdErr AgvBase::WakeUp()
{
    return SetStatus(CmdSta_Wakeup);
}

AgvBase::CmdErr AgvBase::Reset()
{
    return SetStatus(CmdSta_Reset);
}

AgvBase::CmdErr AgvBase::Restart()
{
    return SetStatus(CmdSta_Restart);
}

AgvBase::CmdErr AgvBase::Scream()
{
    return SetStatus(CmdSta_Scream);
}

AgvBase::CmdErr AgvBase::Sleep()
{
    return SetStatus(CmdSta_Sleep);
}

AgvBase::CmdErr AgvBase::Pause()
{
    return SetStatus(CmdSta_Pause);
}

AgvBase::CmdErr AgvBase::Continue()
{
    return SetStatus(CmdSta_Continue);
}

AgvBase::CmdErr AgvBase::RemoteScream()
{
    return SetStatus(CmdSta_RmtScream);
}

AgvBase::CmdErr AgvBase::Shutdown()
{
    return SetStatus(CmdSta_Shutdown);
}

AgvBase::CmdErr AgvBase::Move(const unsigned short &_rfid)
{
    if(IsConnected() == false)
    {
        // 网络未连接
        return Cmd_NetErr;
    }

    if(m_mode != Mode_Auto)
    {
        // 未处于自动模式
        return Cmd_StatusErr;
    }

    if(m_status != Sta_Wait)
    {
        // 未处于待机状态
        return Cmd_StatusErr;
    }

    // 编号 + 功能 + 起始RFID + 终止RFID
    unsigned int _size = sizeof(m_id) + 1 + sizeof(m_curRfid) * 2;  /*!< 数据包大小 */

    char* _packet = new char[_size];    /*!< 数据包 */

    // 初始化数据包
    memset(_packet,0,_size);

    unsigned int _index = 0;    /*!< 下标 */

    // 编号
    for(unsigned int i = sizeof(m_id); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_id >> 8 * (i-1)) & 0xFF);
    }

    // 功能
    _packet[_index++] = Func_Move;

    // 当前RFID地标卡
    for(unsigned int i = sizeof(m_curRfid); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_curRfid >> 8 * (i-1)) & 0xFF);
    }

    // 终止RFID地标卡
    for(unsigned int i = sizeof(m_curRfid); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((_rfid >> 8 * (i-1)) & 0xFF);
    }

    // 合成报文包
    m_listSend.push_back(m_pType->m_pProtocol->CreatePacket(QByteArray(_packet,static_cast<int>(_index))));

    // 释放内存
    delete[] _packet;

    return Cmd_Success;
}

AgvBase::CmdErr AgvBase::TrafficPass()
{
    if(IsConnected() == false)
    {
        // 网络未连接
        return Cmd_NetErr;
    }

    if(m_mode != Mode_Auto)
    {
        // 未处于自动模式
        return Cmd_StatusErr;
    }

    if(m_status != Sta_TrafficStop)
    {
        // 未处于交通管制停止状态
        return Cmd_StatusErr;
    }

    // 编号 + 功能 + 当前RFID + 命令
    unsigned int _size = sizeof(m_id) + 1 + sizeof(m_curRfid) + 1;  /*!< 数据包大小 */

    char* _packet = new char[_size];    /*!< 数据包 */

    //  初始化数据包
    memset(_packet,0,_size);

    unsigned int _index = 0;    /*!< 下标 */

    // 编号
    for(unsigned int i = sizeof(m_id); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_id >> 8 * (i-1)) & 0xFF);
    }

    // 功能
    _packet[_index++] = Func_Traffic;

    // 当前RFID地标卡
    for(unsigned int i = sizeof(m_curRfid); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_curRfid >> 8 * (i-1)) & 0xFF);
    }

    // 命令
     _packet[_index++] = 1;

    // 合成报文包
    m_listSend.push_back(m_pType->m_pProtocol->CreatePacket(QByteArray(_packet,static_cast<int>(_index))));

    // 释放内存
    delete[] _packet;

    return Cmd_Success;
}

AgvBase::CmdErr AgvBase::SetSpeed(const char &_speed)
{
    if(IsConnected() == false)
    {
        // 网络未连接
        return Cmd_NetErr;
    }

    if(m_mode != Mode_Auto)
    {
        // 未处于自动模式
        return Cmd_StatusErr;
    }

    if(_speed > 100 || _speed < -100)
    {
        // 速度值过大
        return Cmd_ParamErr;
    }

    if((m_speed > 0 && _speed <= 0) || (m_speed < 0 && _speed >= 0))
    {
        // 不能改变当前AGV移动的方向
        return Cmd_ParamErr;
    }

    // 编号 + 功能 + 当前RFID + 速度
    unsigned int _size = sizeof(m_id) + 1 + sizeof(m_curRfid) + sizeof(m_speed);    /*!< 数据包大小 */

    char* _packet = new char[_size];    /*!< 数据包 */

    // 初始化数据包
    memset(_packet,0,_size);

    unsigned int _index = 0;    /*!< 下标 */

    // 编号
    for(unsigned int i = sizeof(m_id); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_id >> 8 * (i-1)) & 0xFF);
    }

    // 功能
    _packet[_index++] = Func_Speed;

    // 当前RFID地标卡
    for(unsigned int i = sizeof(m_curRfid); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_curRfid >> 8 * (i-1)) & 0xFF);
    }

    // 速度
    for(unsigned int i = sizeof(m_speed); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((_speed >> 8 * (i-1)) & 0xFF);
    }

    // 合成报文包
    m_listSend.push_back(m_pType->m_pProtocol->CreatePacket(QByteArray(_packet,static_cast<int>(_index))));

    // 释放内存
    delete[] _packet;

    return Cmd_Success;
}

std::string AgvBase::GetCmdError(const AgvBase::CmdErr &_err)
{
    std::string _str = "unknown";
    switch(_err)
    {
    case Cmd_ParamErr:
        _str = "指令参数错误";
        break;
    case Cmd_Success:
        _str = "指令创建成功";
        break;
    case Cmd_NetErr:
        _str = "网络未连接";
        break;
    case Cmd_ActionErr:
        _str = "动作状态不正确";
        break;
    case Cmd_StatusErr:
        _str = "状态不正确";
        break;
    default:
        _str = "unknown";
        break;
    }

    return _str;
}

AgvBase::CmdErr AgvBase::StopAction()
{
    if(m_action == 0 || m_actStatus == ActSta_Fin)
    {
        // 无动作或动作已完成
        return Cmd_ActionErr;
    }

    return Action(0);
}

void AgvBase::Heartbeat()
{
    if(IsConnected() == false)
    {
        // 网络未连接
        return;
    }

    // 类型 + 编号 + 功能码 + 模式 + 状态 + 速度 + 电量 + 当前RFID + 终点RFID + 载货数量 + 异常 + 动作 + 动作状态
    unsigned int _size = sizeof(m_id) + 1 + sizeof(m_mode) + sizeof(m_status) + sizeof(m_speed) + sizeof(m_battery)
            + sizeof(m_curRfid) + sizeof(m_endRfid) + sizeof(m_cargo) + sizeof(m_error) + sizeof(m_action) + sizeof(m_actStatus);   /*!< 数据体大小 */

    char _error = m_error;  /*!< 当前异常信息 */

    if(_error < 0)
    {
        // 当前异常是由系统自检测出的时候,异常信息不上传至AGV
        _error = 0;
    }

    char* _packet = new char[_size];    /*!< 数据包 */

    // 初始化数据包
    memset(_packet,0,_size);

    unsigned int _index = 0;    /*!< 下标 */

    // 编号
    for(unsigned int i = sizeof(m_id); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_id >> 8 * (i-1)) & 0xFF);
    }

    // 功能
    _packet[_index++] = Func_Heartbeat;

    // 类型
    for(unsigned int i = sizeof(m_mode); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_mode >> 8 * (i-1)) & 0xFF);
    }

    // 状态
    for(unsigned int i = sizeof(m_status); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_status >> 8 * (i-1)) & 0xFF);
    }

    // 速度
    for(unsigned int i = sizeof(m_speed); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_speed >> 8 * (i-1)) & 0xFF);
    }

    // 电量
    for(unsigned int i = sizeof(m_battery); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_battery >> 8 * (i-1)) & 0xFF);
    }

    // 当前RFID
    for(unsigned int i = sizeof(m_curRfid); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_curRfid >> 8 * (i-1)) & 0xFF);
    }

    //  终点RFID
    for(unsigned int i = sizeof(m_endRfid); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_endRfid >> 8 * (i-1)) & 0xFF);
    }

    //  载货数量
    for(unsigned int i = sizeof(m_cargo); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_cargo >> 8 * (i-1)) & 0xFF);
    }

    // 异常信息
    for(unsigned int i = sizeof(m_error); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((_error >> 8 * (i-1)) & 0xFF);
    }

    // 动作信息
    for(unsigned int i = sizeof(m_action); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_action >> 8 * (i-1)) & 0xFF);
    }

    // 动作状态
    for(unsigned int i = sizeof(m_actStatus); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_actStatus >> 8 * (i-1)) & 0xFF);
    }

    // 合成报文包
    m_listSend.push_back(m_pType->m_pProtocol->CreatePacket(QByteArray(_packet,static_cast<int>(_index)))); /*!< 报文包 */

    // 释放内存
    delete[] _packet;

    return;
}

AgvBase::CmdErr AgvBase::SetStatus(const unsigned char& _cmd)
{
    if(IsConnected() == false)
    {
        // 网络未连接
        return Cmd_NetErr;
    }

    if(m_mode != Mode_Auto)
    {
        // 未处于自动模式
        return Cmd_StatusErr;
    }

    switch(_cmd)
    {
    case CmdSta_Pause:
        // 暂停
        if(m_status == Sta_Pause)
        {
            // 已经暂停
            return Cmd_StatusErr;
        }
        break;
    case CmdSta_Reset:
        // 复位
        if(m_status != Sta_AllScream && m_status != Sta_RemoteScream)
        {
            // 不能够复位此状态
            return Cmd_StatusErr;
        }
        break;
    case CmdSta_Sleep:
        // 休眠
        if(m_status == Sta_Sleep)
        {
            // 已经休眠
            return Cmd_StatusErr;
        }
        break;
    case CmdSta_Scream:
        // 急停
        if(m_status == Sta_AllScream || m_status == Sta_RemoteScream)
        {
            // 已经急停
            return Cmd_StatusErr;
        }
        break;
    case CmdSta_Wakeup:
        // 唤醒
        if(m_status != Sta_Sleep)
        {
            // 不能唤醒此状态
            return Cmd_StatusErr;
        }
        break;
    case CmdSta_Restart:
        // 重置
        if(m_status != Sta_Wait || m_speed != 0 || m_battery != 0
                || m_curRfid != 0 || m_endRfid != 0 || m_cargo != 0 || m_error != Err_None || m_action != 0 || m_actStatus != 0)
        {
            // 已经重置
            return Cmd_StatusErr;
        }
        break;
    case CmdSta_Continue:
        // 继续
        if(m_status != Sta_Pause)
        {
            // 不能继续此状态
            return Cmd_StatusErr;
        }
        break;
    case CmdSta_Shutdown:
        // 关机
        break;
    case CmdSta_RmtScream:
        // 远程急停
        if(m_status == Sta_AllScream || m_status == Sta_RemoteScream)
        {
            // 已经急停
            return Cmd_StatusErr;
        }
        break;
    default:
        // 无效的命令
        return Cmd_ParamErr;
    }

    // 类型 + 功能 + 命令
    unsigned int _size = sizeof(m_id) + 1 +sizeof(_cmd);    /*!< 数据包大小 */

    char* _packet = new char[_size];    /*!< 数据包 */

    // 初始化数据包
    memset(_packet,0,_size);

    unsigned int _index = 0;    /*!< 下标 */

    // 编号
    for(unsigned int i = sizeof(m_id); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_id >> 8 * (i-1)) & 0xFF);
    }

    // 功能
    _packet[_index++] = Func_Status;

    // 命令
    for(unsigned int i = sizeof(_cmd); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((_cmd >> 8 * (i-1)) & 0xFF);
    }

    // 合成报文包
    m_listSend.push_back(m_pType->m_pProtocol->CreatePacket(QByteArray(_packet,static_cast<int>(_index))));

    // 释放内存
    delete[] _packet;

    return Cmd_Success;
}

AgvBase::CmdErr AgvBase::Action(const unsigned char &_act)
{
    if(IsConnected() == false)
    {
        // 网络未连接
        return Cmd_NetErr;
    }

    if(_act == m_action && m_actStatus == ActSta_Fin)
    {
        // 已完成相同的动作
        return Cmd_ActionErr;
    }

    if(m_mode != Mode_Auto)
    {
        // 未处于自动模式
        return Cmd_StatusErr;
    }

    if(m_status != Sta_Wait)
    {
        // 未处于待机状态
        return Cmd_StatusErr;
    }

    // 编号 + 功能 + 当前RFID + 动作码
    unsigned int _size = sizeof(m_id) + 1 + sizeof(m_curRfid) + sizeof(m_action);   /*!< 数据包大小 */

    char* _packet = new char[_size];    /*!< 数据包 */

    // 初始化数据包
    memset(_packet,0,_size);

    unsigned int _index = 0;    /*!< 下标 */

    // 编号
    for(unsigned int i = sizeof(m_id); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_id >> 8 * (i-1)) & 0xFF);
    }

    // 功能
    _packet[_index++] = Func_Action;

    // 当前RFID地标卡
    for(unsigned int i = sizeof(m_curRfid); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((m_curRfid >> 8 * (i-1)) & 0xFF);
    }

    // 动作码
    for(unsigned int i = sizeof(m_action); i > 0;--i)
    {
        _packet[_index++] = static_cast<char>((_act >> 8 * (i-1)) & 0xFF);
    }

    // 合成报文包
    m_listSend.push_back(m_pType->m_pProtocol->CreatePacket(QByteArray(_packet,static_cast<int>(_index))));

    // 释放内存
    delete[] _packet;

    return Cmd_Success;
}

void AgvBase::ProcessPacket(const QByteArray &_packet)
{
    unsigned int _sizeLen = 0;

    switch (m_pType->m_pProtocol->GetType())
    {
    case Protocol_STM32:
        _sizeLen = sizeof(ProtocolStm32::DATA_LEN);
        break;
    case Protocol_PLC:
        _sizeLen = sizeof(ProtocolPlc::DATA_LEN);
        break;
    }

    const char* _begin = _packet.data();
    const char* _data = _begin + _sizeLen;

    AId_t _id = 0;  /*!< 报文上传的编号 */
    for(unsigned int i = 0; i < sizeof(AId_t);++i)
    {
        _id |= *(_data+(sizeof(AId_t) - i)) << (8 * i);
    }

    if(_id != m_id)
    {
        // 编号不符合
        return;
    }

    _data += sizeof(AId_t);

    unsigned char _func = static_cast<unsigned char>(*_data);   /*!< 功能码 */

    _data += 1;

    switch(_func)
    {
    case Func_Heartbeat:
    {
        // 心跳报文回复

        // 模式
        AMode_t _mode = 0;

        for(unsigned int i = 0; i < sizeof(AMode_t);++i)
        {
            _mode |= static_cast<AMode_t>(*(_data + i) << (8 * i));
        }

        _data = _data +sizeof(AMode_t);

        // 运行状态
        AStatus_t _status = 0;

        for(unsigned int i = 0; i < sizeof(AStatus_t);++i)
        {
            _status |= static_cast<AStatus_t>(*(_data + i) << (8 * i));
        }

        _data = _data + sizeof(AStatus_t);

        // 速度
        ASpeed_t _speed = 0;

        for(unsigned int i = 0; i < sizeof(ASpeed_t);++i)
        {
            _speed |= static_cast<ASpeed_t>(*(_data + i) << (8 * i));
        }

        _data = _data + sizeof(ASpeed_t);

        // 电量
        ABattery_t _battery = 0;

        for(unsigned int i = 0; i < sizeof(ABattery_t);++i)
        {
            _battery |= static_cast<ABattery_t>(*(_data + i) << (8 * i));
        }

        _data = _data + sizeof(ABattery_t);

        // 当前位置信息
        RfidBase::Rfid_t _curRfid = 0;

        for(unsigned int i = 0; i < sizeof(RfidBase::Rfid_t);++i)
        {
            _curRfid |= static_cast<RfidBase::Rfid_t>(*(_data + i) << (8 * i));
        }

        _data = _data + sizeof(RfidBase::Rfid_t);

        // 移动终点位置信息
        RfidBase::Rfid_t _endRfid = 0;

        for(unsigned int i = 0; i < sizeof(RfidBase::Rfid_t);++i)
        {
            _endRfid |= static_cast<RfidBase::Rfid_t>(*(_data + i) << (8 * i));
        }

        _data = _data + sizeof(RfidBase::Rfid_t);

        // 载货数量信息
        ACargo_t _cargo = 0;

        for(unsigned int i = 0; i < sizeof(ACargo_t);++i)
        {
            _cargo |= static_cast<ACargo_t>(*(_data + i) << (8 * i));
        }

        _data = _data + sizeof(ACargo_t);

        // 异常信息
        AError_t _error = 0;

        for(unsigned int i = 0; i < sizeof(AError_t);++i)
        {
            _error |= static_cast<AError_t>(*(_data + i) << (8 * i));
        }

        _data = _data + sizeof(AError_t);

        // 动作信息
        AAction_t _action = 0;

        for(unsigned int i = 0; i < sizeof(AAction_t);++i)
        {
            _action |= static_cast<AAction_t>(*(_data + i) << (8 * i));
        }

        _data = _data + sizeof(AAction_t);

        // 动作状态信息
        AActStatus_t _actStatus = 0;

        for(unsigned int i = 0; i < sizeof(AActStatus_t);++i)
        {
            _actStatus |= static_cast<AActStatus_t>(*(_data + i) << (8 * i));
        }

//        _data = _data + sizeof(AAction_t);

        // 更新
        bool bUpdate = false;   /*!< 更新标识 */

        if(UpdateMode(_mode))
        {
            bUpdate = true;
        }

        if(UpdateStatus(_status))
        {
            bUpdate = true;
        }

        if(UpdateSpeed(_speed))
        {
            bUpdate = true;
        }

        if(UpdateBattery(_battery))
        {
            bUpdate = true;
        }

        if(UpdateCurRfid(_curRfid))
        {
            bUpdate = true;
        }

        if(UpdateEndRfid(_endRfid))
        {
            bUpdate = true;
        }

        if(UpdateCargo(_cargo))
        {
            bUpdate = true;
        }

        if(UpdateError(_error))
        {
            bUpdate = true;
        }

        if(UpdateAction(_action,_actStatus))
        {
            bUpdate = true;
        }

        if(bUpdate)
        {
            emit Update();
        }

        break;
    }
    case  Func_Move:
    {
        // 移动控制报文回复
        // 本版本中，移动控制报文不回复
        break;
    }
    case Func_Speed:
    {
        // 速度控制报文回复
        // 本版本中，速度控制报文不回复
        break;
    }
    case Func_Action:
    {
        // 动作控制报文回复
        // 本版本中，动作控制报文不回复
        break;
    }
    case Func_Status:
    {
        // 状态控制报文回复
        // 本版本中，状态控制报文不回复
        break;
    }
    case Func_Traffic:
    {
        // 交通管制控制报文回复
        // 本版本中，交通管制控制报文不回复
        break;
    }
    default:
        break;
    }

    return;
}

void AgvBase::Connect()
{
    if(m_bClient)
    {
        // AGV为客户端模式，不支持连接至服务端
        return;
    }

    if(m_peerAddr.isNull() || m_peerAddr.isEmpty())
    {
        // AGV IP为空
        throw("The IP address cannot be empty");
    }

    m_pSocket = new QTcpSocket();

    // 网络关闭时触发槽函数
    connect(m_pSocket,SIGNAL(QTcpSocket::disconnected),this,SLOT(AgvBase::DisConnected));
    // 网络连接失败时触发槽函数
    connect(m_pSocket,SIGNAL(QTcpSocket::error),this,SLOT(AgvBase::Error));
    // 网络连接成功时触发槽函数
    connect(m_pSocket,SIGNAL(QTcpSocket::connected),this,SLOT(AgvBase::Connected));
    // 有数据读取时触发槽函数
    connect(m_pSocket,SIGNAL(QTcpSocket::readyRead),this,SLOT(AgvBase::ReadData));

    if((m_localAddr.isNull() == false && m_localAddr.isEmpty() == false) || m_localPort !=0)
    {
        // 绑定指定的IP与端口
        QHostAddress _addr = QHostAddress::Any;

        if(m_localAddr.isNull() == false && m_localAddr.isEmpty() == false)
        {
            _addr.setAddress(m_localAddr);
        }

         if(m_pSocket->bind(_addr,m_localPort) == false)
         {
             return;
         }
    }

    // 连接服务端
    m_pSocket->connectToHost(m_peerAddr,m_peerPort);

    return;
}

bool AgvBase::Connect(QTcpSocket &_socket)
{
    if(m_bClient == false)
    {
        // AGV为服务端模式，不支持客户端连接
        return false;
    }

    if(m_pSocket == &_socket)
    {
        // 已连接
        return true;
    }

    if(m_peerAddr.isNull() || m_peerAddr.isEmpty())
    {
        // AGVIP地址为空
        return false;
    }

    if(m_peerAddr != _socket.peerName())
    {
        // IP地址不符
        return false;
    }

    if(m_peerPort != 0 && m_peerPort != _socket.peerPort())
    {
        // 端口不符
        return false;
    }

    if(m_pSocket)
    {
        // 关闭已建立的连接
        m_pSocket->close();
        m_pSocket->waitForDisconnected(-1);
    }

    m_pSocket = &_socket;

    // 连接关闭时触发的槽函数
    connect(m_pSocket,SIGNAL(QTcpSocket::disconnected),this,SLOT(AgvBase::DisConnected));
    // 有数据读取时触发的槽函数
    connect(m_pSocket,SIGNAL(QTcpSocket::readyRead),this,SLOT(AgvBase::ReadData));

    // 连接成功触发的槽函数
    Connected();

    return true;
}

bool AgvBase::IsConnected() const
{
    if(m_pSocket == nullptr)
    {
        return false;
    }

    return m_pSocket->isOpen();
}

void AgvBase::Connected()
{
    m_timer.start();

    if(m_errSelf == Err_Net)
    {
        m_errSelf = Err_None;
    }

    return;
}

void AgvBase::DisConnected()
{
    if(m_pSocket)
    {
        m_pSocket->deleteLater();
    }

    m_pSocket = nullptr;

    m_timer.stop();

    m_listSend.clear();

    emit LinkBreak();

    return;
}

void AgvBase::Error()
{
    // 重新连接客户端
    m_pSocket->connectToHost(m_peerAddr,m_peerPort);

    return;
}

void AgvBase::ReadData()
{
    if(m_pSocket->isReadable())
    {
        m_buf += m_pSocket->readAll();

        // 处理数据
        QByteArrayList _list = m_pType->m_pProtocol->ProcessData(m_buf);

        for(QByteArrayList::iterator it = _list.begin(); it != _list.end();++it)
        {
            ProcessPacket(*it);
        }
    }

    return;
}

void AgvBase::SendPacket()
{
    if(IsConnected())
    {
        return;
    }

    Heartbeat();

    for(QByteArrayList::iterator it = m_listSend.begin();it != m_listSend.end();++it)
    {
        if(m_pSocket->write(it->data(),it->size()) == -1)
        {
            UpdateErrorSelf(Err_Net);

            DisConnected();

            return;
        }

        m_thread.wait(100);
    }

    return;
}

AgvType::AgvType(const std::string &_name,ProtocolBase& _protocol, const unsigned char &_type, const float &_speed, const float &_weright, const std::string &_brand, const std::string &_version)
{
    this->m_name = _name;
    this->m_type = _type;
    this->m_brand = _brand;
    this->m_version = _version;
    this->m_maxSpeed = _speed;
    this->m_maxWeight = _weright;
    this->m_pProtocol = &_protocol;
}

AgvType::AgvType(const AgvType &_type)
{
    this->m_name = _type.m_name;
    this->m_type = _type.m_type;
    this->m_brand = _type.m_brand;
    this->m_version = _type.m_version;
    this->m_maxSpeed = _type.m_maxSpeed;
    this->m_maxWeight = _type.m_maxWeight;
    this->m_pProtocol = _type.m_pProtocol;
}

AgvType::~AgvType()
{
}

bool AgvType::operator==(const bool &_bool) const
{
    bool _b = true;

    if(m_name.empty() || m_type == 0 || m_maxSpeed == 0.0f || m_pProtocol == nullptr)
    {
        _b = false;
    }

    return _b == _bool;
}

void AgvType::operator=(const AgvType &_type)
{
    m_name = _type.m_name;
    m_type = _type.m_type;
    m_brand = _type.m_brand;
    m_version = _type.m_version;
    m_maxSpeed = _type.m_maxSpeed;
    m_maxWeight = _type.m_maxWeight;
    m_pProtocol = _type.m_pProtocol;
}

bool AgvType::IsNull() const
{
    if(m_name.empty() || m_type == 0 || m_maxSpeed == 0.0f || m_pProtocol == nullptr)
    {
        return false;
    }

    return true;
}

AgvType::operator bool() const
{
    if(m_name.empty() || m_type == 0 || m_maxSpeed == 0.0f || m_pProtocol == nullptr)
    {
        return false;
    }

    return true;
}
