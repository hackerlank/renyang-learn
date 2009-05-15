

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <netdb.h>
#include <string.h>

#include "Error.h"

#include "SctpSocketHandler.h"

/* static functions */
int SctpSocketHandler::SctpSetMaxStream(int sd , int num = 5)
{
	int ret;

	struct sctp_initmsg initmsg;

	memset( &initmsg, 0, sizeof(initmsg) );
	initmsg.sinit_num_ostreams = num; 
	initmsg.sinit_max_instreams = num;
	initmsg.sinit_max_attempts = num - 1;
	ret = setsockopt( sd , IPPROTO_SCTP, SCTP_INITMSG,
			&initmsg, sizeof(initmsg) );
	return ret;
}

int SctpSocketHandler::SctpGetAssocID(int sd)
{
	struct sctp_status status;
	int ret,in;

	in = sizeof(status);
	ret = getsockopt( sd , SOL_SCTP, SCTP_STATUS,
			(void *)&status, (socklen_t *)&in );

	return status.sstat_assoc_id; 
}

int SctpSocketHandler::SctpEnable(int sd)
{
	 struct sctp_event_subscribe events;
	int ret;

	/* Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg */
	memset( (void *)&events, 0, sizeof(events) );
	events.sctp_data_io_event = 1;
	ret = setsockopt( sd , SOL_SCTP, SCTP_EVENTS,
			(const void *)&events, sizeof(events) );

	if( ret < 0 )
		throw Error(QString("SctpSocketHandler::SctpEnable Error"));

	return ret;
}

/* static functions */
int SctpSocketHandler::SctpSetNoDelay(int sd)
{
	int ret; 
	int flag = 1;
	ret = setsockopt( sd , IPPROTO_SCTP, SCTP_NODELAY,
			&flag, sizeof(flag) );
	return ret;
}


int SctpSocketHandler::SctpGetRtoMax(int sd)
{
	struct sctp_rtoinfo rto_info;
	int ret,in;

	in = sizeof(rto_info);
	ret = getsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info, (socklen_t *)&in );

	if( ret < 0 )
		throw Error(QString("SctpSocketHandler::SctpGetRtoMax Error"));

	return rto_info . srto_max; 
}

int SctpSocketHandler::SctpGetRtoMin(int sd)
{
	struct sctp_rtoinfo rto_info;
	int ret,in;

	in = sizeof(rto_info);
	ret = getsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info, (socklen_t *)&in );

	if( ret < 0 )
		throw Error(QString("SctpSocketHandler::SctpGetRtoMin Error"));

	return rto_info . srto_min; 
}

int SctpSocketHandler::SctpSetRtoMax(int sd , int n)
{
	struct sctp_rtoinfo rto_info;
	int ret,in;

	in = sizeof(rto_info);
	ret = getsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info, (socklen_t *)&in );

	rto_info . srto_max = n; 
	ret = setsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info, in );

	return ret;
}

int SctpSocketHandler::SctpSetRtoMin(int sd , int n)
{
	struct sctp_rtoinfo rto_info;
	int ret,in;

	in = sizeof(rto_info);
	ret = getsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info,(socklen_t *)& in );

	rto_info . srto_min = n; 
	ret = setsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info, in );

	return ret; 
}


int SctpSocketHandler::SctpSetAutoClose(int sd , int n)
{
	int ret;

	ret = setsockopt(sd, IPPROTO_SCTP, SCTP_AUTOCLOSE, &n, sizeof (n)); 

	return ret; 
}

int SctpSocketHandler::SctpTurnOnAllEvent(int sd)
{
	int ret;
	struct sctp_event_subscribe event;
	int n = sizeof(event);

	event . sctp_data_io_event = 1;
	event . sctp_association_event = 1;
	event . sctp_address_event = 1;
	event . sctp_send_failure_event = 1;
	event . sctp_peer_error_event = 1;
	event . sctp_shutdown_event = 1;
	event . sctp_partial_delivery_event = 1;
	event . sctp_adaptation_layer_event = 1;

	ret = setsockopt(sd, IPPROTO_SCTP, SCTP_EVENTS, &event , n); 

	return ret; 
}

int SctpSocketHandler::SctpTurnOffAllEvent(int sd)
{
	int ret;
	struct sctp_event_subscribe event;
	int n = sizeof(event);

	event .  sctp_data_io_event = 0;
	event . sctp_association_event = 0;
	event . sctp_address_event = 0;
	event . sctp_send_failure_event = 0;
	event . sctp_peer_error_event = 0;
	event . sctp_shutdown_event = 0;
	event . sctp_partial_delivery_event = 0;
	event . sctp_adaptation_layer_event = 0;

	ret = setsockopt(sd, IPPROTO_SCTP, SCTP_EVENTS, &event , n); 

	return ret; 
}

QString SctpSocketHandler::SctpGetPrim(int sd)
{
}

void SctpSocketHandler :: SctpSetPrim(int sd)
{
}

void SctpSocketHandler :: SctpSetPeerPrim(int sd)
{
}
