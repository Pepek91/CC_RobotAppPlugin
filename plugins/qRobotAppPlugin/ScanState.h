#pragma once
#include "utils.h"

class ScanState
{
public:

	ScanState()
		: m_bWaitingForScan(false)
		, m_bXYZReceived(false)
		, m_bRGBReceived(false)
		, m_bNormReceived(false)
	{
	}

	~ScanState()
	{
	}

	void setWaitingForScan(bool b){ m_bWaitingForScan = b; }
	void setXYZReceived(bool b){ m_bXYZReceived = b; }
	void setRBGReceived(bool b){ m_bRGBReceived = b; }
	void setNormReceived(bool b){ m_bNormReceived = b; }
	void setDataSize(unsigned int i){ m_iDataSize = i; }

	bool isWaitingForScan(void){ return m_bWaitingForScan; }
	bool isXYZReceived(void){ return  m_bXYZReceived; }
	bool isRBGReceived(void){ return m_bRGBReceived; }
	bool isNormReceived(void){ return m_bNormReceived;}
	unsigned int getDataSize(void){ return m_iDataSize; }

private:
	bool m_bWaitingForScan;
	bool m_bXYZReceived;
	bool m_bRGBReceived;
	bool m_bNormReceived;

	unsigned int m_iDataSize;
};

