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

/** The id we register our filter as
// {B3F5D418-CDB1-441C-9D6D-2063D5538962}
*/
#define FILTER_GUID "{B3F5D418-CDB1-441C-9D6D-2063D5538962}"
DEFINE_GUID(CLSID_PushSourceCisco, 
0xb3f5d418, 0xcdb1, 0x441c, 0x9d, 0x6d, 0x20, 0x63, 0xd5, 0x53, 0x89, 0x62);


#ifdef __cplusplus
extern "C" {
#endif

DEFINE_GUID(nullGuid, 
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);

#ifdef __cplusplus
}
#endif

