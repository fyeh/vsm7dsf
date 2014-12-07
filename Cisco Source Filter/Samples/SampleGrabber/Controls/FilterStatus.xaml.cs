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

namespace SampleGrabber
{
    /// <summary>
    /// View for Filter status
    /// <see cref="ViewModel"/>
    /// The filter status shows a graphical view of the graph contruction. 
    /// Green indicates that the graph is correctly constructed and connected
    /// </summary>
    public partial class FilterStatus
    {
        private readonly ViewModel _viewModel;

        /// <summary>
        /// Constructor
        /// </summary>
        public FilterStatus()
        {
            InitializeComponent();
            _viewModel = ViewModel.Get();
            DataContext = _viewModel;
        }

        /// <summary>
        /// Start the graph
        /// </summary>
        /// <param name="sourceUrl">Ciscosource url</param>
        public void Start(string sourceUrl)
        {
            try
            {
                _viewModel.Start(sourceUrl);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }

        /// <summary>
        /// Stop the graph
        /// </summary>
        public void Stop()
        {
            _viewModel.MediaControl.Stop();
            _viewModel.Running = false;
        }
    }
}
