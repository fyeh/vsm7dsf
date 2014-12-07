/*
 * IBM Confidential
 *
 * OCO Source Material
 *
 * 5725H94
 *
 * (C) Copyright IBM Corp. 2005,2006
 *
 * The source code for this program is not published or otherwise divested
 * of its trade secrets, irrespective of what has been deposited with the
 * U. S. Copyright Office.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 */

#include "stdafx.h"
#include "PushSource.h"
#include "PushPin.h"
#include "PushGuids.h"
#include "TransportUrl.h"
#include "Settings.h"
#include "live.h"
#include "trace.h"
#include <Dvdmedia.h>

#pragma managed
/**
 * This is the output pin construction for our source filter
 */
CPushPinCisco::CPushPinCisco(HRESULT *phr, CSource *pFilter)
        : CSourceStream(NAME("Push Source Cisco"), phr, pFilter, L"Out")
{
	TRACE_INFO(  "Pin constructor");
	//create and instance of the view object
	m_rtStart=0;
	m_bufferCount=0;
	m_lostFrameBufferCount=0;
	
	m_retryConnectionTime=0;
	m_reloadLostVideoTime=0;
	m_videoWidth=m_videoHeight=m_bitCount=0;
	m_reloadFrameBufferTime=0;
	m_currentVideoState=NoVideo;
	m_framerate=12;
	m_streamMedia=NULL;
}

/**
* Destructor, 
*/
CPushPinCisco::~CPushPinCisco()
{   
	//stop the pipe/viewer first
	Stop();

	TRACE_INFO( "Shutting down stream");
	if(m_streamMedia!=NULL)
	{
		m_streamMedia->rtspClientCloseStream();
		delete m_streamMedia;
	}
	m_streamMedia=NULL;
	TRACE_DEBUG(  "Pin destructor");
}

/**
* Returns the media type that we will be rendering 
* back to the base class.  This is where we actually
* load the video pipeline, we do it up front so that
* we have the correct image size for the buffer
*/
HRESULT CPushPinCisco::GetMediaType(CMediaType *pMediaType)
{
	TRACE_DEBUG(  "GetMediaType");
	TransportUrl *url=((CPushSourceCisco*)this->m_pFilter)->m_transportUrl;
	QueryVideo(url);

	CAutoLock cAutoLockShared(&m_cSharedState);
	VIDEOINFO *pvi = (VIDEOINFO *)pMediaType->AllocFormatBuffer(sizeof(VIDEOINFO));

	/* Would this ever really happen? */
    if (NULL == pvi) return S_OK;

	/* Zero out the memory */
	ZeroMemory(pvi, sizeof(VIDEOINFO));

	/* Plugin in our FOURCC */
	TRACE_INFO( "  biCompression = BI_RGB");
	pvi->bmiHeader.biCompression = BI_RGB;

	/* 24 bits per pixel */
    pvi->bmiHeader.biBitCount = 24;

	/* Header size */
    pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	/* Video width */
    pvi->bmiHeader.biWidth =  m_videoWidth;

	/* Video height */
    pvi->bmiHeader.biHeight =  -m_videoHeight;

	/* Video Planes */
    pvi->bmiHeader.biPlanes = 1;

	/* The max size a video frame will be */
    pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
	
	/* ?? */
    pvi->bmiHeader.biClrImportant = 0;

	/* This stuff is pretty much ignored in this situation */
	pvi->AvgTimePerFrame = UNITS/30;
	
	/* Set the Major-type of the media for DShow */
    pMediaType->SetType(&MEDIATYPE_Video);

	/* Set the structure type so DShow knows 
	 * how to read the structure */
    pMediaType->SetFormatType(&FORMAT_VideoInfo);

	/* If our codec uses temporal compression */
    pMediaType->SetTemporalCompression(FALSE);
		
	GUID subTypeGUID;
	subTypeGUID = MEDIASUBTYPE_RGB24;
	pMediaType->SetSubtype(&subTypeGUID);

	/* Set the max sample size */
    TRACE_DEBUG(  " biSizeImage=%d", pvi->bmiHeader.biSizeImage);
	 pMediaType->SetSampleSize(pvi->bmiHeader.biSizeImage);
	
	/* Get our video header to configure */
	VIDEOINFOHEADER *vihOut = (VIDEOINFOHEADER *)pMediaType->Format();
	
	/* 30 Fps - Not important */
	vihOut->AvgTimePerFrame = UNITS / 30;
	return S_OK;
}

/**
Setup our video envirnoment.
Using the transport URL setup the environment
this most likely means connecting to a VSM and getting
a security token for the camera.  Note this can take some time
to complete, we made to revist.  It could be possible to get
the token in a background thread as long as we have the width
and height in the url
*/
int CPushPinCisco::QueryVideo(TransportUrl * url)
{
	int ret=-1;

	TRACE_INFO("Query video");
	if(!(m_currentVideoState==VideoState::NoVideo||m_currentVideoState==VideoState::Lost)) 
	{
		TRACE_INFO("Video already configured, exiting");
		return 0;
	}

	if(!url->hasUrl())
	{
		TRACE_INFO("Missing the URL, query the VSM");
		ret=QueryVsom(url);
		if(ret<0)
		{
			TRACE_ERROR( "VSOM Query failed ret=%d", ret);
			return ret;
		}
	}

	try
	{
		TRACE_INFO("Try to open the RTSP video stream");
		if(m_streamMedia==NULL)
			m_streamMedia=new CstreamMedia();

		ret =  m_streamMedia->rtspClientOpenStream((const char *)url->get_RtspUrl());
		if (ret < 0)
		{
			TRACE_ERROR( "Unable to open rtsp video stream ret=%d", ret);
			return E_FAIL;	
		}
	}
	catch(...)
	{
		TRACE_CRITICAL( "QueryVideo Failed from VSOM");
		m_currentVideoState=VideoState::NoVideo;
		throw false;
	}

	//attempt to get the media info from the stream
	//we know that in 7.2 this does not work, but we are
	//hoping that 7.5 will enable width and height
	MediaInfo videoMediaInfo;
	try{
		TRACE_INFO("Get Media Info");
		ret= m_streamMedia->rtspClinetGetMediaInfo(CODEC_TYPE_VIDEO, videoMediaInfo);
		if(ret < 0)
		{	
			TRACE_CRITICAL( "Unable to get media info from RTSP stream.  ret=%d (url=%s)", ret,url->get_Url());
			return VFW_S_NO_MORE_ITEMS;
		}
	}
	catch(...)
	{
		TRACE_CRITICAL( "QueryVideo Failed from VSOM");
		m_currentVideoState=VideoState::NoVideo;
		throw false;
	}

	TRACE_INFO( "Format: %d",videoMediaInfo.i_format);
	TRACE_INFO( "Codec: %s",videoMediaInfo.codecName);
	if(videoMediaInfo.video.width>0)
	{
		TRACE_INFO( "Using video information directly from the stream");
		m_videoWidth = videoMediaInfo.video.width;
		m_videoHeight = videoMediaInfo.video.height;
		m_bitCount = videoMediaInfo.video.bitrate;
		if(videoMediaInfo.video.fps>0)
			m_framerate=(REFERENCE_TIME)(10000000/videoMediaInfo.video.fps);
	}else{
		TRACE_WARN( "No video info in stream, using defaults from url");
		m_videoWidth = url->get_Width();
		m_videoHeight = url->get_Height();
		m_bitCount = 1;
		if(url->get_Framerate()>0)
			m_framerate=(REFERENCE_TIME)(10000000/url->get_Framerate());
	}

	TRACE_INFO( "Width: %d",m_videoWidth);
	TRACE_INFO( "Height: %d",m_videoHeight);
	TRACE_INFO( "FPS: %d",10000000/m_framerate);
	TRACE_INFO( "Bitrate: %d",m_bitCount);
	m_currentVideoState=VideoState::Reloading;
		
	return ret;
}

/**
Connect to a VSM and query for the camera specified in the url
once the camera is located attempt to get a security token for that
camera.
*/
int CPushPinCisco::QueryVsom(TransportUrl * url)
{
	int ret=-1;
	System::String ^domain=gcnew System::String(url->get_Domain());
	System::String ^server=gcnew System::String(url->get_Server());
	System::String ^user=gcnew System::String(url->get_UserName());
	System::String ^pwd=gcnew System::String(url->get_Password());
	System::String ^camera=gcnew System::String(url->get_Camera());

	try
	{
		HelperLib::TraceHelper::WriteInfo( "Connecting VSOM server {0} with user name {1}",server,user);
		Cisco::PSBU::VSOMWebService::VsomServices ^ vsomService = gcnew Cisco::PSBU::VSOMWebService::VsomServices(server, domain, user, pwd, true, true);
		Cisco::PSBU::VSOMWebService::cameraFilter ^ dF = gcnew Cisco::PSBU::VSOMWebService::cameraFilter;
		Cisco::PSBU::VSOMWebService::pageInfo ^ pI = gcnew Cisco::PSBU::VSOMWebService::pageInfo();
			
		dF->byUid=gcnew array<System::String^> {camera};
		dF->pageInfo = pI;
		dF->pageInfo->limit = 50; // we should never have more than one!

		Cisco::PSBU::VSOMWebService::page ^cameraRefList = vsomService->CameraService->getCameras(dF);
		if ((cameraRefList->totalRows > 0))
		{
				Cisco::PSBU::VSOMWebService::device ^camera =(Cisco::PSBU::VSOMWebService::device^)cameraRefList->items[0];
				                
			HelperLib::TraceHelper::WriteDebug(System::String::Format("Camera: {0}, States: {1}/{2}/{3}", 
				camera->alternateId, 
				camera->adminState, 
				camera->operState, 
				camera->model));

			int tokenExpirationTime = 300;
			// 24 hours --- ONLY for DEBUG, real application suppose to set to ~5 min,

			camera->managedByRef->refObjectTypeSpecified = true;
			Cisco::PSBU::VSOMWebService::baseObjectRef^cameraRef = Cisco::PSBU::VSOMWebService::baseObject2Ref::bO2Ref(camera);

			HelperLib::TraceHelper::WriteDebug("Getting security token for {0}", camera->name);
			System::String ^ token = vsomService->CameraService->getSecurityToken(cameraRef, tokenExpirationTime);

			HelperLib::TraceHelper::WriteDebug("Getting host ip for {0}", camera->name);
			Cisco::PSBU::VSOMWebService::device ^ ums = vsomService->UmsService->getUms(camera->managedByRef);
			System::String ^ host = ums->deviceAccess->hostname_ip;

			msclr::interop::marshal_context oMarshalContext;
			token=System::Web::HttpUtility::UrlEncode(token);
            
			HelperLib::TraceHelper::WriteInfo("We got a token {0}",token);
			url->set_Token(oMarshalContext.marshal_as<const char*>(ums->deviceAccess->hostname_ip), oMarshalContext.marshal_as<const char*>(token));
			ret=0;

			HelperLib::TraceHelper::WriteInfo("Logout from VSOM");
			vsomService->AuthService->logout();


		}else{
			TRACE_ERROR("Camera %s not found!", url->get_Camera());
		}
	}
	catch(...)
	{
		TRACE_CRITICAL( "QueryVideo Failed from VSOM");
		m_currentVideoState=VideoState::NoVideo;
	}
	return ret;
}

/**
* DecideBufferSize
*
* This will always be called after the format has been sucessfully
* negotiated. So we have a look at m_mt to see what size image we agreed.
* Then we can ask for buffers of the correct size to contain them.
*/
HRESULT CPushPinCisco::DecideBufferSize(IMemAllocator* pMemAlloc, ALLOCATOR_PROPERTIES* pProperties)
{
	/* Thread-saftey */
    TRACE_DEBUG(  "DecideBufferSize");
	CAutoLock cAutoLockShared(&m_cSharedState);

	HRESULT hr = S_OK;
	VIDEOINFO *pvi = (VIDEOINFO *)m_mt.Format();
	pProperties->cBuffers = 1;
	pProperties->cbBuffer =2* pvi->bmiHeader.biSizeImage;		
	
//	pProperties->cBuffers = 20;
//	pProperties->cbBuffer = 65535;
	
	/* Ask the allocator to reserve us some sample memory, NOTE the function
	 * can succeed (that is return NOERROR) but still not have allocated the
	 * memory that we requested, so we must check we got whatever we wanted */
	ALLOCATOR_PROPERTIES Actual;

	hr = pMemAlloc->SetProperties(pProperties, &Actual);
	if (FAILED(hr)) 
	{
		TRACE_ERROR( "DecideBufferSize pMemAlloc->SetProperties.  HRESULT = %#x, SIZE=%d\n", hr, pProperties->cbBuffer);
		return hr;
	}

	/* Is this allocator unsuitable? */ 
	if (Actual.cbBuffer < pProperties->cbBuffer) 
	{
		TRACE_ERROR( "DecideBufferSize to small %d < %d\n",Actual.cbBuffer, pProperties->cbBuffer);
		return E_FAIL;
	}

	TRACE_INFO( "DecideBufferSize SIZE=%d\n",  pProperties->cbBuffer);

	TransportUrl *url=((CPushSourceCisco*)this->m_pFilter)->m_transportUrl;
	if(m_currentVideoState!=Playing)
		m_streamMedia->rtspClientPlayStream(url->get_RtspUrl());
	m_currentVideoState=Playing;
	return S_OK;
}



/**
* This is where we insert the DIB bits into the video stream.
* FillBuffer is called once for every sample in the stream.
* We then pass the buffer to the ProcessVideo to do the actual
* byte copy
*
* If the source checked out ok but fill buffer never gets 
* called then the problem is probably a bad media type.
* FillBuffer is called by the directshow thread.
*/
HRESULT CPushPinCisco::FillBuffer(IMediaSample *pSample)
{
	bool syncPoint;
	HRESULT hr = S_OK;
    REFERENCE_TIME rtStop = 0, rtDuration = 0;
	bool rc=true;

	CheckPointer(pSample, E_POINTER);
	//CAutoLock cAutoLockShared(&CPushPinCisco);

	try
	{
		// Copy the DIB bits over into our filter's output buffer.
		//This is where the magic happens, call the pipeline to fill our buffer
		rc = ProcessVideo(pSample);

		if (rc)
		{
			// Set the timestamps that will govern playback frame rate.
			REFERENCE_TIME rtStop  = m_rtStart + m_framerate;
			pSample->SetTime(&m_rtStart, &rtStop);
			pSample->SetDiscontinuity(FALSE);
			m_rtStart=rtStop;
			pSample->SetSyncPoint(TRUE);
		}else{
			hr=E_FAIL;
		}
	}
	catch(...)
	{
		TRACE_ERROR(  "--Exception---------------------");
		TRACE_ERROR(  "FillBuffer...");
		TRACE_ERROR(  "--------------------------------");
		
		hr=E_FAIL;//This will cause the filter to stop
	}

	return hr;
}

/**
Send our video buffer the live media provider who will copy
the next frame from the queue
*/
bool CPushPinCisco::ProcessVideo(IMediaSample *pSample)
{
	bool rc=true;
	long cbData;
	BYTE *pData;


	// Access the sample's data buffer
	pSample->GetPointer(&pData);
	cbData = pSample->GetSize();
	long bufferSize=cbData;

	rc=m_streamMedia->GetFrame(pData, bufferSize);

	if(rc)
	{
		m_lostFrameBufferCount=0;
		m_currentVideoState=Playing;
	}else{
		//paint black video to indicate a lose
		int count=((CPushSourceCisco*)this->m_pFilter)->m_transportUrl->get_LostFrameCount();
		if(m_lostFrameBufferCount>count)
		{
			if(!(m_currentVideoState==VideoState::Lost))
			{
				TRACE_INFO("Lost frame count (%d) over limit {%d). Paint Black Frame",m_lostFrameBufferCount, count );
				//HelperLib::TraceHelper::WriteInfo("Lost frame count over limit. Paint Black Frame and shutdown.");
				memset(pData,0, bufferSize);
				m_currentVideoState=VideoState::Lost;
				rc=true;
			} else{
				TRACE_INFO("Shutting Down");
				rc=false;
			}
		
			/*
			// The following section is  meant to try to recover video. It works except that there are unhanlded exceptions thrown in the Live555 code that we cannot fix so we will let the SSE restart the engine instead.
			//
			//TransportUrl *url=((CPushSourceCisco*)this->m_pFilter)->m_transportUrl;
			if(!(m_currentVideoState==VideoState::Lost))
			{
				TRACE_WARN("Lost frame count (%d) over limit {%d). Restarting video",m_lostFrameBufferCount, count );
				memset(pData,0, bufferSize);
				m_lostFrameBufferCount=0;
				rc = ReloadVideo(url);
				if(!rc)
				{	
					m_currentVideoState=VideoState::Lost;
				}
				rc=true;
			} else{
				rc = false;
				return rc;
				TRACE_WARN("Lost frame count (%d) over limit {%d) twice. Restarting Session",m_lostFrameBufferCount, count );
				url->clear_Token();
				QueryVsom(url);
				rc = ReloadVideo(url);
				if(!(rc))
				{
					
				}
			}
			rc=true;
			//(CPushSourceCisco*)this->Stop();
			*/
		}else{
			m_lostFrameBufferCount++;
			rc = true;
			//if(m_currentVideoState==VideoState::Lost) Sleep(1000);

		}
	}
	return rc;
}


/**
Helper function
Attempt to reload lost video.  We may lose video for a number of reasons
such as a network outage or power failure.  This method will attempt to
reconnect to the video stream.  It must reconnect to the same stream as
the orginal connection otherwise the video buffer will not be the correct
size.
*/
bool CPushPinCisco::ReloadVideo(TransportUrl * url)
{
	bool rc=true;
	int ret = 0;
	try
	{
		TRACE_DEBUG( "Reload video");
		Stop();
		m_lostFrameBufferCount=0;
		m_bufferCount=0;
		//TransportUrl *url=((CPushSourceCisco*)this->m_pFilter)->m_transportUrl;
		if(m_streamMedia!=NULL) delete m_streamMedia;
		m_streamMedia=NULL;
		m_streamMedia=new CstreamMedia();
		ret =  m_streamMedia->rtspClientOpenStream((const char *)url->get_RtspUrl());
		if(ret!=0)
		{
			TRACE_ERROR("Unable to open stream ret=%d", ret);
			rc=false;
		}else{
			ret = m_streamMedia->rtspClientPlayStream(url->get_RtspUrl());
			if(ret!=0)
			{
				TRACE_ERROR("Unable to open stream ret=%d", ret);
				rc=false;
			}
		}

		/*if we have not received a frame then switch to unicast for the next try
		if(!m_firstFrame)
		{
			TRACE_DEBUG( "Reload using unicast connection");
		}
		*/
	}
	catch(...)
	{
		TRACE_ERROR( "Reload Video unexpected error");
		rc=false;
	}

	return rc;
}

/**
* Any special work that needs to happen when the playback thread
* is created.
*/
HRESULT CPushPinCisco::OnThreadCreate(void)
{
	HRESULT hr=S_OK;
	hr=CSourceStream::OnThreadCreate();
	TransportUrl *url=((CPushSourceCisco*)this->m_pFilter)->m_transportUrl;

	TRACE_INFO("Open Stream");
	if(m_streamMedia==NULL)
		m_streamMedia=new CstreamMedia();
	int ret =  m_streamMedia->rtspClientOpenStream(url->get_RtspUrl());
	if(ret!=0)
	{
		TRACE_ERROR("Unable to open stream ret=%d", ret);
		hr=E_FAIL;
	}

	TRACE_ERROR( "OnThreadCreate.  HRESULT =  %#x", hr);
	return hr;
}
/**
* Cleanup once the playback thread is killed
*/
HRESULT CPushPinCisco::OnThreadDestroy(void)
{
	CAutoLock cAutoLockShared(&m_cSharedState);
	HRESULT hr=S_OK;
//	CSourceStream::Stop();
	hr=CSourceStream::OnThreadDestroy();
	int ret =  m_streamMedia->rtspClientCloseStream();

	if(m_streamMedia!=NULL)
		delete m_streamMedia;
	m_streamMedia=NULL;

	TRACE_INFO( "OnThreadDestroy.  HRESULT = %#x", hr);
	return hr;
}

/**
* FillBuffer is about to get called for the first time
* do any prep work here
*/
HRESULT CPushPinCisco::OnThreadStartPlay(void)
{
	HRESULT hr=S_OK;
	hr=CSourceStream::OnThreadStartPlay();
	TRACE_INFO("Play Stream");
	TransportUrl *url=((CPushSourceCisco*)this->m_pFilter)->m_transportUrl;
	int ret =  m_streamMedia->rtspClientPlayStream(url->get_RtspUrl());
	if(ret!=0)
	{
		TRACE_ERROR("Unable to play stream ret=%d", ret);
		hr=E_FAIL;
	}

	TRACE_DEBUG( "OnThreadStartPlay.  HRESULT =  %#x", hr);
	return hr;
}
/**
* Stopping FillBuffer and playback thread
*/
HRESULT CPushPinCisco::Stop(void)
{
	CAutoLock cAutoLockShared(&m_cSharedState);
	HRESULT hr=S_OK;
	TRACE_DEBUG( "Stopping playback");
	if(m_streamMedia!=NULL)
	{
		int ret =  m_streamMedia->rtspClientCloseStream();
		if (ret < 0)
		{
			TRACE_ERROR( "Unable to close rtsp video stream ret=%d", ret);
			return E_FAIL;	
		}
	}
	TRACE_INFO( "Stop.  HRESULT = %#x", hr);
	return hr; 
}
