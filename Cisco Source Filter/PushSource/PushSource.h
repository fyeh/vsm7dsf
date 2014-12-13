

#pragma once
#include "stdafx.h"
#include "PushGuids.h"
#include "PushPin.h"

/**
* This is the actual Direct Show Filter, the output pin is defined in CPushPinCisco
* The source filter establishes the connetion between the media element, via direct show
* and our MediaBridge, via callback functions.  This is the first place to look when
* you are not seeing video.
*/
class CPushSourceCisco : public CSource, public IFileSourceFilter//, public IMediaBridgeSourceFilter
{
public:
	DECLARE_IUNKNOWN;
	static CUnknown *		WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
	STDMETHODIMP			NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	STDMETHODIMP_(ULONG)	NonDelegatingAddRef();
	STDMETHODIMP_(ULONG)	NonDelegatingRelease();
	STDMETHODIMP			Stop(void);
	STDMETHODIMP			GetCurFile(LPOLESTR * ppszFileName, AM_MEDIA_TYPE *pmt);
	STDMETHODIMP			Load(LPCOLESTR lpwszFileName, const AM_MEDIA_TYPE *pmt);
	STDMETHODIMP			InitializeVideo(int VideoPixelWidth, int VideoPixelHeight, int BitCount, DWORD FourCC, GUID MediaSubType);
	STDMETHODIMP			SendSample(char * pBuffer, int BufferLen);

	~CPushSourceCisco();

	//Used to find .Net dlls during runtime
static System::Reflection::Assembly^ MyResolveEventHandler(System::Object^ sender, System::ResolveEventArgs^ args);


private:
	CPushSourceCisco(LPUNKNOWN punk, HRESULT *phr);
	CCritSec					m_cSharedState;
	CPushPinCisco*				m_pPin;
	std::string					m_currentFile;

public:
	TransportUrl*				m_transportUrl;				
};



