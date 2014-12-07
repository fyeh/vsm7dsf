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
    /// View for image control
    /// <see cref="ViewModel"/>
    /// The image control allows the user to get the current frame
    /// from the stream via the sample grabber filter
    /// </summary>
    public partial class Image 
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public Image()
        {
            InitializeComponent();
            DataContext = ViewModel.Get();
        }
    }
}
