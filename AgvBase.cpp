#include "AgvBase.h"

AgvBase::AgvBase(const AgvType* _type,const unsigned short& _id,QObject *parent) : QObject(parent)
{
    Initialize(_type,_id);
}

AgvBase::~AgvBase()
{
    if(m_pSocket)
    {
        m_pSocket->close();

        m_pSocket->waitForDisconnected(-1);
    }
}

void AgvBase::Initialize(const AgvType* _type, const unsigned short &_id)
{
    if(_type == nullptr || _type)
    {
        throw("The AGV type cannot be empty");
    }

    this->m_pType = _type;
    this->m_id = _id;

    InitAttribute();
}

void AgvBase::InitAttribute()
{
    this->m_mode = Mode_Hand;
    this->m_status = Sta_Wait;
    this->m_battery = 0;
    this->m_speed = 0;
    this->m_cargo = 0;

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

unsigned short AgvBase::GetID() const
{
    return m_id;
}

bool AgvBase::UpdateMode(const unsigned char &_mode)
{
    if(m_mode != _mode)
    {
        m_mode = _mode;
        return true;
    }

    return false;
}

unsigned char AgvBase::GetMode() const
{
    return m_mode;
}

bool AgvBase::UpdateStatus(const unsigned char &_status)
{
    if(m_status != _status)
    {
        m_status = _status;
        return true;
    }

    return false;
}

unsigned char AgvBase::GetStatus() const
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

bool AgvBase::UpdateSpeed(const char &_speed)
{
    if(m_speed != _speed)
    {
        m_speed = _speed;
        return true;
    }

    return false;
}

char AgvBase::GetSpeed() const
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

bool AgvBase::UpdateBattery(const unsigned char& _battery)
{
    if(m_battery != _battery)
    {
        m_battery = _battery;
        return true;
    }

    return false;
}

unsigned char AgvBase::GetBattery() const
{
    return m_battery;
}

bool AgvBase::UpdateCargo(const unsigned char &_cargo)
{
    if(m_cargo != _cargo)
    {
        m_cargo = _cargo;
        return true;
    }

    return false;
}

unsigned char AgvBase::GetCargo() const
{
    return m_cargo;
}

bool AgvBase::UpdateAction(const unsigned char &_action, const unsigned char &_actStatus)
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

unsigned char AgvBase::GetAction() const
{
    return m_action;
}

unsigned char AgvBase::GetActionStatus() const
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

size_t AgvBase::GetAction(unsigned char &_action, unsigned char &_actStatus) const
{
    _action = m_action;
    _actStatus = m_actStatus;

    return GetActionExecutionTime();
}

bool AgvBase::UpdateCurRfid(const unsigned short &_rfid)
{
    if(m_curRfid != _rfid)
    {
        m_oldRfid = m_curRfid;
        m_curRfid = _rfid;

        return true;
    }

    return false;
}

unsigned short AgvBase::GetCurRfid() const
{
    return m_curRfid;
}

unsigned short AgvBase::GetOldRfid() const
{
    return m_oldRfid;
}

bool AgvBase::UpdateEndRfid(const unsigned short &_rfid)
{
    if(m_endRfid != _rfid)
    {
        m_oldEndRfid = m_endRfid;
        m_endRfid = _rfid;

        return true;
    }

    return false;
}

unsigned short AgvBase::GetEndRfid() const
{
    return m_endRfid;
}

unsigned short AgvBase::GetOldEndRfid() const
{
    return m_oldEndRfid;
}

void AgvBase::Connect()
{
    if(m_bClient)
    {
        return;
    }

    if(m_peerAddr.isNull() || m_peerAddr.isEmpty())
    {
        throw("The IP address cannot be empty");
    }

    m_pSocket = new QTcpSocket();

    connect(m_pSocket,SIGNAL(QTcpSocket::disconnected),this,SLOT(AgvBase::DisConnected));
    connect(m_pSocket,SIGNAL(QTcpSocket::error),this,SLOT(AgvBase::Error));
    connect(m_pSocket,SIGNAL(QTcpSocket::connected),this,SLOT(AgvBase::Connected));
    connect(m_pSocket,SIGNAL(QTcpSocket::readyRead),this,SLOT(AgvBase::ReadData));

    if((m_localAddr.isNull() == false && m_localAddr.isEmpty() == false) || m_localPort !=0)
    {
        QHostAddress _addr;

        if(m_localAddr.isNull() == false && m_localAddr.isEmpty() == false)
        {
            _addr.setAddress(m_localAddr);
        }

         m_pSocket->bind(_addr,m_localPort);
    }

    m_pSocket->connectToHost(m_peerAddr,m_peerPort);

    return;
}

bool AgvBase::Connect(QTcpSocket &_socket)
{
    if(m_bClient == false)
    {
        return false;
    }

    if(m_pSocket == &_socket)
    {
        return true;
    }

    if(m_peerAddr.isNull() || m_peerAddr.isEmpty())
    {
        return false;
    }

    if(m_peerAddr != _socket.peerName())
    {
        return false;
    }

    if(m_peerPort != 0 && m_peerPort != _socket.peerPort())
    {
        return false;
    }

    if(m_pSocket)
    {
        m_pSocket->close();
        m_pSocket->waitForDisconnected(-1);
    }

    m_pSocket = &_socket;

    connect(m_pSocket,SIGNAL(QTcpSocket::disconnected),this,SLOT(AgvBase::DisConnected));
    connect(m_pSocket,SIGNAL(QTcpSocket::readyRead),this,SLOT(AgvBase::ReadData));

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

void AgvBase::DisConnected()
{
    if(m_pSocket)
    {
        m_pSocket->deleteLater();
    }

    m_pSocket = nullptr;

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
    }

    return;
}

AgvType::AgvType()
{
    this->m_name = "";
    this->m_type = 0;
    this->m_brand = "";
    this->m_version = "";
    this->m_maxSpeed = 0.0f;
    this->m_maxWeight = 0.0f;
}

AgvType::AgvType(const std::string &_name, const unsigned char &_type, const float &_speed, const float &_weright, const std::string &_brand, const std::string &_version)
{
    this->m_name = _name;
    this->m_type = _type;
    this->m_brand = _brand;
    this->m_version = _version;
    this->m_maxSpeed = _speed;
    this->m_maxWeight = _weright;
}

AgvType::AgvType(const AgvType &_type)
{
    this->m_name = _type.m_name;
    this->m_type = _type.m_type;
    this->m_brand = _type.m_brand;
    this->m_version = _type.m_version;
    this->m_maxSpeed = _type.m_maxSpeed;
    this->m_maxWeight = _type.m_maxWeight;
}

AgvType::operator bool()
{
    if(m_name.empty() || m_type == 0 || m_maxSpeed == 0.0f)
    {
        return false;
    }

    return true;
}
