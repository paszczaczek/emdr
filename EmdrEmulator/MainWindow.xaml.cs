﻿using System;
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

            // obsługa Serial.write()
            EmdrWrapper.Sketch.serialWriteEvent += SerialWriteEventHandler;

            // obsługa FastLED.show()
            unsafe { EmdrWrapper.Sketch.fastLEDShowEvent += FastLEDShowEventHandler; }

            // uruchomienie funkcji setup()
            EmdrWrapper.Sketch.setup();

            // okresowe uruchamianie funcji loop()
            _timer = new Timer(LoopCallback, null, 0, 20);
        }

        private void SerialWriteEventHandler(string text)
        {
            Dispatcher.Invoke(() =>
            {
                model.SerialMonitor += $"{text}\n";
                serialMonitorScrollViewer.ScrollToBottom();
            });
        }

        private unsafe void FastLEDShowEventHandler(byte* ledsData, int ledsCount)
        {
            try
            {
                Dispatcher.Invoke(() =>
                {
                    if (ledsCount == 1)
                        model.RemoteControlStripModel 
                        = UpdateModel(ledsData, ledsCount, model.RemoteControlStripModel);
                    else
                        model.EmdrStripModel
                            = UpdateModel(ledsData, ledsCount, model.EmdrStripModel);
                });
            }
            catch (TaskCanceledException) { }
        }

        private unsafe Strip.Model UpdateModel(byte* ledsData, int ledsCount, Strip.Model model)
        {
            if (model.Leds.Count != ledsCount)
            {
                // nowy strip
                var stripModel = new Strip.Model();
                for (int i = 0; i < ledsCount; i++)
                {
                    byte* rgb = ledsData + i * 3;
                    Color color = Color.FromRgb(rgb[0], rgb[1], rgb[2]);
                    stripModel.Leds.Add(new Strip.LedModel(i, new SolidColorBrush(color)));
                }
                return stripModel;
            }
            else
            {
                // aktualizacja kolorow w strip
                for (int i = 0; i < ledsCount; i++)
                {
                    byte* rgb = ledsData + i * 3;
                    Color color = Color.FromRgb(rgb[0], rgb[1], rgb[2]);
                    model.Leds[i].Brush.Color = color;
                }
                return model;
            }

        }

        private static bool _loopCallbackWorking = false;
        private TimerCallback LoopCallback = (object state) =>
        {
            if (_loopCallbackWorking)
                return;
            _loopCallbackWorking = true;
            EmdrWrapper.Sketch.loop();
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

            private Strip.Model _emdrStripModel = new Strip.Model();
            public Strip.Model EmdrStripModel
            {
                get => _emdrStripModel;
                set => SetProperty(ref _emdrStripModel, value);
            }

            private Strip.Model _remoteControlStripModel = new Strip.Model();
            public Strip.Model RemoteControlStripModel
            {
                get => _remoteControlStripModel;
                set => SetProperty(ref _remoteControlStripModel, value);
            }

            public static Model DesignTime
            {
                get
                {
                    var model = new Model();
                    model.SerialMonitor = "setup\nloop\nFastLED.show";
                    model.EmdrStripModel = Strip.Model.EmdrDesignTime;
                    model.RemoteControlStripModel = Strip.Model.RemoteControlDesignTime;

                    return model;
                }
            }
        }
    }
}
