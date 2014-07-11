namespace FindCameras.View
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow 
    {
        public MainWindow()
        {
            InitializeComponent();

            //create and assign view model
            DataContext = new ViewModel.ViewModel();
        }
    }
}
