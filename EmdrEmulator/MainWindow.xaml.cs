using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
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
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Timer _timer;
        private Model model = new Model();

        public MainWindow()
        {
            InitializeComponent();
            DataContext = model;

            // logowanie Serial.write()
            EmdrWrapper.Sketch.serialWriteEvent +=
                (text) => Dispatcher.Invoke(() =>
                {
                    model.SerialMonitor += $"{text}\n";
                    serialMonitorScrollViewer.ScrollToBottom();
                });

            // wyświetlanie FastLED
            unsafe
            {
                EmdrWrapper.Sketch.fastLEDShowEvent +=
                    (leds, ledsCount) => Dispatcher.Invoke(() =>
                    {
                        var stripModel = new StripLed.Model();
                        for (int i = 0; i < ledsCount; i++)
                        {
                            byte* led = leds + i * 3;
                            Color color = Color.FromRgb(led[0], led[1], led[2]);
                            stripModel.Leds.Add(new StripLed.Led(i, new SolidColorBrush(color)));
                        }
                        model.StripLedModel = stripModel;
                    });
            }

            // uruchomienie funkcji setup()
            EmdrWrapper.Sketch.setup();

            // okresowe uruchamianie funcji loop()
            bool timerWorking = false;
            _timer = new Timer(state =>
            {
                if (timerWorking)
                    return;
                timerWorking = true;
                EmdrWrapper.Sketch.loop();
                timerWorking = false;
            }, null, 0, 1000);
        }

        public class Model : ModelBase
        {
            private string _serialMonitor;
            public string SerialMonitor
            {
                get => _serialMonitor;
                set => SetProperty(ref _serialMonitor, value);
            }

            private StripLed.Model _stripLedModel = new StripLed.Model();
            public StripLed.Model StripLedModel
            {
                get => _stripLedModel;
                set => SetProperty(ref _stripLedModel, value);
            }

            public static Model DesignTime
            {
                get
                {
                    var model = new Model();
                    model.SerialMonitor = "setup\nloop\nFastLED.show";
                    model.StripLedModel = StripLed.Model.DesignTime;

                    return model;
                }
            }
        }
    }
}
