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
using Cisco.PSBU.VSOMWebService;

namespace HelperLib
{
    /// <summary>
    /// Helper class used to locate cameras on a cisco VSM
    /// pass it a valid VSM server then call <seealso cref="WalkCameras"/>
    /// with a callback function.
    /// </summary>
    public class CameraSupport
    {
        private static VsomServices _vsomServices;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="vServices">Valid VSM server instance</param>
        public CameraSupport(VsomServices vServices)
        {
            _vsomServices = vServices;
        }

        /// <summary>
        /// Enumerate all the cameras on the VSM, returning them in the
        /// callback function
        /// </summary>
        /// <param name="callback">A valid VSM device (camera), host address of the
        /// camera, and the security token</param>
        public void WalkCameras(Action<device, string, string> callback)
        {
            try
            {
                var dF = new cameraFilter {byNameContains = string.Empty}; // create empty filter
                //walk through all cameras, not  needed for that example, but good to demo it from SDK point of view
                //TraceHelper.WriteInfo("\nCameras List: ");
                var pI = new pageInfo();
                dF.pageInfo = pI;
                dF.pageInfo.limit = 100;
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

        /// <summary>
        /// helper function for <see cref="WalkCameras"/>, basically this is where
        /// we get the security token
        /// </summary>
        /// <param name="callback">function to call with a camera, host, and token</param>
        /// <param name="camera">the camera we are getting the token for</param>
        private static void AddCamera(Action<device, string, string> callback, Cisco.PSBU.VSOMWebService.device camera)
        {
            try
            {
                // only look for cameras with IP address
                TraceHelper.WriteDebug(string.Format("Camera: {0} @ {1}, States: {2}/{3}/{4}",
                    camera.name, camera.deviceAccess.hostname_ip, camera.adminState, camera.operState, camera.model));
                int tokenExpirationTime = 3600; //86400;
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