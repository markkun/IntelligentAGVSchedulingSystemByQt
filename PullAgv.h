#ifndef PULLAGV_H
#define PULLAGV_H

#include "AgvBase.h"

class PullAgv : public AgvBase
{
    Q_OBJECT
public:
    explicit PullAgv(const AgvType& _type,const AId_t& _id,
                     const bool& _bClient,const QString& _peerAddr,const unsigned short& _peerPort,
                     const QString& _localAddr = "",const unsigned short& _localPort = 0,
                     QObject *parent = nullptr);
};

#endif // PULLAGV_H
