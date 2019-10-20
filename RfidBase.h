#ifndef RFIDBASE_H
#define RFIDBASE_H
#include <chrono>

class RfidBase
{
public:
    typedef unsigned short Rfid_t;
public:
    RfidBase();
    RfidBase(const Rfid_t& _no);
    RfidBase(const RfidBase& _rfid);

public:
    operator bool() const;
    void operator=(const RfidBase& _rfid);
    bool operator==(const bool& _bool) const;
    bool operator!=(const bool& _bool) const;
    bool operator>(const RfidBase& _rfid) const;
    bool operator<(const RfidBase& _rfid) const;

protected:
    Rfid_t m_id;                                        /*!< 编号 */
    std::chrono::steady_clock::time_point m_lockTime;   /*!< 锁定RFID卡的时间 */
    void* m_pLocker;                                    /*!< 锁定RFID卡的对象 */
    void* m_pPeerLocker;                                /*!< 远程锁定RFID卡的对象:即对象不在RFID卡上,提前锁定RFID卡 */

public:
    bool Lock(void* _locker);
    bool Free(void* _locker = nullptr);
    bool PeerLock(void* _locker);
    bool Cancel(void* _locker = nullptr);
    void Clear();
    void* GetLocker() const;
    void* GetPeerLocker() const;
};

#endif // RFIDBASE_H
