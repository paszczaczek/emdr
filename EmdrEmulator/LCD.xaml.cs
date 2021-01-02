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
    /// Interaction logic for LCD.xaml
    /// </summary>
    public partial class LCD : UserControl
    {
        private Model model = new Model();

        public LCD()
        {
            InitializeComponent();
            DataContext = model;
        }

        public class Model : ModelBase
        {
            private string _line1;
            public string Line1
            {
                get => _line1;
                set => SetProperty(ref _line1, value);
            }

            private string _line2;
            public string Line2
            {
                get => _line2;
                set => SetProperty(ref _line2, value);
            }

            public static Model DesignTime
            {
                get
                {
                    var dataContext = new Model();
                    dataContext.Line1 = "1234567890123456";
                    dataContext.Line2 = "abcdefghijklmnou";

                    return dataContext;
                }
            }
        }
    }
}
