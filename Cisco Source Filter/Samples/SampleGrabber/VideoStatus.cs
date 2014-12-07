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
using System.ComponentModel;
using System.Windows.Forms;


namespace SampleGrabber
{
    /// <summary>
    /// The main windows dialog of sample grabber
    /// </summary>
    public partial class VideoStatus : Form
    {
        private string _sourceUrl;
        private readonly Timer _loadTimer;

        /// <summary>
        /// Create the form and load timer
        /// waiting a second before we load the graph
        /// gives the ui a chance to display
        /// </summary>
        public VideoStatus()
        {
            InitializeComponent();
            _loadTimer = new Timer { Interval = 1000 };
            _loadTimer.Tick += LoadTimerTick;

            Closing += VideoStatusClosing;
        }

        void _closeTimer_Tick(object sender, EventArgs e)
        {
            Close();
        }

        /// <summary>
        /// We load the graph on a timer tick because the
        /// graph is loaded in the main thread and we want
        /// the UI to have a chance to display
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void LoadTimerTick(object sender, EventArgs e)
        {
            try
            {
                _loadTimer.Stop();
                filterStatus1.Start(_sourceUrl);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        /// <summary>
        /// Clean up the graph when the window/application closes
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void VideoStatusClosing(object sender, CancelEventArgs e)
        {
            try
            {
                filterStatus1.Stop();
            }
            catch (Exception)
            {
            }
		}        


        /// <summary>
        /// Start the timer once the UI is loaded
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void VideoStatusLoad(object sender, EventArgs e)
        {
            _loadTimer.Start();
        }

        /// <summary>
        /// Helper to set the source url
        /// </summary>
        /// <param name="url"></param>
        public void LoadUrl(string url)
        {
            _sourceUrl = url;
        }
    }
}
