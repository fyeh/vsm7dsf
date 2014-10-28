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
    
Cisco VSM 7 SDK - https://developer.cisco.com/fileMedia/download/2b462c5c-8f5a-49d2-892c-4c87edfc6b37

    Provides lilbraries used to interface with Cisco Web Services. The required dll's are:
      
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
