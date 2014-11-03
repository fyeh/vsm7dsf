vsm7dsf
=======

DirectShow Filter for Cisco VSM7

Description

  Provides access to streaming video from Cisco Video Surveillance Manager version 7 from Windows DirectShow. 
  
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

  These will be inlcuded in the releases of this project
  
      Directshowlib - http://directshownet.sourceforge.net/
        .NET access to DirectShow Used by Samples
      Log4net - http://logging.apache.org/log4net/
        Logging libraries Used by Samples
  
Project Contents

  PushSource - The Actual DirectShow Filter.

  HelperLib - C# managed library.  Used to integrate the managed Cisco VSM SDK with the unmanaged direct show filter.
  
  Samples - Samples that use the Filter and Cisco Web Services calls. Also useful as utilities for deploying.
