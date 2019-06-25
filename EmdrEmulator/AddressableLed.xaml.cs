using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace EmdrEmulator
{
    /// <summary>
    /// Interaction logic for AddressableLed.xaml
    /// </summary>
    public partial class AddressableLed : UserControl
    {
        private MyDataContext myDataContext = new MyDataContext();

        public AddressableLed()
        {
            InitializeComponent();
            DataContext = myDataContext;
        }

        public class MyDataContext : DataContextBase
        {
            private int _ledNumber;
            public int LedNumber {
                get => _ledNumber;
                set => SetProperty(ref _ledNumber, value);
            }

            private SolidColorBrush _brush;
            public SolidColorBrush Brush
            {
                get => _brush;
                set => SetProperty(ref _brush, value);
            }

            public static MyDataContext DesignTime
            {
                get
                {
                    var dataContext = new MyDataContext();
                    dataContext.LedNumber = 10;
                    dataContext.Brush = Brushes.DarkSalmon;

                    return dataContext;
                }
            }

        }
    }
}
