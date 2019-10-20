#include "RfidBase.h"

#include "AgvBase.h"

RfidBase::RfidBase()
{
    m_id = 0;
    m_pLocker = nullptr;
    m_pPeerLocker = nullptr;
    m_lockTime = std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());
}

RfidBase::RfidBase(const RfidBase::Rfid_t &_no)
{
    m_id = _no;
    m_pLocker = nullptr;
    m_pPeerLocker = nullptr;
    m_lockTime = std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());
}

RfidBase::RfidBase(const RfidBase &_rfid)
{
    m_id = _rfid.m_id;
    m_pLocker = _rfid.m_pLocker;
    m_pPeerLocker = _rfid.m_pPeerLocker;
    m_lockTime = _rfid.m_lockTime;
}

void RfidBase::operator=(const RfidBase &_rfid)
{
    m_id = _rfid.m_id;
    m_pLocker = _rfid.m_pLocker;
    m_pPeerLocker = _rfid.m_pPeerLocker;
    m_lockTime = _rfid.m_lockTime;
}

bool RfidBase::operator==(const bool &_bool) const
{
    if(m_id == 0)
    {
        return false == _bool;
    }

    return true == _bool;
}

bool RfidBase::operator!=(const bool &_bool) const
{
    if(m_id == 0)
    {
        return false != _bool;
    }

    return true != _bool;
}

bool RfidBase::operator>(const RfidBase &_rfid) const
{
    // 为0的时间为无效时间
    // 任何有效时间与无效时间对比
    // 有效时间 小于 无效时间

    auto _left = m_lockTime - std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());
    auto _right = m_lockTime - std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());
    auto _dis =  std::chrono::duration_cast<std::chrono::milliseconds>(m_lockTime - _rfid.m_lockTime);

    if(_dis.count() > 0)
    {
        // 时间对比结果,当期时间大于比对时间

        if(_right.count() == 0)
        {
            // 比对的时间为0,则比对时间无效,当前时间小
            return false;
        }

        // 比对的时间不为0,则当前时间大
        return true;
    }

    // 时间对比结果,当前时间小于等于比对时间
    if(_left.count() == 0 && _left.count()!= _right.count())
    {
        // 当前时间为0,且当前时间与比对时间不相等,则当前时间大,比对时间小
        return true;
    }

    return false;
}

bool RfidBase::operator<(const RfidBase &_rfid) const
{
    // 为0的时间为无效时间
    // 任何有效时间与无效时间对比
    // 有效时间 小于 无效时间

    auto _left = m_lockTime - std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());
    auto _right = m_lockTime - std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());
    auto _dis =  std::chrono::duration_cast<std::chrono::milliseconds>(m_lockTime - _rfid.m_lockTime);

    if(_dis.count() < 0)
    {
        // 时间对比结果,当期时间小于比对时间

        if(_left.count() == 0)
        {
            // 当前的时间为0,则比对时间小
            return false;
        }

        // 当前的时间不为0,则比对时间大
        return true;
    }

    // 时间对比结果,当前时间大于等于比对时间
    if(_right.count() == 0 && _left.count() != _right.count())
    {
        // 比对时间为0,且当前时间与比对时间不相等,则比对时间大,当前时间小
        return true;
    }

    return false;
}

RfidBase::operator bool() const
{
    if(m_id == 0)
    {
        return false;
    }

    return true;
}

bool RfidBase::Lock(void *_locker)
{
    if(m_pLocker != nullptr)
    {
        if(m_pLocker != _locker)
        {
            return false;
        }

        return true;
    }

    m_pLocker = _locker;

    m_lockTime = std::chrono::steady_clock::now();

    if(m_pPeerLocker == _locker)
    {
        Cancel();
    }

    return true;
}

bool RfidBase::Free(void *_locker)
{
    if(_locker == nullptr || m_pLocker == _locker)
    {
        m_pLocker = nullptr;
        m_lockTime = std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration::zero());

        return true;
    }

    return false;
}

bool RfidBase::PeerLock(void *_locker)
{
    if(m_pPeerLocker != nullptr)
    {
        if(m_pPeerLocker != _locker)
        {
            return false;
        }

        return true;
    }

    m_pPeerLocker = _locker;

    return true;
}

bool RfidBase::Cancel(void *_locker)
{
    if(_locker == nullptr || m_pPeerLocker == _locker)
    {
        m_pPeerLocker = nullptr;

        return true;
    }

    return false;
}

void RfidBase::Clear()
{
    m_pLocker = nullptr;
    m_pPeerLocker = nullptr;

    return;
}

void *RfidBase::GetLocker() const
{
    return m_pLocker;
}

void *RfidBase::GetPeerLocker() const
{
    return m_pPeerLocker;
}
