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

/** @mainpage CiscoFilter
*
 * @subpage intro Introduction<br>
 * @subpage basic Operation<br>
 * @subpage related Required Libraries<br>
 * @subpage requirements Requirements<br>
 * @subpage transport TransportURL<br>
 * @subpage samples Samples<br>
 * @subpage example Example<br>
 * @subpage notes Notes<br>
 * @subpage settings Configuration Settings<br>
 * @subpage release Release Notes<br>
 * @subpage trouble Trouble Shooting<br>
 * @subpage tdo Things To Do<br>
 * @subpage issues Known Issues<br>
 * 
 * 
* @authors David Horth
*
* @page intro Introduction
* The CiscoFilter project is a direct show filter for the Cisco VSM 7+ video system. The main components
 * of the filter are the CiscoSource.ax, HelperLib, VSOMWebServices.
 * The uses both live555 libraries and ffmpeg libraries to receive and decode video from a VSM 7.x system.
 * The filter is configured with a single output pin with the format set as RGB24.  

 *  @page basic Operation
 The filter accepts a ciscosource url as a parameter to the push source filters Load method.  This string is
 passed to the TransportUrl class which parses out any necessary settings from the string.  Once we have a valid
 url the filter will get called when the output pin is connected.  During the connection we first query the VSM
 specified in the url to get a security token for the video stream.  Once we have the security token we then create
 a valid RTSP url to pass to the live555 library to open the stream and query the media info.  Once the connection
 to the stream is established and verified we start 'playing' the rtsp stream.  At this point we start receiving 
 H264 video from the source, these video frames are decoded in real time, using ffmpeg, and place on a queue.  When 
 the DS filter starts playing the stream (ie FillBuffer gets called) we pull the frames from the video queue and 
 write the data directly to the direct show buffer.
 * 
 * @page related Required Libraries
 * The following libraries are used in the product.
 * 
 * - Cisco API
 * Used by the filter to query the system manager and get unicast addresses, it is also used in all the samples. 
 This api consists of two dll's the first is a .Net dll, VSOMServices, which is provided as part of the Cisco
 VSM SDK.  The second is another .Net dll called HelperLib.  All HelperLib does is wrap up some of the calls to the
 VSM to make integration a bit easier.
 
 * 
 * - Windows Media Play 10+
 * Because the Cisco API uses a direct show filter to supply the video frames target systems
 * must have at least Windows Media Player version 10 or higher.
 * 
 * - DirectX June 2010 release
 * The most recent version of the DirectX is required.
 * 
 * It is expected that all target machines are running with the most up to date service packs
 * from Microsoft.
 * 
 * @section dsfilter Cisco Direct Show Filter
 * Included in the API is a direct show filter for Cisco this must be installed and registered
 * on each system. The file name for the filter is CiscoSource.ax, it is registerd with regsvr32.
 This is a standard direct show source filter whois input is a string via a load method, and the 
 output pin is a RGB24 video stream.
 * 
 * 
* <hr>
* @page requirements Requirements
 * @li  Implement all the interfaces required by DirectShow Source filter
 * @li  Stream video from Cisco VSM 7.2 servers
 * @li	Support the RTSP protocol used by the VSM to stream video
 * @li  Live view only, playback of recorded video is not required
 * @li  Receive the frames as H264 and convert them to RGB24
 * @li  Support a sample grabber filter at the end of the graph to get video frames in RGB format
 * @li  The stream shall be selectable as part of he URL (nice to have)
 * @li  The framerate shall be selectable as part of the URL (nice to have)
 * @li  The camera could be directly addressable, ie not require a VSM (nice to have)
 * 
 * 
 * @page transport TransportURL
 * The direct show filter uses a registered url prefix of Ciscosource. 
 * DirectShow uses this tag to indentify and load our source filter when
 * trying to render video. The Ciscosource url contains a number of query
 * parameters that are used to indentify the video characteristics
 * @see TransportUrl for more information of the properties of the
 * Ciscosource url.  Basically, the job of the transport URL is to decode
 * the cisco url string and convert it to a RTSP string
 *
 * 
 * @page samples Samples<br>
 * @subpage samplegrabberpage Sample Grabber<br>
 * @subpage findcamerapage Find Cameras<br>
 * 
 @page samplegrabberpage Sample Grabber<br>
 Using directshow.net, sample grabber with construction a graph
 using the source url, a sample grabber and a video renderer.
 The purpose of sample grabber is to emulate an envirnoment some
 what similar to the IBM tool.

 @page findcamerapage Find Cameras<br>
 The FindCameras application will enumerate all the cameras the specified VSM.  
Provide both the vsm ip adress or dns name along with the user name and passowrd, the 
application will query for all available camers and display the metadata for each
camera.  It will also get a thumbnail image from each camera.  You can copy individual
ciscosource urls by either pressing the copy url button or by direct copy from the 
property grid.  The RTSP url can be copies from the property grid as well.
All results cane be exported to a CSV file of later examination.
IMPORTANT - the image size returned in the ciscosource url may not be correct

 * @page example Example
 * @see samplegrabber for a working example of this code
 * @code
	This is an example of a graph that will render video using the Ciscosource
	filter and a sample grabber
        private void BuildGraph(IGraphBuilder pGraph, string srcFile1)
        {

            //reset our properties
            Filter = false;
            Sample = false;
            Render = false;
            Running = false;
            FirstFrame = string.Empty;

            //graph builder
            var pBuilder = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();
            int hr = pBuilder.SetFiltergraph(pGraph);
            CheckHr(hr, "Can't SetFiltergraph");

            //add Cisco Filter
            var pCiscoFilter2 = CreateCiscoFilter(pGraph, srcFile1);

            //add Colorspace conveter
            var pColorSpaceConverter = CreateColorSpace(pGraph);
            var pColorSpaceConverter2 = CreateColorSpace(pGraph);

            //add SampleGrabber
            var pSampleGrabber = CreateSampleGrabber(pGraph);

            //add Video Renderer
            var pVideoRenderer = CreateVideoRenderer(pGraph);
            
            //connect Cisco Filter and color space converter
            hr = pGraph.ConnectDirect(GetPin(pCiscoFilter2, "Out"), GetPin(pColorSpaceConverter, "Input"), null);
            CheckHr(hr, "Can't connect Cisco Filter and Color space converter");
            Color = true;

            //connect color space converter and sample grabber
            hr = pGraph.ConnectDirect(GetPin(pColorSpaceConverter, "XForm Out"), GetPin(pSampleGrabber, "Input"), null);
            CheckHr(hr, "Can't connect Cisco Filter and Color Space Converter and Sample Grabber");

            //?? Do we really need a second color space converter??
            hr = pGraph.ConnectDirect(GetPin(pSampleGrabber, "Output"), GetPin(pColorSpaceConverter2, "Input"), null);
            CheckHr(hr, "Can't connect Cisco Filter and Color Space Converter and Sample Grabber and Color converter 2");
            Sample = true;

            //add a renderer
            hr = pGraph.ConnectDirect(GetPin(pColorSpaceConverter2, "XForm Out"), GetPin(pVideoRenderer, "VMR Input0"), null);
            CheckHr(hr, "Can't connect Cisco Filter and Color Space Converter and Sample Grabber and Color converter and video render");
            Render = true;

            _grabber = pSampleGrabber as ISampleGrabber;
            Trace("Graph Complete");
		}
 * @endcode
 * 
 * 
 * @page notes Notes
@li Logging - The Ciscosource filter uses log4net for all loging.  Logs are stored in a subdirectory of the installation
called Logs.  Make sure you have write privelages to this directory.

@li Verobisity Levels
0:  Off 
No log file will be written

1: Critical
Only those errors that will stop the filter from operating (ie unable to find video stream)

2: Error
Errors encountered as part of the normal processing of video (ie unable to decode frame).  These errors typically do not interrupt the operation of the filter.

3:Warning
Something unexpected occurred but we will able to proceed without any major impact to the system. (ie unable to get a value from the query string, so we defaulted back to a registry value)

4: Info
Logs normal operational events in the filter, this include setup and teardown of both the filter and the rtsp stream.  This is the best level for diagnostic testing, it will write all the critical events to the log file while avoiding those which occur in loops and cause log file bloat.

5: Verbose
This is the most detailed logging and will log each frame as it is received, decoded and delivered.  Very helpful when trouble shooting specific errors, however, will cause rapid log file bloat due the number of log entries that occur in loops.  This level of logging my impact performance of the filter.

6: Debug
Logs the memory usage at various stages of the process.  This level of logging will impact performance of the filter.

@li If you can obtain a security token from an external source it is possible to supply it as part of the cisco query string, thereby avoiding the calls to the webservice.  See the transport url for more details

*   

*  <hr> 
* @page settings Configuration Settings
	static int DefaultFramerate()
	If no Framerate is specified in the query string then use
	this value as the framerate.
	Default = 12

	static int MaxFramerate()
	The upper limit of supported framerates per the query string.
	Default = 30

	static int RetryFrameCount()
	The number seconds that can be empty before the system attempts
	to reload the video pipeline.  
	Default = 120

	static int ReloadFrameCount()
	The number frames to run through while the filter stabilizes after a reload.  
	Default = 500

	static int LostFrameCount()
	The number seconds to wait before we take action for a lost frame
	problem. Often when video is lost the same frame will be sent over 
	and over again.  This value represents how many times we get the
	same frame before we fall into the lost video buffering mode. A value
	to low could cause performance problems if the video can self recover
	a value to high will cause lengthy delays.
	Default = 10

* @page trouble Trouble Shooting
	@li Make sure you have WMP 10+
	@li Make sure .Net 4.0 full is installed
	@li Make sure CiscoSource.ax is registered, (use regsvr32 Ciscosource.ax)
	@li CiscoSource.ax is in the same directory as Helperlib.dll, log4net.dll, and vsomservices.dll
	@li You have write access to a sud-directory called logs
*								
* @page tdo Things To Do
 * @section Cisco Filter
 	@li Seperate the VideoDecoder into its own transformfilter
 	@li Support different streams

 * @section Samples
 * 
*
* @page release Release Notes
* @remarks Date     Version     Author          Notes
* @remarks 11/18/13   1.0       Horth, D        Initial release
 * 
 * @page issues Known Issues
 * - Windows media player will crash.
 * 
* @page depreciated Depreciated
*/


#pragma once
#include <streams.h>
#include <shlwapi.h>
#include <sstream>
#include <cctype>       // std::toupper
#include <atlbase.h>
#include <atlconv.h>
#include <fstream>
#include <stdarg.h>
#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <initguid.h>
#include <regex>
#include <string.h>
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>

//Link in our dependent library
//Direct Show
#pragma comment (lib,"strmbase.lib")

//Live555 & ffmpeg proxy
#pragma comment (lib,"LiveProxy.lib")

//windows
#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"Ws2_32.lib")