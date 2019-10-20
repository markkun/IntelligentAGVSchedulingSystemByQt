/*!
 * @file ProtocolBase
 * @brief 描述协议类型的文件
 * @date 2019-10-21
 * @author FanKaiyu
 * @version 1.0
 */
#ifndef PROTOCOLBASE_H
#define PROTOCOLBASE_H

#include <QObject>

/*!
 * @class ProtocolBase
 * @brief 描述协议的类型的基类
 */
class ProtocolBase
{
public:
    ProtocolBase(const unsigned char& _type);
    virtual ~ProtocolBase();

public:
    typedef short CRC_16;

protected:
    unsigned char m_type;

protected:
    static unsigned char auchCRCHi[];
    static unsigned char auchCRCLo[];

public:
    virtual QByteArrayList ProcessData(QByteArray& _data) = 0;
    virtual QByteArray CreatePacket(const QByteArray& _data) = 0;

protected:
    static short CRC16(const char *puchMsg,unsigned int _len);

public:
    unsigned char GetType() const;
};

/*! @brief 协议类型 */
enum ProtocolType
{
    Protocol_PLC,   /*!< 与PLC通信的协议 */
    Protocol_STM32, /*!< 与STM32通信的协议 */
};

#endif // PROTOCOLBASE_H
