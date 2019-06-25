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
    /// Interaction logic for StripLed.xaml
    /// </summary>
    public partial class StripLed : UserControl
    {
        private Model model = new Model();

        public StripLed()
        {
            InitializeComponent();
            DataContext = model;
        }


        public class Led
        {
            public int Number { get; set; }
            public SolidColorBrush Brush { get; set; }

            public Led(int number, SolidColorBrush brush)
            {
                Number = number;
                Brush = brush;
            }
        }

        public class Model : ModelBase
        {
            private List<Led> _leds = new List<Led>();
            public List<Led> Leds
            {
                get => _leds;
                set => SetProperty(ref _leds, value);
            }

            public static Model DesignTime
            {
                get
                {
                    var dataContext = new Model();
                    dataContext.Leds.Add(new Led(0, Brushes.DarkSalmon));
                    dataContext.Leds.Add(new Led(1, Brushes.DarkSeaGreen));
                    dataContext.Leds.Add(new Led(2, Brushes.DarkViolet));
                    dataContext.Leds.Add(new Led(3, Brushes.Black));
                    dataContext.Leds.Add(new Led(4, Brushes.White));

                    return dataContext;
                }
            }

        }
    }
}
