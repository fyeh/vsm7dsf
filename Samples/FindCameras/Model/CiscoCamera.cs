using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media.Imaging;
using Cisco.PSBU.VSOMWebService;
using FindCameras.Annotations;
using HelperLib;

namespace FindCameras.Model
{
    /// <summary>
    /// this is the model of a cisco camera object
    /// it contains all the properties of a camera
    /// including the ability to download its
    /// own thumbnail image
    /// 
    /// NOTE: its a bit of hack but this 'model' is also
    /// the view model for the camera.  Because of the 
    /// limited time and duration of the project this 
    /// seemed like a reasonable shortcut
    /// </summary>
    public class CiscoCamera : INotifyPropertyChanged
    {
        private readonly string _user;
        private readonly string _password;
        private readonly int _width;
        private readonly int _height;


        /// <summary>
        /// Camera constructor
        /// </summary>
        /// <param name="camera">The device object from the VSM that represents a camera</param>
        /// <param name="host">The host address of the camera</param>
        /// <param name="token">The security token of the camera</param>
        /// <param name="user">The user name and password of the VSM (used to create the cisco url)</param>
        /// <param name="password">The user name and password of the VSM (used to create the cisco url)</param>
        public CiscoCamera(device camera, string host, string token, string user, string password)
        {
            _user = user;
            _password = password;
            _width = 1920;
            _height = 1080;
            CopyUrlToClipboardCommand = new RelayCommand(ExecuteCopyUrlToClipboard, CanCopyUrlToClipboard);
            CameraProperties=new Dictionary<string, string>
            {
                {"UID", camera.uid},
                {"Name", camera.name},
                {"Host", host},
                {"State", camera.deviceState.aggregateState.ToString()},
                {"Model", camera.modelName},
                {"AdminState", camera.adminState.ToString()},
                {"MultiCastIpAddress", camera.multicastIp_primary},
                {"IpAddress", camera.deviceAccess.hostname_ip},
                {"Vendor", camera.vendor},
                {"Token", token},
                {"ObjectType", camera.objectType.ToString()}
            };
            Task.Factory.StartNew(GetThumbnail);
        }

        /// <summary>
        /// The unique id of the camera
        /// </summary>
        public string UID { get { return CameraProperties["UID"]; } }

        /// <summary>
        /// The name of the camera
        /// </summary>
        public string Name { get { return CameraProperties["Name"]; } }

        /// <summary>
        /// The host name of ip address of the camera
        /// </summary>
        public string Host { get { return CameraProperties["Host"]; } }

        /// <summary>
        /// The current operating state of the camera ('ok' is what we want)
        /// </summary>
        public string State { get { return CameraProperties["State"]; } }

        /// <summary>
        /// The model number of the camera
        /// </summary>
        public string Model { get { return CameraProperties["Model"]; } }

        /// <summary>
        /// The IP address of the physical camera
        /// </summary>
        public string IpAddress { get { return CameraProperties["IpAddress"]; } }

        /// <summary>
        /// The vendor name of the camera
        /// </summary>
        public string Vendor { get { return CameraProperties["Vendor"]; } }

        /// <summary>
        /// The security token for the camera
        /// </summary>
        public string Token { get { return CameraProperties["Token"]; } }

        /// <summary>
        /// the device object type, should always be camera
        /// </summary>
        public string ObjectType { get { return CameraProperties["ObjectType"]; } }


        /// <summary>
        /// Collection of properties, used as part of the export
        /// </summary>
        public Dictionary<string, string> CameraProperties { get; private set; }

        #region Thumbnail

        private BitmapSource _thumbnail;

        /// <summary>
        /// Propery value for Thumbnail which will call
        /// NotifyPropertyChanged when the value of
        /// this property has changed.
        /// </summary>
        public BitmapSource Thumbnail
        {
            get { return _thumbnail; }
            set
            {
                _thumbnail = value;
                OnPropertyChanged("Thumbnail");
            }
        }

        #endregion

        /// <summary>
        /// A valid RTSP address to the camera
        /// NOTE this is only valid as long as the security token is valid.
        /// You cannot store and reuse this URL it must be created fresh each
        /// time.
        /// </summary>
        public string RtspUrl { get { return string.Format("rtsp://{0}/{1}?token={2}", CameraProperties["Host"], CameraProperties["UID"], CameraProperties["Token"]); } }

        /// <summary>
        /// A valid address for our direct show filter.  Note that unlike the <see cref="RtspUrl"/> this address is 
        /// valid and can be stored.  Since the security token is not part of the URL the filter we use the connection
        /// information to get its own security token
        /// </summary>
        public string CiscoUrl { get { return string.Format("ciscosource://{0}?camera={1}&User={2}&Password={3}&Width={4}&Height={5}", 
            CameraProperties["Host"], 
            CameraProperties["UID"], 
            _user,
            _password,
            _width,
            _height); } }

        #region CopyUrlToClipboardCommand

        /// <summary>
        /// Defines the CopyUrlToClipboard Command
        /// <seealso cref="ExecuteCopyUrlToClipboard"/>
        /// <seealso cref="CanCopyUrlToClipboard"/>
        /// </summary>
        public RelayCommand CopyUrlToClipboardCommand { get; private set; }

        /// <summary>
        /// Check the security service to make sure that this operation is
        /// available at the current time
        /// todo: add custom logic to check availabiltiy of command
        /// <seealso cref="CopyUrlToClipboardCommand"/>
        /// <seealso cref="ExecuteCopyUrlToClipboard"/>
        /// </summary>
        /// <param name="arg"></param>
        /// <returns>true if the command is available</returns>
        public virtual bool CanCopyUrlToClipboard(object arg)
        {
            return true;
        }

        /// <summary>
        /// Execute the CopyUrlToClipboard Command
        /// <seealso cref="CopyUrlToClipboardCommand"/>
        /// <seealso cref="CanCopyUrlToClipboard"/>
        /// </summary>
        /// <param name="arg"></param>
        public virtual void ExecuteCopyUrlToClipboard(object arg)
        {
            try
            {
                Clipboard.SetText(CiscoUrl);
            }
            catch (Exception ex)
            {
                ExceptionHelper.HandleException(ex);
                MessageBox.Show("Operation Failed");
            }
        }

        #endregion

        private void GetThumbnail()
        {
            try
            {
                TraceHelper.WriteInfo("Trying to get thumbnail for {0}",CameraProperties["Name"]);
                string lcUrl = string.Format("https://{0}/ismserver/json/camera/getThumbnails", AuthenticationInfo.HostName);
                var loHttp = (HttpWebRequest)WebRequest.Create(lcUrl);
                var encoding = new UTF8Encoding();
                string postData = "{'request':{'cameraRef':{'refUid':'" + CameraProperties["UID"]
                    + "','refName':'" + CameraProperties["Name"] + "','refObjectType':'" + CameraProperties["ObjectType"]
                    + "'},'numThumbnails':1,'forRecordings':false, 'thumbnailResolution':'full'}}";
                postData = postData.Replace('\'', '"').Trim();
                byte[] data = encoding.GetBytes(postData);
                loHttp.Method = "POST";
                loHttp.ContentLength = data.Length;
                loHttp.ContentType = "application/json";
                loHttp.Headers.Add("x-ism-sid", AuthenticationInfo.SessionId);

                Stream loPostData = loHttp.GetRequestStream();
                loPostData.Write(data, 0, data.Length);
                loPostData.Close();
                var loWebResponse = (HttpWebResponse)loHttp.GetResponse();

                Stream responseStream = loWebResponse.GetResponseStream();
                if (responseStream != null)
                { 
                    var decoder = new JpegBitmapDecoder(responseStream, BitmapCreateOptions.PreservePixelFormat,
                        BitmapCacheOption.OnLoad);
                    Thumbnail = decoder.Frames[0];
                    Thumbnail.Freeze();
                    TraceHelper.WriteInfo("Found thumbnail for {0}", CameraProperties["Name"]);
                }
                else
                {
                    TraceHelper.WriteError("Failed to get thumbnail for {0}", CameraProperties["Name"]);
                }
            }
            catch (Exception ex)
            {
                ExceptionHelper.HandleException(ex);
                TraceHelper.WriteInfo(string.Format("Exception occurred in GetThumbnail(): {0}", ex));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
