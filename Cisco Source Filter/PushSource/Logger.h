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

#define LOGLEVEL_OFF 0
#define LOGLEVEL_CRITICAL 1
#define LOGLEVEL_ERROR 2
#define LOGLEVEL_WARN 3
#define LOGLEVEL_INFO 4
#define LOGLEVEL_VERBOSE 5
#define LOGLEVEL_DEBUG 6

#define TRACE_CRITICAL(x,...) Log(LOGLEVEL_CRITICAL, __FUNCTION__, x, ##__VA_ARGS__) 
#define TRACE_ERROR(x,...) Log(LOGLEVEL_ERROR, __FUNCTION__, x, ##__VA_ARGS__) 
#define TRACE_WARN(x,...) Log(LOGLEVEL_WARN, __FUNCTION__, x, ##__VA_ARGS__) 
#define TRACE_INFO(x,...) Log(LOGLEVEL_INFO, __FUNCTION__, x, ##__VA_ARGS__) 
#define TRACE_VERBOSE(x,...) Log(LOGLEVEL_VERBOSE, __FUNCTION__, x, ##__VA_ARGS__) 
#define TRACE_DEBUG(x,...) Log(LOGLEVEL_DEBUG, __FUNCTION__, x, ##__VA_ARGS__) 

extern int g_logLevel;

extern "C"
{
	_declspec(dllexport) void Write(int level, const char * msg);
	_declspec(dllexport) void Log(int level, const char * functionName, const char * lpszFormat, ...);
}
