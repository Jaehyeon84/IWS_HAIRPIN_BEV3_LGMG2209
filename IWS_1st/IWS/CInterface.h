#pragma once
class CInterface
{
public:
	enum {
		Mode_Manual = 0,
		Mode_Auto,
		Mode_End
	};
	CInterface();
	~CInterface();
	int m_iMode;
	int m_iSeq;
};

