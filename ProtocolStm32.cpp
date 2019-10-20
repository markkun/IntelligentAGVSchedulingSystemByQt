#include "ProtocolStm32.h"

#include <string.h>

const unsigned char ProtocolStm32::PACKET_HEAD = static_cast<unsigned char>(0xBA);
const unsigned char ProtocolStm32::PACKET_TAIL = static_cast<unsigned char>(0xBE);

ProtocolStm32::ProtocolStm32() : ProtocolBase(ProtocolType::Protocol_STM32)
{

}

QByteArrayList ProtocolStm32::ProcessData(QByteArray &_data)
{
    QByteArrayList _list;

    const unsigned int _sizeHead = sizeof(PACKET_HEAD);                                         /*!< 报文头大小 */
    const unsigned int _sizeTail = sizeof(PACKET_TAIL);                                         /*!< 报文尾大小 */
    const unsigned int _sizeLen = sizeof(DATA_LEN);                                             /*!< 数据长度大小 */
    const unsigned int _sizeCrc = sizeof(CRC_16);                                               /*!< 校验码大小 */

    const unsigned int MIN_PACKET_LEN = _sizeHead+_sizeLen+_sizeCrc+_sizeTail;                  /*!< 最小数据长度 */

    const char* _begin = _data.data();                                                          /*!< 指向数据起始位的指针 */
    const unsigned int _size = static_cast<unsigned int>(_data.size());                         /*!< 数据的大小 */
    const char* _last = const_cast<char*>(_begin);                                              /*!< 指向待处理数据的指针 */
    unsigned int _lastSize = _size;                                                             /*!< 待处理数据的大小 */

    while(1)
    {
        if(_lastSize < MIN_PACKET_LEN)
        {
            // 待处理数据太少
            break;
        }

        // 1、查找报文头
        const char* _head = reinterpret_cast<char*>(memchr(_last,PACKET_HEAD,_lastSize));       /*!< 指向报文头的指针 */

        if(_head == nullptr)
        {
            // 在数据中未找到报文头
            _lastSize = 0;
            break;
        }

        // 舍弃报文头前无效的数据
        _last = _head;
        _lastSize = _size - static_cast<unsigned int>(_head - _begin);

        // 2、查找报文尾
        char* _tail = reinterpret_cast<char*>(memchr(_last + 1,PACKET_TAIL,_lastSize -1));      /*!< 指向报文尾的指针 */

        if(_tail == nullptr)
        {
            // 在数据中未找到报文尾,数据未接收完全
            break;
        }

        // 3、反转义获取源数据
        // 3.1、获取数据内容
        const char* _data = _head + _sizeHead;                                                  /*!< 指向数据内容的指针 */
        unsigned int _dataSzie = static_cast<unsigned int>(_tail - _data);                      /*!< 数据内容的大小 */

        // 4、 反转义获取源数据
        QByteArray _source = Decoding(_data,_dataSzie);                                         /*!< 源数据 */
        const char* _srcData = _source.data();                                                  /*!< 指向源数据起始地址的指针 */
        const unsigned int _srcSize = static_cast<unsigned int>(_source.size());                /*!< 源数据大小 */

        // 5、获取数据长度
        unsigned int _packetSize = 0;                                                           /*!< 报文上传的数据长度 */
        // 获取报文长度
        for(int i = 0; i < static_cast<int>(sizeof(DATA_LEN));++i)
        {
            _packetSize |= static_cast<unsigned int>(*(_srcData + i) << (8 * i));
        }

        // 7、获取数据校验码
        const char* _crcPtr = _srcData + (_packetSize - _sizeHead - _sizeCrc - _sizeTail);      /*!< 指向报文上传的CRC校验码起始位的指针 */
        CRC_16 _packetCrc =static_cast<CRC_16>((*_crcPtr) << 8 | *(_crcPtr + 1));               /*!< 报文上传的CRC校验码 */
        CRC_16 _srcCrc = CRC16(_srcData ,_srcSize - _sizeCrc);                                  /*!< 获取数据的实际校验码 */

        // 8、校验数正确性
        if(_srcSize + _sizeHead + _sizeTail == _packetSize && _srcCrc == _packetCrc)
        {
            // 长度与校验码校验通过校验
            _list.push_back(QByteArray(_srcData,static_cast<int>(_srcSize - _sizeCrc)));
        }

        _last = _tail + sizeof(PACKET_TAIL);
        _lastSize = _size - static_cast<unsigned int>(_last - _begin);
    }

    if(_lastSize > 0)
    {
        _data.clear();
        _data = QByteArray(_last,static_cast<int>(_lastSize));
    }
    else
    {
        _data.clear();
    }

    return _list;
}

QByteArray ProtocolStm32::CreatePacket(const QByteArray &_data)
{
    const unsigned int _sizeHead = sizeof(PACKET_HEAD);                                         /*!< 报文头大小 */
    const unsigned int _sizeTail = sizeof(PACKET_TAIL);                                         /*!< 报文尾大小 */
    const unsigned int _sizeLen = sizeof(DATA_LEN);                                             /*!< 数据长度大小 */
    const unsigned int _sizeCrc = sizeof(CRC_16);                                               /*!< 校验码大小 */

    unsigned int _tmpLen = _sizeLen + static_cast<unsigned int>(_data.size()) + _sizeCrc;       /*!< 数据体长度 */
    char * _packet = new char[_tmpLen];                                                         /*!< 数据体数组 */

    // 初始化数据体
    memset(_packet,0,_tmpLen);

    unsigned int _packetSize = _sizeHead + _tmpLen + _sizeTail;                                 /*!< 数据总长度 */

    // 储存数据长度
    for(unsigned int i = _sizeLen; i > 0; --i)
    {
        _packet[_sizeLen - i] = static_cast<char>((_packetSize >> 8 * (i-1)) & 0xFF);
    }

    // 储存数据内容
    memcpy(_packet + _sizeLen,_data.data(), static_cast<unsigned int>(_data.size()));

    // 获取校验码
    CRC_16 _crc = CRC16(_packet,_tmpLen - _sizeCrc);

    // 储存校验码
    for(unsigned int i = 1;i<= _sizeCrc;++i)
    {
        _packet[_tmpLen - i] = static_cast<char>((_crc >> 8 * (i-1)) & 0xFF);
    }

    // 转义数据
    QByteArray tf = Encoding(_packet,_tmpLen);

    // 释放内存空间
    delete[] _packet;

    // 获取新的数据长度
    _packetSize = _sizeHead + _sizeTail + static_cast<unsigned int>(tf.size());

    // 创建内存空间
    _packet = new char[_packetSize];

    // 存放报文头
    for(unsigned int i = _sizeHead; i > 0; --i)
    {
        _packet[_sizeHead - i] = static_cast<char>((PACKET_HEAD >> 8 * (i-1)) & 0xFF);
    }

    // 存放数据体
    memcpy(_packet + _sizeHead,tf.data(), static_cast<unsigned int>(tf.size()));

    // 存放报文尾
    for(unsigned int i = 1;i<= _sizeTail;++i)
    {
        _packet[_packetSize - i] = static_cast<char>((PACKET_TAIL >> 8 * (i-1)) & 0xFF);
    }

    QByteArray _package = QByteArray(_packet,static_cast<int>(_packetSize));                       /*!< 报文包 */

    // 释放内存空间
    delete[] _packet;

    return _package;
}

QByteArray ProtocolStm32::Encoding(const QByteArray &_data)
{
    return Encoding(_data.data(),static_cast<size_t>(_data.size()));
}

QByteArray ProtocolStm32::Encoding(const char *_data, const size_t &_size)
{
    char* _encode = new char[_size * 2];

    size_t _len = 0;
    for(size_t i = 0; i < _size;++i,++_len)
    {
        switch(_data[i])
        {
        case 0xB0:
            _encode[_len] = static_cast<char>(0xB0);
            _encode[++_len] = 0x00;
            break;
        case 0xBA:
            _encode[_len] = static_cast<char>(0xB0);
            _encode[++_len] = 0x02;
            break;
        case 0xBE:
            _encode[_len] = static_cast<char>(0xB0);
            _encode[++_len] = 0x01;
            break;
        default:
            _encode[_len] = _data[i];
            break;
        }
    }

    QByteArray _tf(_encode,static_cast<int>(_len));

    return _tf;
}

QByteArray ProtocolStm32::Decoding(const QByteArray &_data)
{
    return Decoding(_data.data(),static_cast<size_t>(_data.size()));
}

QByteArray ProtocolStm32::Decoding(const char *_data, const size_t &_size)
{
    char* _decode = new char[_size];

    size_t _len = 0;
    for(size_t i = 0; i < _size;++i,++_len)
    {
        if(_data[i] == static_cast<char>(0xB0))
        {
            switch(_data[++i])
            {
            case 0x00:
                _decode[_len] = static_cast<char>(0xB0);
                break;
            case 0x01:
                _decode[_len] = static_cast<char>(0xBA);
                break;
            case 0x02:
                _decode[_len] = static_cast<char>(0xBE);
                break;
            }
        }
        else
        {
            _decode[_len] = _data[i];
        }
    }

    QByteArray _src(_decode,static_cast<int>(_len));

    return _src;
}
