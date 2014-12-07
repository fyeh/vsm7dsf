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

using System.Windows.Controls;

namespace SampleGrabber.Controls
{
    /// <summary>
    /// View for Status control
    /// <see cref="ViewModel"/>
    /// The status control display information about the current system status
    /// green indicators mean everything is installed correctly.  Red indicates
    /// that a critical component is not correctly install on the machine
    /// </summary>
    public partial class SystemStatus 
    {
        /// <summary>
        /// constructor
        /// </summary>
        public SystemStatus()
        {
            InitializeComponent();
            DataContext = ViewModel.Get();
        }
    }
}
