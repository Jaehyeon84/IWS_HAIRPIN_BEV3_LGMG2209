#pragma once
#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigeCamera.h>

//using namespace Basler_GigETLParams;
using namespace Pylon;
//using namespace GenApi;

class CGrab2DBuff
{
public:
	CGrab2DBuff(const size_t ImageSize);
	~CGrab2DBuff();
	uint8_t* GetBuffPointer(void) { return m_pBuff; }
	StreamBufferHandle GetBuffHandle(void) { return m_hBuff; }
	void SetBufferHandle(StreamBufferHandle hBuffer) { m_hBuff = hBuffer; };

protected:
	uint8_t *m_pBuff;
	StreamBufferHandle m_hBuff;
};

