
#ifndef __NETWORK_INTERFACE_H__
#define __NETWORK_INTERFACE_H__

#include <qobject.h>
#include "network/mac.h"
#include "network/phy.h"
#include "pos.h"
#include "network/MetaData.h"

class NI // Network Interface 
	: public QObject
{
Q_OBJECT 
public:
	enum NI_STATE{ // 分為三種情況
		NI_SEND, // 目前正在傳送
		NI_RECV, // 目前正在接收
		NI_IDLE  // 目前是閒置的
	};
	NI( QObject * parent = 0, const char * name = 0 );

	virtual bool BrSend(Packet* pkt ) = 0; // 虛擬函數
	virtual bool BrRecv(Packet* pkt, MetaData md) = 0; // 虛擬函數

	/*
	virtual int GetState() = 0;
	virtual bool isBusy() = 0;
	virtual bool isIdle() = 0;
	*/

public slots:
	virtual void dataRecv(Packet*) = 0; //arise from mac layer

protected:
	NI_STATE _ni_state;
};

#endif 
