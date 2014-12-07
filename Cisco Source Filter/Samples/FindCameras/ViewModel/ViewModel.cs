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
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using System.Windows;
using Cisco.PSBU.VSOMWebService;
using FindCameras.Annotations;
using FindCameras.Model;
using HelperLib;
using Microsoft.Win32;

namespace FindCameras.ViewModel
{
    /// <summary>
    /// The viewmodel for our application, this is where all
    /// the magic happens
    /// </summary>
    public class ViewModel : INotifyPropertyChanged
    {
        private CameraSupport _cameraSupport;

        /// <summary>
        /// Creates an instance of the viewmodel and reads in our default
        /// values for server, user and password from the app.config
        /// </summary>
        public ViewModel()
        {
            Server = ProgramSettings.GetSetting("Server");
            UserName = ProgramSettings.GetSetting("User");
            Password = ProgramSettings.GetSetting("Password");

            Cameras = new ObservableCollection<CiscoCamera>();
            LoadCamerasCommand = new RelayCommand(ExecuteLoadCameras, CanLoadCameras);
            ExportCommand = new RelayCommand(ExecuteExport, CanExport);
        }

        #region Title

        /// <summary>
        /// Gets the title of the appliation including a version stamp
        /// and a build date
        /// </summary>
        public string Title
        {
            get {
                Assembly asm = Assembly.GetEntryAssembly() ?? Assembly.GetCallingAssembly();
                FileVersionInfo ver = FileVersionInfo.GetVersionInfo(asm.Location);
                return "Find Cameras: " + ver.FileVersion +
                       string.Format(" ({0}{1}_{2}_{3})", File.GetLastWriteTime(ver.FileName).Month,
                           File.GetLastWriteTime(ver.FileName).Day,
                           File.GetLastWriteTime(ver.FileName).Hour,
                           File.GetLastWriteTime(ver.FileName).Minute);

            }
        }

        #endregion

        #region IsLoading

        private bool _isLoading;

        /// <summary>
        /// Used to indicate that we are currently trying to enumerate the
        /// cameras on the VSM.
        /// </summary>
        public bool IsLoading
        {
            get { return _isLoading; }
            set
            {
                if (_isLoading == value)
                    return;
                _isLoading = value;
                OnPropertyChanged("IsLoading");
                LoadCamerasCommand.OnCanExecuteChanged();
            }
        }

        #endregion

        #region Status

        private string _status;

        /// <summary>
        /// Used to indicate what operation is currently happening.
        /// </summary>
        public string Status
        {
            get { return _status; }
            set
            {
                if (_status == value)
                    return;
                _status = value;
                OnPropertyChanged("Status");
            }
        }

        #endregion

        #region Cameras

        /// <summary>
        /// The collection of cameras we discovered
        /// </summary>
        public ObservableCollection<CiscoCamera> Cameras { get; private set; }

        #endregion

        #region Server

        private string _server;

        /// <summary>
        /// The name/address of the server.
        /// </summary>
        public string Server
        {
            get { return _server; }
            set
            {
                if (_server == value)
                    return;
                _server = value;
                ProgramSettings.SetSetting("Server", value);
                OnPropertyChanged("Server");
            }
        }

        #endregion

        #region UserName

        private string _userName;

        /// <summary>
        /// The user name to authenicate with to the server.
        /// </summary>
        public string UserName
        {
            get { return _userName; }
            set
            {
                if (_userName == value)
                    return;
                _userName = value;
                ProgramSettings.SetSetting("User", value);
                OnPropertyChanged("UserName");
            }
        }

        #endregion

        #region Password

        private string _password;

        /// <summary>
        /// The password for the VSM.
        /// </summary>
        public string Password
        {
            get { return _password; }
            set
            {
                if (_password == value)
                    return;
                _password = value;
                ProgramSettings.SetSetting("Password", value);
                OnPropertyChanged("Password");
            }
        }

        #endregion

        #region SelectedCamera

        private CiscoCamera _selectedCamera;

        /// <summary>
        /// The currently selected camera.
        /// </summary>
        public CiscoCamera SelectedCamera
        {
            get { return _selectedCamera; }
            set
            {
                if (_selectedCamera == value)
                    return;
                _selectedCamera = value;
                OnPropertyChanged("SelectedCamera");
            }
        }

        #endregion

        #region LoadCamerasCommand

        /// <summary>
        /// Defines the LoadCameras Command
        /// <seealso cref="ExecuteLoadCameras"/>
        /// <seealso cref="CanLoadCameras"/>
        /// </summary>
        public RelayCommand LoadCamerasCommand { get; private set; }

        /// <summary>
        /// Check the security service to make sure that this operation is
        /// available at the current time
        /// todo: add custom logic to check availabiltiy of command
        /// <seealso cref="LoadCamerasCommand"/>
        /// <seealso cref="ExecuteLoadCameras"/>
        /// </summary>
        /// <param name="arg"></param>
        /// <returns>true if the command is available</returns>
        public virtual bool CanLoadCameras(object arg)
        {
            return !IsLoading;
        }

        /// <summary>
        /// Execute the LoadCameras Command
        /// <seealso cref="LoadCamerasCommand"/>
        /// <seealso cref="CanLoadCameras"/>
        /// </summary>
        /// <param name="arg"></param>
        public virtual void ExecuteLoadCameras(object arg)
        {
            try
            {
                Status = "Loading";
                IsLoading = true;
                Cameras.Clear();
                ProgramSettings.SetSetting("Server",Server);
                Task.Factory.StartNew(LoadCameras);
            }
            catch (Exception ex)
            {
                ExceptionHelper.HandleException(ex);
                MessageBox.Show("Failed to load cameras. \n" + ex.Message);
            }
        }


        #endregion

        #region ExportCommand

        /// <summary>
        /// Defines the Export Command
        /// <seealso cref="ExecuteExport"/>
        /// <seealso cref="CanExport"/>
        /// </summary>
        public RelayCommand ExportCommand { get; private set; }

        /// <summary>
        /// Check the security service to make sure that this operation is
        /// available at the current time
        /// todo: add custom logic to check availabiltiy of command
        /// <seealso cref="ExportCommand"/>
        /// <seealso cref="ExecuteExport"/>
        /// </summary>
        /// <param name="arg"></param>
        /// <returns>true if the command is available</returns>
        public virtual bool CanExport(object arg)
        {
            try
            {
                return Cameras.Count > 0;
            }
            catch (Exception ex)
            {
                ExceptionHelper.HandleException(ex);
            }
            return false;
        }

        /// <summary>
        /// Execute the Export Command
        /// <seealso cref="ExportCommand"/>
        /// <seealso cref="CanExport"/>
        /// </summary>
        /// <param name="arg"></param>
        public virtual void ExecuteExport(object arg)
        {
            try
            {
                TraceHelper.WriteInfo("Exporting results");

                //ask where to save the results
                var dlg = new SaveFileDialog
                {
                    FileName = "CameraList_" + Server + ".csv",
                    DefaultExt = ".csv",
                    Filter = "CSV|.csv",
                    OverwritePrompt = true,
                    Title = "Export File Name",
                    ValidateNames = true
                };

                //go ahead with the export
                if (dlg.ShowDialog().GetValueOrDefault(false))
                {
                    //first the column headers
                    string txt = Cameras[0].CameraProperties.Keys.Aggregate("",
                        (current, prop) => current + (prop + ","));
                    txt += "CiscoUrl,RtspUrl\r\n";

                    //Now the actual property values
                    foreach (var camera in Cameras)
                    {
                        TraceHelper.WriteInfo(camera.Name);
                        foreach (var cProps in camera.CameraProperties.Values)
                        {
                            if (cProps != null)
                                txt += cProps.Replace(',', ' ') + ",";
                            else
                                txt += ",";
                        }
                        txt += camera.CiscoUrl + "," + camera.RtspUrl + "\r\n";
                    }
                    TraceHelper.WriteDebug(txt);
                    TraceHelper.WriteInfo("Exported to " + dlg.FileName);
                    File.WriteAllText(dlg.FileName, txt);

                    //start the filename
                    Process.Start(dlg.FileName);

                }
            }
            catch (Exception ex)
            {
                ExceptionHelper.HandleException(ex);
                MessageBox.Show("Export Failed\n" + ex.Message);
            }
        }

        #endregion

        /// <summary>
        /// Authenicate with the server then load the cameras
        /// </summary>
        private void LoadCameras()
        {
            TraceHelper.WriteInfo("Starting Load Camera Server: {0} User: {1}", Server, UserName);
            VsomServices vsomServices = null;
            try
            {
                Status = "Authenicating";
                vsomServices = new VsomServices(Server, string.Empty, UserName, Password, true, true);
            }
            catch (Exception ex)
            {
                ExceptionHelper.HandleException(ex);
                MessageBox.Show(
                    "An error occurred while trying to login to the VSM.  Please check the credentitials and try again.\n\n" +
                    ex.Message);
            }
            TraceHelper.WriteInfo("Authenicated. Loading Cameras");
            try
            {
                Status = "Cameras";
                _cameraSupport = new CameraSupport(vsomServices);

                TraceHelper.WriteInfo("Walking Cameras");
                _cameraSupport.WalkCameras(AddCamera);
                //cameraSupport.cameraOperations(camera, cmd);
            }
            catch (Exception ex)
            {
                ExceptionHelper.HandleException(ex);
                MessageBox.Show(
                    "An error occurred while enumerating the cameras.  Please check the VSM and try again.\n\n" +
                    ex.Message);
            }
        }

        /// <summary>
        /// Callback for the WalkCamera call
        /// </summary>
        /// <param name="camera">device that represents a camera</param>
        /// <param name="host">hostname for the camera</param>
        /// <param name="token">the security token for the camera</param>
        private void AddCamera(device camera, string host, string token)
        {
            if (!Application.Current.Dispatcher.CheckAccess())
            {
                Application.Current.Dispatcher.BeginInvoke(new Action(() => AddCamera(camera, host, token)));
                return;
            }

            if (camera != null)
            {
                Status = camera.name;
                TraceHelper.WriteInfo("Adding Camera {0}", camera.name);
                Cameras.Add(new CiscoCamera(camera, host, token, UserName, Password));
            }
            
            IsLoading = camera != null;
            if (!IsLoading)
                TraceHelper.WriteInfo("Found last camera");
            ExportCommand.OnCanExecuteChanged();
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
