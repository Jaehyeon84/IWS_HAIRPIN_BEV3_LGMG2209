#include "../stdafx.h"
#include "Grab2DBuff.h"


/* -- CLASS: CGrab2DBuffer -- */
// Constructor allocates the image buffer
CGrab2DBuff::CGrab2DBuff(const size_t ImageSize) :
	m_pBuff(NULL)
{
	m_pBuff = new uint8_t[ImageSize];
	if (NULL == m_pBuff)
	{
		GenICam::GenericException e("Not enough memory to allocate image buffer", __FILE__, __LINE__);
		throw e;
	}
}

// Freeing the memory
CGrab2DBuff::~CGrab2DBuff()
{
	if (NULL != m_pBuff)
		delete[] m_pBuff;
}
