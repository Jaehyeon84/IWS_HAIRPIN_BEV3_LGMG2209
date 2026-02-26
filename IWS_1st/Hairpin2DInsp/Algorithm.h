#pragma once

class CAlgorithm
{
public:
	CAlgorithm();
	~CAlgorithm();

	// Blob 알고리즘
	BOOL AllocateBlobBuffer();
	BOOL DestroyBlobBuffer();
	BOOL Blob(BYTE* src, const long imageWidth, const long imageHeight, const CRect roi, const BOOL b4Neighbor, const long minBlobSize, const long maxBlobCount, HUBSDK::BlobInfo* blobInfo, long &blobCount, long* labelImage);
	BOOL BlobMerge(const long imageWidth, const long imageHeight, long* label, const long maxBlobCount, const float mergeDistance, HUBSDK::BlobInfo* blobInfo, long &nBlobMergeCount);
	BOOL GetGrayToColorImage(BYTE* src, const long imageWidth, const long imageHeight, const long bpp, const CRect roi, CImage* dst);
	BOOL DoubleThreshold(const BYTE* src, const long imageWidth, const long imageHeight, const CRect roi, const BYTE thresholdLow, const BYTE thresholdHigh, const BOOL reverse, BYTE* dst);

	void ResizeVector(std::vector<std::vector<CPoint>> &tVector, long resizeCount1, long resizeCount2, CPoint value = CPoint(0, 0));
	void ResizeVector(std::vector<std::vector<short>> &tVector, long resizeCount1, long resizeCount2, short value = 0);
	void ResizeVector(std::vector<std::vector<float>> &tVector, long resizeCount1, long resizeCount2, float value = 0.f);
	void ResizeVector(std::vector<std::vector<BOOL>> &tVector, long resizeCount1, long resizeCount2, BOOL value = FALSE);
	void ResizeVector(std::vector<std::vector<std::vector<HUBSDK::BlobInfo>>> &tVector, long resizeCount1, long resizeCount2, long resizeCount3, HUBSDK::BlobInfo value);
	void ResizeVector(std::vector<std::vector<HUBSDK::BlobInfo>> &tVector, long resizeCount1, long resizeCount2, HUBSDK::BlobInfo value);
	void ResizeVector(std::vector<HUBSDK::BlobInfo> &tVector, long resizeCount, HUBSDK::BlobInfo value);
	void ResizeVector(std::vector<std::vector<SResult>> &tVector, long resizeCount1, long resizeCount2, SResult value);
	void ResizeVector(std::vector<std::vector<CRect>> &tVector, long resizeCount1, long resizeCount2, CRect value = CRect(0, 0, 0, 0));
	void ResizeVector(std::vector<std::vector<CString>> &tVector, long resizeCount1, long resizeCount2, CString value = _T(""));
	void ResizeVector(std::vector<std::vector<long>> &tVector, long resizeCount1, long resizeCount2, long value = 0);

	void *m_bufferHandle;
};

extern CAlgorithm g_Algorithm;
