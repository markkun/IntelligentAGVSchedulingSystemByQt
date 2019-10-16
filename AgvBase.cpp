#include "AgvBase.h"

AgvBase::AgvBase(const AGVTYPE& _type,const AGVID& _id)
{
    Initialize(_type,_id);
}

void AgvBase::Initialize(const AGVTYPE& _type,const AGVID& _id)
{
    m_byType = _type;
    m_usID = _id;
}

void AgvBase::InitStatus()
{
    m_byStatus = STA_WAIT;
    m_byBattery = 0;
    m_chSpeed = 0;
    m_chError = ERR_NONE;
    m_byAct = 0;
    m_byActSta = ACT_NONE;
    m_byCargo = CRG_EMTPY;
    m_byMode = MODE_HAND;

    m_curLocat = AgvLocation(0);
    m_endLocat = AgvLocation(0);

    m_oldLocat = m_curLocat;
    m_oldEndLocat = m_endLocat;
}

AGVTYPE AgvBase::GetType() const
{
    return m_byType;
}

unsigned short AgvBase::GetID() const
{
    return m_usID;
}

bool AgvBase::UpdateStatus(const AGVSTA& _sta)
{
    if(m_byStatus != _sta)
    {
        m_byStatus = _sta;
        return true;
    }

    return false;
}

AGVSTA AgvBase::GetStatus() const
{
    return m_byStatus;
}

std::string AgvBase::GetStatusText() const
{
    std::string _str = "unknown";

    switch(m_byStatus)
    {
    case STA_WAIT:
        _str = "待机";
        break;
    case STA_RUN:
        _str = "运行";
        break;
    case STA_STOP:
        _str = "停止";
        break;
    case STA_PAUSE:
        _str = "暂停";
        break;
    case STA_SLEEP:
        _str = "休眠";
        break;
    case STA_CHARGE:
        _str = "充电";
        break;
    case STA_SCREAM:
        _str = "急停(手动)";
        break;
    case STA_OBSDOWN:
        _str = "避障减速";
        break;
    case STA_SPEEDUP:
        _str = "加速";
        break;
    case STA_TRASTOP:
        _str = "交通管制";
        break;
    case STA_ALLSCREAM:
        _str = "急停(全线)";
        break;
    case STA_RMTSCREAM:
        _str = "急停(远程)";
        break;
    case STA_SPEEDDOWN:
        _str = "减速";
        break;
    default:
        _str = "unknown";
        break;
    }

    return _str;
}

bool AgvBase::UpdateBattery(const AGVBATTERY& _battery)
{
    if(m_byBattery != _battery)
    {
        m_byBattery = _battery;
        return true;
    }

    return false;
}

AGVBATTERY AgvBase::GetBattery() const
{
    return m_byBattery;
}

bool AgvBase::UpdateError(const AGVERR& _error)
{
    if(m_chError != _error)
    {
        m_chError = _error;
        return true;
    }

    return false;
}

AGVERR AgvBase::GetError() const
{
    return m_chError;
}

std::string AgvBase::GetErrorText() const
{
    std::string _str = "unknown";

    switch(m_chError)
    {
    case ERR_ARM:
        _str = "机械臂异常:机械臂执行动作超时未完成";
        break;
    case ERR_NET:
        _str = "网络异常:与设备之间的网络中断";
        break;
    case ERR_OBS:
        _str = "移动异常:检测到运行路线上存在障碍物,已停止移动";
        break;
    case ERR_MISS:
        _str = "移动异常:检测磁轨超时,已停止移动";
        break;
    case ERR_MOBS:
        _str = "移动异常:检测到碰撞物体,已停止移动";
        break;
    case ERR_NONE:
        _str = "无异常";
        break;
    case ERR_LIFTER:
        _str = "升降杆异常:升降杆执行动作超时未完成";
        break;
    case ERR_ROLLER:
        _str = "辊筒异常:辊筒执行动作超时未完成";
        break;
    default:
        _str = "unknown";
        break;
    }

    return _str;
}

bool AgvBase::UpdateSpeed(const AGVSPEED& _speed)
{
    if(m_chSpeed != _speed)
    {
        m_chSpeed = _speed;
        return true;
    }

    return false;
}

AGVSPEED AgvBase::GetSpeed() const
{
    return m_chSpeed;
}

bool AgvBase::UpdateAction(const AGVACT& _act, const AGVACTSTA& _sta)
{
    // 更新标识
    bool bUpdate = false;

    // 对比动作信息
    if(m_byAct != _act)
    {
        bUpdate = true;

        m_byAct = _act;
    }

    // 对比动作状态
    if(m_byActSta != _sta)
    {
        bUpdate = true;

        m_byActSta = _sta;
    }

    // 状态更新
    if(bUpdate)
    {
        switch(m_byActSta)
        {
        case ACT_NONE:
            break;
        case ACT_EXE:
            // 记录动作开始执行的时间
            m_actCount = std::chrono::steady_clock::now();
            break;
        case ACT_FIN:
            break;
        default:
            break;
        }
    }

    return bUpdate;
}

AGVACT AgvBase::GetAction() const
{
    return m_byAct;
}

AGVACTSTA AgvBase::GetActStatus() const
{
    return m_byActSta;
}

size_t AgvBase::GetActExeTime() const
{
    switch(m_byActSta)
    {
    case ACT_NONE:
        return 0;
    case ACT_FIN:
        return 0;
    }

    std::chrono::steady_clock::time_point tNow = std::chrono::steady_clock::now();
    std::chrono::steady_clock::duration d = tNow - m_actCount;

    if (d == std::chrono::steady_clock::duration::zero())
    {
        return 0;
    }

    return static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(d).count());
}

size_t AgvBase::GetAction(AGVACT &_act, AGVACTSTA _sta) const
{
    _act = m_byAct;
    _sta = m_byActSta;
    return GetActExeTime();
}

bool AgvBase::UpdateCargo(const AGVCRG& _cargo)
{
    if(m_byCargo != _cargo)
    {
        m_byCargo = _cargo;
        return true;
    }

    return false;
}

AGVCRG AgvBase::GetCargo() const
{
    return m_byCargo;
}

bool AgvBase::UpdateMode(const AGVMODE& _mode)
{
    if(m_byMode != _mode)
    {
        m_byMode = _mode;
        return true;
    }

    return false;
}

AGVMODE AgvBase::GetMode() const
{
    return m_byMode;
}

bool AgvBase::UpdateCurLocation(const AgvLocation &_locat)
{
    if(m_curLocat != _locat)
    {
        m_oldLocat = m_curLocat;
        m_curLocat = _locat;

        return true;
    }

    return false;
}

AgvLocation AgvBase::GetCurLocation() const
{
    return m_curLocat;
}

bool AgvBase::UpdateEndLocation(const AgvLocation &_locat)
{
    if(m_endLocat != _locat)
    {
        m_oldEndLocat = m_endLocat;
        m_endLocat = _locat;
        return true;
    }

    return false;
}

AgvLocation AgvBase::GetEndLocation() const
{
    return m_endLocat;
}

AgvLocation AgvBase::GetOldEndLocation() const
{
    return m_oldEndLocat;
}
