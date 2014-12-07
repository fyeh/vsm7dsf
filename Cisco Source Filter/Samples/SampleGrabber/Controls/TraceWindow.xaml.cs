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

namespace SampleGrabber.Controls
{
    /// <summary>
    /// View for Trace window
    /// <see cref="ViewModel"/>
    /// This control acts like an internal trace window, only showing
    /// log items from the core product
    /// </summary>
    public partial class TraceWindow
    {
        public TraceWindow()
        {
            InitializeComponent();
            DataContext = ViewModel.Get();
        }
    }
}
