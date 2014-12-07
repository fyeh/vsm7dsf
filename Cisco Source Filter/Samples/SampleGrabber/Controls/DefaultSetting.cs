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
    /// class model for the the registry settings
    /// </summary>
    public class DefaultSetting
    {
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <param name="tooltip"></param>
        public DefaultSetting(string name, string value, string tooltip)
        {
            Name = name;
            Value = value;
            Tooltip = tooltip;
        }

        /// <summary>
        /// Name of the setting
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// Current value of the setting
        /// </summary>
        public string Value { get; set; }

        /// <summary>
        /// Tooltip for the setting
        /// </summary>
        public string Tooltip { get; set; }
    }
}
