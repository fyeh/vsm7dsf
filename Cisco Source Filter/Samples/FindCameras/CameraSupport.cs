// IBM Confidential
//
// OCO Source Material
//
// 5725H94
//
// (C) Copyright IBM Corp. 2005,2006
//
// The source code for this program is not published or otherwise divested
// of its trade secrets, irrespective of what has been deposited with the
// U. S. Copyright Office.
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.

using System;
using System.Diagnostics;
using Cisco.PSBU.VSOMWebService;
using HelperLib;

namespace FindCameras
{
    public class CameraSupport
    {
        private static VsomServices _vsomServices;

        public CameraSupport(VsomServices vServices)
        {
            _vsomServices = vServices;
        }

        public void WalkCameras(Action<device, string, string> callback)
        {
            try
            {
                var dF = new cameraFilter {byNameContains = string.Empty}; // create empty filter
                //walk through all cameras, not  needed for that example, but good to demo it from SDK point of view
                //TraceHelper.WriteInfo("\nCameras List: ");
                var pI = new pageInfo();
                dF.pageInfo = pI;
                dF.pageInfo.limit = 50; // using reasonable page limit to keep response size under control !
                int i = 0;
                page cameraRefList;
                do
                {
                    cameraRefList = _vsomServices.CameraService.getCameras(dF);
                    if ((cameraRefList == null) || (cameraRefList.totalRows == 0))
                    {
                        TraceHelper.WriteInfo(string.Format("Did not find any cameras"));
                        callback(null, null, null);
                        break;
                    }

                    foreach (object item in cameraRefList.items)
                    {
                        var camera = item as device;
                        if (camera != null && camera.deviceAccess != null)
                            AddCamera(callback, camera);


                        i++;
                    }
                    dF.pageInfo.start = i;
                } while (i < cameraRefList.totalRows);
            }
            catch (Exception ex)
            {
                ExceptionHelper.HandleException(ex);
            }
            callback(null, null, null);
        }

        private static void AddCamera(Action<device, string, string> callback, device camera)
        {
            try
            {
                // only look for cameras with IP address
                TraceHelper.WriteDebug(string.Format("Camera: {0} @ {1}, States: {2}/{3}/{4}",
                    camera.name, camera.deviceAccess.hostname_ip, camera.adminState, camera.operState, camera.model));
                int tokenExpirationTime = 86400;
                // 24 hours --- ONLY for DEBUG, real application suppose to set to ~5 min,

                camera.managedByRef.refObjectTypeSpecified = true;
                baseObjectRef cameraRef = baseObject2Ref.bO2Ref(camera);

                TraceHelper.WriteDebug("Getting security token for {0}", camera.name);
                string token = _vsomServices.CameraService.getSecurityToken(cameraRef, tokenExpirationTime);

                TraceHelper.WriteDebug("Getting host ip for {0}", camera.name);
                var ums = _vsomServices.UmsService.getUms(camera.managedByRef);
                var host = ums.deviceAccess.hostname_ip;

                callback(camera, host, token);
            }
            catch (Exception ex)
            {
                ExceptionHelper.HandleException(ex);
            }
        }
    }
}