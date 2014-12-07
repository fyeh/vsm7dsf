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
using System.Windows.Input;

namespace SampleGrabber
{
    /// <summary>
    /// WPF helper for viewmodel commands
    /// </summary>
    public class RelayCommand : ICommand
    {
        private readonly Func<object, bool> _canExecute = (parameter) => true;
        private readonly Action<object> _execute = (parameter) => { };

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="execute"></param>
        public RelayCommand(Action<object> execute)
        {
            _execute = execute;
        }

        /// <summary>
        /// Constructor with execute and can execute methods
        /// </summary>
        /// <param name="execute"></param>
        /// <param name="canExecute"></param>
        public RelayCommand(Action<object> execute, Func<object, bool> canExecute)
            : this(execute)
        {
            _canExecute = canExecute;
        }


        public void OnCanExecuteChanged()
        {
            if (CanExecuteChanged != null)
                CanExecuteChanged(this, new EventArgs());
        }
        #region ICommand Members

        public event EventHandler CanExecuteChanged = delegate { };


        public bool CanExecute(object parameter)
        {
            return _canExecute.Invoke(parameter);
        }


        public void Execute(object parameter)
        {
            _execute.Invoke(parameter);
        }


        #endregion
    }
}