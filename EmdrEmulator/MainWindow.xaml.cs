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
        private readonly Timer _timer;
        private readonly Model model = new Model();

        public MainWindow()
        {
            InitializeComponent();
            DataContext = model;

            // obsługa Serial
            EmdrWrapper.EmdrSketch.Serial.writeEvent += Serial_WriteEventHandler;

            // obsługa FastLED
            unsafe { EmdrWrapper.EmdrSketch.FastLED.addLedsEvent += FastLED_AddLedsEventHandler; }
            unsafe { EmdrWrapper.EmdrSketch.FastLED.showEvent += FastLED_ShowEventHandler; }

            // obsluga Event
            unsafe { EmdrWrapper.EmdrControllerSketch.Event.sendEvent += Event_SendEventHandler; }

            // obsluga lcd
            unsafe { EmdrWrapper.EmdrSketch.lcd.printEvent += Lcd_PrintEventHandler; }
            unsafe { EmdrWrapper.EmdrSketch.lcd.setCursorEvent += Lcd_SetCursorEventHandler; }
            unsafe { EmdrWrapper.EmdrSketch.lcd.backlightEvent += Lcd_BacklightEventHandler; }
            unsafe { EmdrWrapper.EmdrSketch.lcd.clearEvent += Lcd_ClearEventHandler; }

            // uruchomienie funkcji setup()
            EmdrWrapper.EmdrSketch.setup();
            EmdrWrapper.EmdrControllerSketch.setup();

            // okresowe uruchamianie funcji loop()
            _timer = new Timer(LoopCallback, null, 0, 20);
        }

        private void Serial_WriteEventHandler(string text)
        {
            try
            {
                Dispatcher.Invoke(() =>
                {
                    model.SerialMonitor += text;
                    serialMonitorScrollViewer.ScrollToBottom();
                    Debug.Write(text);
                });
            }
            catch (TaskCanceledException) { }
        }

        private unsafe void FastLED_AddLedsEventHandler(int ledsCount)
        {
            try
            {
                Dispatcher.Invoke(() =>
                {
                    if (ledsCount > 1)
                    {
                        // strip
                        var stripModel = new Strip.Model();
                        for (int i = 0; i < ledsCount; i++)
                            stripModel.Leds.Add(new Strip.LedModel(i, new SolidColorBrush(Colors.Black)));
                        model.Strip = stripModel;
                    }
                    else
                    {
                        // remote controller
                    }
                });
            }
            catch (TaskCanceledException) { }
        }

        private unsafe void FastLED_ShowEventHandler(byte* ledsData, int ledsCount)
        {
            try
            {
                Dispatcher.Invoke(() =>
                {
                    // aktualizacja kolorow w strip
                    for (int i = 0; i < ledsCount; i++)
                    {
                        byte* rgb = ledsData + i * 3;
                        Color color = Color.FromRgb(rgb[0], rgb[1], rgb[2]);
                        model.Strip.Leds[i].Brush.Color = color;
                    }
                });
            }
            catch (TaskCanceledException) { }
        }

        private unsafe void Event_SendEventHandler(int _name)
        {
            try
            {
                Dispatcher.Invoke(() =>
                {
                    Name name = (Name)_name;
                    string text = $"Event.Name: {name}\n";
                    //model.SerialMonitor += text;
                    serialMonitorScrollViewer.ScrollToBottom();
                    Debug.Write(text);
                });
            }
            catch (TaskCanceledException) { }
        }

        private void Lcd_PrintEventHandler(string text)
        {
            try
            {
                Dispatcher.Invoke(() =>
                {
                    int row = model.Lcd.Row;
                    int col = model.Lcd.Col;
                    model.Lcd.Lines[row] = model.Lcd.Lines[row]
                        .PadRight(16)
                        .Remove(col, text.Length)
                        .Insert(col, text);
                    model.Lcd.Col += text.Length;
                    model.Lcd.Lines = new string[2]
                    {
                        model.Lcd.Lines[0],
                        model.Lcd.Lines[1]
                    };
                });
            }
            catch (TaskCanceledException) { }
        }

        private void Lcd_SetCursorEventHandler(byte col, byte row)
        {
            try
            {
                Dispatcher.Invoke(() =>
                {
                    model.Lcd.Col = col;
                    model.Lcd.Row = row;
                });
            }
            catch (TaskCanceledException) { }
        }

        private void Lcd_BacklightEventHandler(bool on)
        {
            try
            {
                Dispatcher.Invoke(() =>
                {
                    model.Lcd.Backlight = on;
                });
            }
            catch (TaskCanceledException) { }
        }

        private void Lcd_ClearEventHandler()
        {
            try
            {
                Dispatcher.Invoke(() =>
                {
                    model.Lcd.Lines = new string[2] { "", "" };
                    model.Lcd.Row = 0;
                    model.Lcd.Col = 0;
                });
            }
            catch (TaskCanceledException) { }
        }

        private static bool _loopCallbackWorking = false;
        private readonly TimerCallback LoopCallback = (object state) =>
        {
            if (_loopCallbackWorking)
                return;
            _loopCallbackWorking = true;
            EmdrWrapper.EmdrSketch.loop();
            EmdrWrapper.EmdrControllerSketch.loop();
            _loopCallbackWorking = false;
        };

        public class Model : ModelBase
        {
            private string _serialMonitor;
            public string SerialMonitor
            {
                get => _serialMonitor;
                set => SetProperty(ref _serialMonitor, value);
            }

            private Strip.Model _strip = new Strip.Model();
            public Strip.Model Strip
            {
                get => _strip;
                set => SetProperty(ref _strip, value);
            }

            private LCD.Model _lcd = new LCD.Model();
            public LCD.Model Lcd
            {
                get => _lcd;
                set => SetProperty(ref _lcd, value);
            }

            public static Model DesignTime
            {
                get
                {
                    var model = new Model
                    {
                        SerialMonitor = "setup\nloop\nFastLED.show",
                        Strip = EmdrEmulator.Strip.Model.EmdrDesignTime,
                        Lcd = LCD.Model.DesignTime
                    };

                    return model;
                }
            }
        }
    }
}
