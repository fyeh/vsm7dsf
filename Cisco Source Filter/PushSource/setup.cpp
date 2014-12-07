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
#pragma unmanaged
#include "PushGuids.h"
#include "PushSource.h"

/**
Register are filter as a directshow filter
*/
void RegisterFilterCisco()
{
	 HKEY   hkey;
	 DWORD  dwDisposition; 
	 if(RegCreateKeyEx(HKEY_CLASSES_ROOT, 
					   TEXT("CiscoSource"), 
					   0, 
					   NULL, 
					   REG_OPTION_NON_VOLATILE, 
					   KEY_ALL_ACCESS, 
					   NULL, 
					   &hkey, 
					   &dwDisposition) == ERROR_SUCCESS)
	 {
		  LPCTSTR value = TEXT("Source Filter");
		  LPCTSTR clsid = TEXT(FILTER_GUID);
		  
		  int result = RegSetValueEx(hkey, value, 0, REG_SZ, (BYTE*)clsid, 76);//wstrlen(clsid) + 1);
		  RegCloseKey(hkey);
	 }

	 if(RegCreateKeyEx(HKEY_CURRENT_USER, 
					   TEXT("Software\\Microsoft\\MediaPlayer\\Player\\Schemes\\CiscoSource"), 
					   0, 
					   NULL, 
					   REG_OPTION_NON_VOLATILE, 
					   KEY_ALL_ACCESS, 
					   NULL, 
					   &hkey, 
					   &dwDisposition) == ERROR_SUCCESS)
	 {
		  LPCTSTR value = TEXT("Runtime");
		  DWORD clsid = 1;
		  int result = RegSetValueEx(hkey, value, 0, REG_DWORD, (BYTE*)&clsid, sizeof(DWORD));
		  
		  RegCloseKey(hkey);
	 }
}

/**
Simple helper function
*/
void RegisterFilter()
{
	RegisterFilterCisco();
}


// Note: It is better to register no media types than to register a partial 
// media type (subtype == GUID_NULL) because that can slow down intelligent connect 
// for everyone else.

// For a specialized source filter like this, it is best to leave out the 
// AMOVIESETUP_FILTER altogether, so that the filter is not available for 
// intelligent connect. Instead, use the CLSID to create the filter or just 
// use 'new' in your application.


// Filter setup data
const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL      // Minor type
};

const AMOVIESETUP_PIN sudOutputPinCisco = 
{
    L"Output",      // Obsolete, not used.
    FALSE,          // Is this pin rendered?
    TRUE,           // Is it an output pin?
    FALSE,          // Can the filter create zero instances?
    FALSE,          // Does the filter create multiple instances?
    &CLSID_NULL,    // Obsolete.
    NULL,           // Obsolete.
    1,              // Number of media types.
    &sudOpPinTypes  // Pointer to media types.
};

const AMOVIESETUP_FILTER sudPushSourceCisco =
{
    &CLSID_PushSourceCisco,// Filter CLSID
    g_wszPushCisco,       // String name
    MERIT_DO_NOT_USE,       // Filter merit
    1,                      // Number pins
    &sudOutputPinCisco    // Pin details
};

// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance.
// We provide a set of filters in this one DLL.

CFactoryTemplate g_Templates[1] = 
{
    { 
      g_wszPushCisco,               // Name
      &CLSID_PushSourceCisco,       // CLSID
      CPushSourceCisco::CreateInstance, // Method to create an instance of MyComponent
      NULL,                           // Initialization function
      &sudPushSourceCisco           // Set-up information (for filters)
    },
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);    



////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////

STDAPI DllRegisterServer()
{
	RegisterFilter(); 
	HRESULT hr=AMovieDllRegisterServer2( TRUE );
    return hr;
}

STDAPI DllUnregisterServer()
{
	HRESULT hr=AMovieDllRegisterServer2( FALSE );
    return hr;
}

//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

