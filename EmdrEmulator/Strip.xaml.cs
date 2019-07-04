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
    public partial class Strip : UserControl
    {
        private Model model = new Model();

        public Strip()
        {
            InitializeComponent();
            DataContext = model;
        }

        public class LedModel
        {
            public int Number { get; set; }
            public SolidColorBrush Brush { get; set; }

            public LedModel(int number, SolidColorBrush brush)
            {
                Number = number;
                Brush = brush;
            }
        }

        public class Model : ModelBase
        {
            private List<LedModel> _leds = new List<LedModel>();
            public List<LedModel> Leds
            {
                get => _leds;
                set => SetProperty(ref _leds, value);
            }

            public static Model EmdrDesignTime
            {
                get
                {
                    var dataContext = new Model();
                    dataContext.Leds.Add(new LedModel(0, Brushes.Red));
                    dataContext.Leds.Add(new LedModel(10, Brushes.Green));
                    dataContext.Leds.Add(new LedModel(50, Brushes.Blue));
                    dataContext.Leds.Add(new LedModel(180, Brushes.White));

                    return dataContext;
                }
            }

            public static Model RemoteControlDesignTime
            {
                get
                {
                    var dataContext = new Model();
                    dataContext.Leds.Add(new LedModel(0, Brushes.Blue));

                    return dataContext;
                }
            }


        }
    }
}
