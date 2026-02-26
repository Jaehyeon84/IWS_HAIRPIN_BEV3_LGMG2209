#include "pch.h"
#include "Algorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAlgorithm g_Algorithm;

CAlgorithm::CAlgorithm()
{
	AllocateBlobBuffer();
}

CAlgorithm::~CAlgorithm()
{
	DestroyBlobBuffer();
}

BOOL CAlgorithm::AllocateBlobBuffer()
{
	m_bufferHandle = nullptr;

	if(HUBIP::AllocateBlobBuffer(m_bufferHandle) != retStatus::success)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAlgorithm::DestroyBlobBuffer()
{
	// Blob Buffer ÇØÁ¦
	if(m_bufferHandle != nullptr)
	{
		if(HUBIP::DestroyBlobBuffer(m_bufferHandle) != retStatus::success)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CAlgorithm::Blob(BYTE* src, const long imageWidth, const long imageHeight, const CRect roi, const BOOL b4Neighbor, const long minBlobSize, const long maxBlobCount, HUBSDK::BlobInfo* blobInfo, long &blobCount, long* labelImage)
{
	if(HUBIP::DetectBlob(src, imageWidth, imageHeight, roi, b4Neighbor, minBlobSize, maxBlobCount, m_bufferHandle, blobInfo, blobCount, labelImage) != retStatus::success)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAlgorithm::BlobMerge(const long imageWidth, const long imageHeight, long* label, const long maxBlobCount, const float mergeDistance, HUBSDK::BlobInfo* blobInfo, long &nBlobMergeCount)
{
	if(HUBIP::BlobLink(imageWidth, imageHeight, label, maxBlobCount, mergeDistance, m_bufferHandle, blobInfo) != retStatus::success)
	{
		return FALSE;
	}

	if(HUBIP::BlobMerge(imageWidth, imageHeight, maxBlobCount, m_bufferHandle, label, blobInfo, nBlobMergeCount) != retStatus::success)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CAlgorithm::GetGrayToColorImage(BYTE* src, const long imageWidth, const long imageHeight, const long bpp, const CRect roi, CImage* dst)
{
	BYTE* pbyColorImage = new BYTE [imageWidth * imageHeight * 3];
	memset(pbyColorImage, 0, imageWidth * imageHeight * 3);

	// Gray -> Color
	if(HUBIP::CombineRGB(src, src, src, imageWidth, imageHeight, CRect(0, 0, imageWidth, imageHeight), pbyColorImage) != retStatus::success)
	{
		return FALSE;
	}

	if(!HUBSDK::ImageInterface::ByteArrayToCImage(pbyColorImage, imageWidth, imageHeight, 3, dst))
	{
		return FALSE;
	}

	delete[] pbyColorImage;

	return TRUE;
}

BOOL CAlgorithm::DoubleThreshold(const BYTE* src, const long imageWidth, const long imageHeight, const CRect roi, const BYTE thresholdLow, const BYTE thresholdHigh, const BOOL reverse, BYTE* dst)
{
	for(long y = roi.top; y < roi.bottom; y++)
	{
		long sum = y * imageWidth;
		for(long x = roi.left; x < roi.right; x++)
		{
			long pos = sum + x;
			auto srcValue = src[pos];
			auto &dstValue = dst[pos];

			if(reverse == TRUE)
			{
				if(srcValue >= thresholdLow && srcValue <= thresholdHigh)
				{
					dstValue = 0;
				}
				else
				{
					dstValue = 255;
				}
			}
			else
			{
				if(srcValue < thresholdLow || srcValue > thresholdHigh)
				{
					dstValue = 0;
				}
				else
				{
					dstValue = 255;
				}
			}
		}
	}

	return TRUE;
}

void CAlgorithm::ResizeVector(std::vector<std::vector<CPoint>> &tVector, long resizeCount1, long resizeCount2, CPoint value)
{
	tVector.clear();
	tVector.resize(resizeCount1);
	for(auto &element1 : tVector)
	{
		element1.resize(resizeCount2);
		for(auto &element2 : element1)
		{
			element2 = value;
		}
	}
}

void CAlgorithm::ResizeVector(std::vector<std::vector<short>> &tVector, long resizeCount1, long resizeCount2, short value)
{
	tVector.clear();
	tVector.resize(resizeCount1);
	for(auto &element1 : tVector)
	{
		element1.resize(resizeCount2);
		for(auto &element2 : element1)
		{
			element2 = value;
		}
	}
}

void CAlgorithm::ResizeVector(std::vector<std::vector<float>> &tVector, long resizeCount1, long resizeCount2, float value)
{
	tVector.clear();
	tVector.resize(resizeCount1);
	for(auto &element1 : tVector)
	{
		element1.resize(resizeCount2);
		for(auto &element2 : element1)
		{
			element2 = value;
		}
	}
}

void CAlgorithm::ResizeVector(std::vector<std::vector<BOOL>> &tVector, long resizeCount1, long resizeCount2, BOOL value)
{
	tVector.clear();
	tVector.resize(resizeCount1);
	for(auto &element1 : tVector)
	{
		element1.resize(resizeCount2);
		for(auto &element2 : element1)
		{
			element2 = value;
		}
	}
}

void CAlgorithm::ResizeVector(std::vector<std::vector<std::vector<HUBSDK::BlobInfo>>> &tVector, long resizeCount1, long resizeCount2, long resizeCount3, HUBSDK::BlobInfo value)
{
	tVector.clear();
	tVector.resize(resizeCount1);
	for(auto &element1 : tVector)
	{
		element1.resize(resizeCount2);
		for(auto &element2 : element1)
		{
			element2.resize(resizeCount3);
			for(auto &element3 : element2)
			{
				element3 = value;
			}
		}
	}
}

void CAlgorithm::ResizeVector(std::vector<std::vector<HUBSDK::BlobInfo>> &tVector, long resizeCount1, long resizeCount2, HUBSDK::BlobInfo value)
{
	tVector.clear();
	tVector.resize(resizeCount1);
	for(auto &element1 : tVector)
	{
		element1.resize(resizeCount2);
		for(auto &element2 : element1)
		{
			element2 = value;
		}
	}
}

void CAlgorithm::ResizeVector(std::vector<HUBSDK::BlobInfo> &tVector, long resizeCount, HUBSDK::BlobInfo value)
{
	tVector.clear();
	tVector.resize(resizeCount);
	for(auto &element : tVector)
	{
		element = value;
	}
}

void CAlgorithm::ResizeVector(std::vector<std::vector<SResult>> &tVector, long resizeCount1, long resizeCount2, SResult value)
{
	tVector.clear();
	tVector.resize(resizeCount1);
	for(auto &element1 : tVector)
	{
		element1.resize(resizeCount2);
		for(auto &element2 : element1)
		{
			element2 = value;
		}
	}
}

void CAlgorithm::ResizeVector(std::vector<std::vector<CRect>> &tVector, long resizeCount1, long resizeCount2, CRect value)
{
	tVector.clear();
	tVector.resize(resizeCount1);
	for(auto &element1 : tVector)
	{
		element1.resize(resizeCount2);
		for(auto &element2 : element1)
		{
			element2 = value;
		}
	}
}

void CAlgorithm::ResizeVector(std::vector<std::vector<CString>> &tVector, long resizeCount1, long resizeCount2, CString value)
{
	tVector.clear();
	tVector.resize(resizeCount1);
	for(auto &element1 : tVector)
	{
		element1.resize(resizeCount2);
		for(auto &element2 : element1)
		{
			element2 = value;
		}
	}
}

void CAlgorithm::ResizeVector(std::vector<std::vector<long>> &tVector, long resizeCount1, long resizeCount2, long value)
{
	tVector.clear();
	tVector.resize(resizeCount1);
	for (auto &element1 : tVector)
	{
		element1.resize(resizeCount2);
		for (auto &element2 : element1)
		{
			element2 = value;
		}
	}
}