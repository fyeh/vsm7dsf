using System;
using System.Windows;
using System.Windows.Input;

namespace FindCameras
{
    /// <summary>
    /// Helper class for ICommand interface
    /// used as a binding object 
    /// </summary>
    public class RelayCommand : ICommand
    {
        private readonly Func<object, bool> _canExecute = (parameter) => true;
        private readonly Action<object> _execute = (parameter) => { };

        /// <summary>
        /// Command constructor, with just an execute
        /// </summary>
        /// <param name="execute">Code to execute</param>
        public RelayCommand(Action<object> execute)
        {
            _execute = execute;
        }


        /// <summary>
        /// Command constructor with both execute and can execute
        /// </summary>
        /// <param name="execute">code to call to execute</param>
        /// <param name="canExecute">code to call to see if we can execute</param>
        public RelayCommand(Action<object> execute, Func<object, bool> canExecute)
            : this(execute)
        {
            _canExecute = canExecute;
        }

        /// <summary>
        /// Triggers a call to the canExecute code
        /// </summary>
        public void OnCanExecuteChanged()
        {
            if (!Application.Current.Dispatcher.CheckAccess())
            {
                Application.Current.Dispatcher.Invoke(new Action(OnCanExecuteChanged));
                return;
            }
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