/*
 *  IHU -- I Hear U, easy VoIP application using Speex and Qt
 *
 *  Copyright (C) 2003-2008 Matteo Trotta - <mrotta@users.sourceforge.net>
 *
 *  http://ihu.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 */

#include <stdlib.h>

#include <qstring.h>
#include <qtimer.h>
 
#include "Error.h"
#include "IhuNoGui.hpp"

#define STAT_TIME 1000

// renyang - 透過configure檔案來初始化NoGui的IHU
// renyang - 這一個動作只是把外部的configure檔案複製一份到IhuNoGui這一個物件裡面
IhuNoGui::IhuNoGui(Config& ihucfg) : ihuconfig(ihucfg)
{
}

IhuNoGui::~IhuNoGui()
{
	delete phone;
	delete fileplayer;
	delete logger;
}

// renyang - 初始化IHU的設定
// renyang - 透過configure檔案初始化IhuNoGui這一個物件
void IhuNoGui::initIhu()
{
	try
	{
		if (!ihuconfig.checkConfig())
		{
			qWarning(QString("\nERROR: %1 contains invalid settings!\nPlease remove the file and restart IHU to restore settings.\nIHU will start with default settings now...\n").arg(ihuconfig.getFileName()));
			// renyang - 因為是用alias宣告ihuconfig, 因此改到ihuconfig就是改到外部的configure檔案
			// renyang - 所以當讀取configure檔案失敗時, 則設定configure為預設值
			ihuconfig.setDefault();
		}

		// renyang - 建立一個實際上在處理語音的類別Phone
		phone = new Phone(ihuconfig.getMaxCalls());
		// renyang - 建立一個撥放與轉換檔案的類別
		fileplayer = new FilePlayer();
		// renyang - 建立一個處理logger檔案的類別
		logger = new Logger();
		// renyang - 產生一個timer
		timer = new QTimer(this);

		// renyang - 當fileplayer表示編碼錯誤時, 會需要使用都輸入正確的編碼
		connect( fileplayer, SIGNAL(keyRequest()), this, SLOT(keyRequest()) );
		// renyang - 由fileplayer通知結束
		connect( fileplayer, SIGNAL(finish()), this, SLOT(stopFile()) );
		// renyang - FilePlayer出現警告訊息
		connect( fileplayer, SIGNAL(warning(QString)), this, SLOT(message(QString)) );
		// renyang - FilePlayer顯示錯誤訊息
		connect( fileplayer, SIGNAL(error(QString)), this, SLOT(abortAll(QString)) );
	
		// renyang - Phone接收到Call，通知IhuNoGui知道
		connect( phone, SIGNAL(receivedCallSignal(int)), this, SLOT(receivedCall(int)) );
		// renyang - Phone已接收連線, 通知IhuNoGui知道
		connect( phone, SIGNAL(connectedCallSignal(int)), this, SLOT(connectedCall(int)) );
		// renyang - Phone拒絕對方的連線要求
		connect( phone, SIGNAL(cancelCallSignal(int)), this, SLOT(cancelCall(int)) );
		// renyang - Phone整個出現錯誤, 要跳離程式???
		connect( phone, SIGNAL(abortSignal(QString)), this, SLOT(abortAll(QString)) );
		// renyang - Phone單一個功能出現錯誤???
		connect( phone, SIGNAL(abortCallSignal(int, QString)), this, SLOT(abortCall(int, QString)) );
		// renyang - Phone要傳送訊息
		connect( phone, SIGNAL(messageSignal(int, QString)), this, SLOT(message(int, QString)) );
		// renyang - 傳送警告訊息
		connect( phone, SIGNAL(warningSignal(QString)), this, SLOT(message(QString)) );
		// renyang - 顯示要使用新的key
		connect( phone, SIGNAL(newKeySignal(int, QString)), this, SLOT(receivedNewKey(int, QString)) );
		// renyang - 改變使用新的key
		connect( phone, SIGNAL(cryptedSignal(int)), this, SLOT(changeKey(int)) );

		// renyang - 每當timerout則取得最高峰
		connect( timer, SIGNAL(timeout()), this, SLOT(statistics()) );

		applySettings();

		phone->createCall();

		timer->start(STAT_TIME, false);
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void IhuNoGui::applySettings()
{
	// renyang - 設定Phone使用者的名字
	phone->setMyName(ihuconfig.getMyName());
	
	int abr = ihuconfig.getABR()*1000;
	int vbr = ihuconfig.getBitrateMode();
	switch (vbr)
	{
		case 0:
				abr = 0;
				break;
		case 1:
				abr = 0;
				break;
		case 2:
				vbr = 0;
				break;
	}
	// renyang - 預設quality為5
	float vbrquality = (float) ihuconfig.getVBRQuality();
	// renyang - 預設DTX為true
	int dtx = ihuconfig.getDTX();
	int th = -96;
	if (dtx)
		th = ihuconfig.getThreshold();

	// renyang - 設定使用的Recorder, 需要設定InputDriver,InputInterface
	// renyang - 這應該是記錄的部分
	phone->setupRecorder(ihuconfig.getInputDriver(), ihuconfig.getInputInterface());
	phone->setupPlayer(ihuconfig.getOutputInterface(), ihuconfig.getPrePackets());
	// renyang - 這個是撥放音訊的部分@@@
	fileplayer->setupPlayer(ihuconfig.getOutputInterface());
	
	// renyang - 設定phone的參數@@@
	phone->setup(ihuconfig.getSpeexMode(), ihuconfig.getQuality(), abr, vbr, vbrquality, ihuconfig.getComplexity(), ihuconfig.getVAD(), dtx, ihuconfig.getTXStop(), th, ihuconfig.getRingVolume());

	adrRefresh(ihuconfig.getADR());
	agcRefresh(ihuconfig.getAGC());

	try
	{
		// renyang - 設定存放記錄檔的檔案
		// renyang - 預設是沒有存放記錄檔的位置
		logger->enable(ihuconfig.getLogFile());
	}
	catch (Error e)
	{
		warning(e.getText());
	}

	autocrypt = ihuconfig.getCrypt();
	showkey = ihuconfig.getShowKey();
}

void IhuNoGui::waitForCalls()
{
	try
	{
		phone->waitCalls(ihuconfig.getInPort(), ihuconfig.getUDP(), ihuconfig.getTCP());
		message(logger->logStartReceiver());
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

// renyang - 打電話給某一個host
void IhuNoGui::call(QString host)
{
#ifdef REN_DEBUG
	qWarning(QString("IhuNoGui::call(QString %1)").arg(host));
#endif
	try {
		if (host.isEmpty())
			throw Error(tr("No host specified!"));
		// renyang - 取出要送出去的port號
		int callPort = ihuconfig.getOutPort();
		QString callHost = host;
		// renyang - 是否有暫時指定port號
		int tmpInd = host.findRev(':');
		if (tmpInd > 0)
		{
			// renyang - 例: "127.0.0.1:1937"
			// renyang - 要連線過去的ip address
			callHost = host.left(tmpInd);
			// renyang - 取出要連線過去的port
			callPort = host.right(host.length() - tmpInd - 1).toInt();
		}
		if (autocrypt)
		{
			// renyang - 對call id 0開啟加密???
			cryptOn(0);
		}
		// renyang - 打電話出去啦, call id一定是0, 由ihuconfig取得使用的通訊協定
		phone->call(0, callHost, callPort, ihuconfig.getProtocol());
		message(logger->logOutgoingCall(host, phone->getCallerIp(0)));
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

// renyang - 接收到一通電話, id應該指的是受話端的id
void IhuNoGui::receivedCall(int id)
{
	try
	{
		message(logger->logReceivedCall(phone->getCallerIp(id), phone->getCallerName(id)));
		if (autocrypt)
		{
			// renyang - 設定這一個call id加密
			cryptOn(id);
		}
		// renyang - 設定這一個id接受別人打過來的電話
		phone->answerCall(id);
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

// renyang - 用來記錄連線的Caller Ip與Caller Name
void IhuNoGui::connectedCall(int id)
{
	message(logger->logConnectedCall(phone->getCallerIp(id), phone->getCallerName(id)));
}

void IhuNoGui::cancelCall(int id)
{
	message(logger->logStop(phone->getCallerIp(id), phone->getCallRX(id), phone->getCallTX(id)));
}

void IhuNoGui::abortCall(int id, QString text)
{
	message(logger->logAbortedCall(phone->getCallerIp(id), text));
}

// renyang - 顯示錯誤或是debug訊息
void IhuNoGui::message(QString text)
{
	qWarning(QString("Warning: %1").arg(text));
}

void IhuNoGui::message(int id, QString text)
{
	qWarning(logger->log(text));
}

void IhuNoGui::abortAll(QString text)
{
	phone->abortAll();
	qWarning("Error: " + text);
	throw Error(text);
}

void IhuNoGui::playFile(QString name)
{
	if (!name.isEmpty())
	{
		try
		{
			warning("Playing " + name + "...");
			fileplayer->playFile(name);
		}
		catch (Error e)
		{
			abortAll(e.getText());
		}
	}
}

void IhuNoGui::stopFile()
{
	message(tr("End of file"));
	// renyang - 把所有還在buffer的資料存入檔案中
	fileplayer->end();
	// renyang - 結束目前的process
	exit(0);
}

void IhuNoGui::agcRefresh(bool on)
{
	try
	{
		float step = (float) (ihuconfig.getAGCStep()/100.f);
		float level = (float) (ihuconfig.getAGCLevel());
		phone->setupAgc(on, ihuconfig.getAGCHw(), ihuconfig.getAGCHw(), step, level, ihuconfig.getAGCControl());
	}
	catch (Error e)
	{
		warning(e.getText());
	}
}

void IhuNoGui::adrRefresh(bool on)
{
	float mindelay = (float) (ihuconfig.getADRMinDelay()/1000.f);
	float maxdelay = (float) (ihuconfig.getADRMinDelay()/1000.f);
	float stretch = (float) ihuconfig.getADRStretch();
	phone->setupAdr(on, mindelay, maxdelay, stretch);
}

void IhuNoGui::cryptOn()
{
	cryptOn(0);
}

void IhuNoGui::cryptOn(int id)
{
	try
	{
		int len = ihuconfig.getKeyLen()/8;
		phone->enableRandomCrypt(id, len);
		warning("Encryption enabled.");
	}
	catch (Error e)
	{
		abortAll(e.getText());
	}
}

void IhuNoGui::disableIn()
{
	phone->disableRecorder(true);
}

void IhuNoGui::disableOut()
{
	phone->disablePlayer(true);
}

// renyang - 表示收到的封包是編碼過的, 但是, key不合, 沒有辦法解碼
void IhuNoGui::keyRequest()
{
	qWarning("Stream is encrypted, but the decryption key is not available");
	fileplayer->noDecrypt();
}

void IhuNoGui::receivedNewKey(int callId, QString text)
{
	if (ihuconfig.getShowKey())
	{
		QString keyMsg = QString("New decryption key: %1").arg(text);
		message(logger->log(QString("%1 (%2) - %3").arg(phone->getCallerIp(callId)).arg(phone->getCallerName(callId)).arg(keyMsg)));
	}
}

void IhuNoGui::changeKey(int id)
{
	int len = ihuconfig.getKeyLen()/8;
	phone->enableRandomCrypt(id, len);
	message("Encryption key changed successfully.");
}

void IhuNoGui::quit()
{
	phone->endAll();
	phone->stopWaiting();
}

// renyang - 取得聲音的最高點
void IhuNoGui::statistics()
{
	phone->getPeak();
}
