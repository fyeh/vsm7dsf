vsm7dsf
=======

DirectShow Filter for Cisco VSM7

Description

  Provides access to streaming video from Cisco Video Surveillance Manager version 7 from Windows DirectShow. 
  
Pre-Req's

LiveProxy - https://github.com/dhorth/LiveProxy
  Provides a single dynamically linked library that combines the functions of Live555 and FFMPeg.
  
  ffMpeg - http://ffmpeg.zeranoe.com/builds/
    Codecs used by the Filter
  
  live555 - http://www.live555.com/liveMedia/
    Support for RTSP
    
Cisco VSM 7 SDK - https://developer.cisco.com/site/networking/security/physical-security/documentation/index.gsp
  Provides libraries used to interface with Cisco VSM 7 Web Services
  
Directshowlib - http://directshownet.sourceforge.net/
  .NET access to DirectShow Used by Samples
  
Log4net - http://logging.apache.org/log4net/
  Logging libraries Used by Samples
  
Project Contents

  PushSource - The Actual DirectShow Filter.

  HelperLib - C# managed library.  Used to integrate the managed Cisco VSM SDK with the unmanaged direct show filter.
  
  Samples - Samples that use the Filter and Cisco Web Services calls. Also useful as utilities for deploying.
