#pragma once

class CMemoryManager
{
public:
	CMemoryManager();
	~CMemoryManager();

	void Clear();
	void SetImageSize(const long originalImageX, const long originalImageY, const long unrolledImageX, const long unrolledImageY);
	void ResetMemoryUse();
	void SetReserveByteOriginalImage(long nNum);
	void SetReserveByteImage(long nNum);
	void SetReserveLongImage(long nNum);
	void SetReserveFloatImage(long nNum);
	void SetReservePosBuffer();
	BYTE* GetNextByteImage();
	long* GetNextLongImage();
	float* GetNextFloatImage();
	BYTE* GetByteImage(long nOrder = 0);
	long* GetLongImage(long nOrder = 0);
	float* GetFloatImage(long nOrder = 0);
	CPoint* GetOriginalPos();
	CPoint* GetUnrolledPos();

public:
	long m_nOriginalImageWidth;
	long m_nOriginalImageHeight;
	long m_nUnrolledImageWidth;
	long m_nUnrolledImageHeight;
	long m_nUseByteImage;
	long m_nUseLongImage;
	long m_nUseFloatImage;
	std::vector<BYTE*> m_vecByteImage;
	std::vector<long*> m_vecLongImage;
	std::vector<float*> m_vecFloatImage;
	CPoint* m_pptOriginalPos;
	CPoint* m_pptUnrolledPos;
	long m_nPosSizeX;
	long m_nPosSizeY;
};

extern CMemoryManager g_MemoryManager;