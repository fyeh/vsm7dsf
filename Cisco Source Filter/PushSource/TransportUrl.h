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

#pragma once
#include "stdafx.h"

   enum  DatePart{Year=0,Month=4,Day=6,Seperator=8,Hour=9,Minute=11,Second=13};


/**
Parse the Ciscosource URL into our required parameters.

The first part of the URL is the address of the VSOM server.  The filter will parse out the server address and use that to query the VSOM server camera list.
The port is optional, if specified the filter will use the port as part of the URL used to communicate with the VSOM server. If it is not specified the standard http ports will be used (80 & 443) depending on the ‘Secure’ 
The query string contains data that will be used to locate the desired video stream. It may contain name/value pairs separated by ampersands, for example ?camera=1.  The name identifier of property is case insensitive; Camera and camera are both valid.

\cond Examples
The following are examples of acceptable URI’s:
Typical URL
ciscosource://ec2-54-219-61-94.us-west-1.compute.amazonaws.com?camera=98620a82-327f-4094-8537-385b21eaee04&User=admin&Password=Admin123&Width=720&Height=480

Full URL
ciscosource://ec2-54-219-61-94.us-west-1.compute.amazonaws.com:443?camera=98620a82-327f-4094-8537-385b21eaee04&Domain=HorthSystems&User=David&Password=Horth&Width=720&Height=480&Secure=True&Framerate=30&Loglevel=5&FrameQueueSize=100

User supplied Token
ciscosource://ec2-54-219-61-94.us-west-1.compute.amazonaws.com:?camera=98620a82-327f-4094-8537-385b21eaee04&Token=98620a82-327f-4094-8537-385b21eaee04^LVEAMO^50^0^0^1382283819^e57cd1b438d6dba6356b5b0952c21f2bf56c5d27&Width=720&Height=480

User supplied Session ID
ciscosource://10.0.0.22?SessionId=70762529&Width=1920&Height=1024
\endcond
*/
   class TransportUrl 
	{

	public:
        /**
        * Constructor which takes a url string and breaks it into
        * its properties for easy access in the code
        **/
        TransportUrl(LPCTSTR url);
		~TransportUrl(){}
        
        /**
        * The complete URL
        **/
		const char* get_Url(){return m_url.c_str();}

		/**
		the converted RTSP url
		*/
		const char* get_RtspUrl(){return m_rtspUrl.c_str();}
		void set_Token(const char * host, const char * token)
		{
			m_host=host;
			m_token=token;
			m_rtspUrl = "rtsp://" + m_host + "/" + m_camera ;
			if(m_stream.length()>0)
				m_rtspUrl = m_rtspUrl+ "/" + m_stream;

			m_rtspUrl = m_rtspUrl+ "?token=" + m_token;
		}
		void clear_Token()
		{
			m_token="";
		}

		/**
		we have loaded a valid url
		*/
		bool hasUrl(){return m_token.length()>0&&m_host.length()>0;}

        /**
        * The scheme name is always Ciscosource, this is not case sensitive.  The scheme name is
        *  available once the DirectShow filter is installed and registered.  This name is recorded
        *  in the windows registry at HKEY_CURRENT_USER-Software-Microsoft-MediaPlayer-Player-Schemes.
        **/
		const char* get_Scheme(){return m_scheme.c_str();}
        
        /**
		the domain of the user
        **/
		 const char* get_Domain(){return m_domain.c_str();}
		 /**
		 the server dns name or ip
		 */
		 const char* get_Server(){return m_server.c_str();}

		 /**
		 the user name to connect to the server
		 */
		 const char* get_UserName(){return m_user.c_str();}

		 /**
		 the password for the user
		 */
		 const char* get_Password(){return m_password.c_str();}


        /**
        * The port number should only be assigned when directly accessing a video device; it will 
        * be ignored when the System Manager is specified in the Domain.
        **/
		 const char* get_Port(){return m_port.c_str();}

        /**
		 * The camera id
        **/
		  const char* get_Camera(){return m_camera.c_str();}

        /**
        *  Use the Framerate parameter in the query string to specify
        *  the speed at which frames will be retrieved from the live
        *  video stream.  The framerate setting is the number of frame
        *  per second (ie 30 would be 30 frame per second or 1 frame 
        *  every 1/33333 seconds, and 2 would be one frame every ½ second).
        *  The Framerate setting here does not have to match the actually
        *  frame rate of the video stream, if for instance you specify 5
        *  for five frames a second but the stream is set for 10 FPS then
        *  you would skip every other frame.  The reverse is true if you
        *  set this setting to 10 and the video stream is configured for
        *  5FPS then you would get the same frame twice before getting a new frame.
        **/
		  int get_Framerate(){return m_framerate;}

        /**
		  * The number of consecutive time we fail to get a frame from the
		  * queue before we restart the video stream
        **/
		  int get_LostFrameCount(){return m_lostFrameCount;}

		  /**
		  width of a single video frame
		  */
		  int get_Width(){return m_width;}

		  /**
		  height of a single video frame
		  */
		  int get_Height(){return m_height;}

		  /**
		  use a secure connection
		  */
		  bool get_IsSecure(){return m_isSecure;}

		  /**
		  the max number of frames our queue can hold
		  */
		 int get_FrameQueueSize(){return m_frameQueueSize;}

	private:
		void getValue(std::string query, LPCSTR field, std::string& returnValue, bool ignoreCase=true);
		int getInt(std::string query, LPCSTR field, bool ignoreCase=true);
		bool IsInt(std::string value);

	private:
		std::string m_url;
		std::string m_scheme;
		std::string m_domain;
		std::string m_server;
		std::string m_user;
		std::string m_password;
		std::string m_camera;
		std::string m_port;
		std::string m_rtspUrl;
		std::string m_token;
		std::string m_host;
		std::string m_stream;

		int m_framerate;
		int m_width;
		int m_height;
		int m_frameQueueSize;
		int m_lostFrameCount;

		bool m_isSecure;
	};
