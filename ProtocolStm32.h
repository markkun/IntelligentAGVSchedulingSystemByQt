#ifndef PROTOCOLSTM32_H
#define PROTOCOLSTM32_H

#include "ProtocolBase.h"

class ProtocolStm32 : public ProtocolBase
{
public:
    ProtocolStm32();
    ~ProtocolStm32() override;

public:
    typedef short DATA_LEN;

protected:
    static const unsigned char PACKET_HEAD; /*!< 报文头 */
    static const unsigned char PACKET_TAIL; /*!< 报文尾 */

public:
    QByteArrayList ProcessData(QByteArray &_data) override;
    QByteArray CreatePacket(const QByteArray &_data) override;
    QByteArray Encoding(const QByteArray& _data);
    QByteArray Encoding(const char* _data,const size_t& _size);
    QByteArray Decoding(const QByteArray& _data);
    QByteArray Decoding(const char* _data,const size_t& _size);
};

#endif // PROTOCOLSTM32_H
