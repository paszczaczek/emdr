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
            private string[] _lines = new string[2] { "", "" };
            public string[] Lines
            {
                get => _lines;
                set => SetProperty(ref _lines, value);
            }

            private int _row;
            public int Row
            {
                get => _row;
                set
                {
                    if (value < 0 || value > 1)
                        throw new ArgumentException("allowed value 0..1", "row");
                    _row = value;
                }
            }

            private int _col;
            public int Col
            {
                get => _col;
                set
                {
                    if (value < 0 || value > 16)
                        throw new ArgumentException("allowed value 0..16", "col");
                    _col = value;
                }
            }

            private bool _backlight;
            public bool Backlight
            {
                get => _backlight;
                set => SetProperty(ref _backlight, value);
            }


            public static Model DesignTime
            {
                get
                {
                    var model = new Model();
                    model.Lines = new string[2]
                    {
                        "1234567890123456",
                        "abcdefghijklmnou"
                    };
                    model.Backlight = true;

                    return model;
                }
            }
        }
    }
}
