vsm7dsf
=======

DirectShow Filter for Cisco VSM7

Description

  Provides access to streaming video from Cisco Video Surveillance Manager version 7 via Windows DirectShow. 
  
Pre-Req's

LiveProxy - https://github.com/fyeh/LiveProxy/releases
  Provides all required supporting libraries in a convenient package. 
  
  Other Supporting libraries included in LiveProxy:
  
    ffMpeg - http://ffmpeg.zeranoe.com/builds/
      Codecs used by the Filter
      
    live555 - http://www.live555.com/liveMedia/
      Support for RTSP
    
Cisco VSM 7.0.1 SDK

    Provides lilbraries used to interface with Cisco Web Services. The required dll's are:
    
    The Cisco SDK redistributables are available at
        http://solutionpartner.cisco.com/web/physical-security/documentation
    1. Download: "VSM 7.0.1 SDK with Examples"
    2. Unzip: VSM 7.0.1 SDK with Examples.zip
    3. Find in: VSM7.0.1SDK\WebServicesDLLs
        VSOMWebService.dll
        VSOMWebService.XmlSerializers.dll
    
  
Other Libraries:

  These will be included in the releases of this project
  
      Directshowlib - http://directshownet.sourceforge.net/
        .NET access to DirectShow Used by Samples
      Log4net - http://logging.apache.org/log4net/
        Logging libraries Used by Samples

Microsoft pre-req's

  Microsoft Visual C++ 2008 Service Pack 1 Redistributable Package MFC Security Update
  Microsoft Visual C++ 2010 Service Pack 1 Redistributable Package MFC Security Update
  Visual C++ Redistributable for Visual Studio 2012 Update 4
  .NET 4

  
Project Contents

  PushSource - The Actual DirectShow Filter.

  HelperLib - C# managed library.  Used to integrate the managed Cisco VSM SDK with the unmanaged direct show filter.
  
  Samples - Samples that use the Filter and Cisco Web Services calls. Also useful as utilities for deploying.

Building

  Build LiveProxy first then build Cisco DS Filter. Do this for whichever configuration (Debug or Release) you are building for. 
  IE build Liveproxy for Debug then Cisco DS Filter for Debug. When you are ready to release, build LiveProxy for Release then Cisco DS Filter for Release.
  
Using

  After a successful build, the CiscoSource.ax filter will be registered on the system. Any program that uses DirectShow
  to render a URL in the specified format will load the filter.
  
  URL should be in the format:
  
  ciscosource://<Cisco VSOM Server>?camera=<Camera UUID>&User=<VSOM UserID>&Password=<Password>&Width=<Width of Stream>&Height=<Height of Stream>&Stream=<Stream UUID>
  
  Note that Width and Height are required and must match the width and height of the stream specified, but stream is optional. If stream is not specified the first stream will be used.
  You should get the3 camera UUID and/or stream UUID from the Cisco VSM Management console.
  