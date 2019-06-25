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
        private MyDataContext myDataContext = new MyDataContext();

        public MainWindow()
        {
            InitializeComponent();
            DataContext = myDataContext;

            // logowanie Serial.write()
            EmdrWrapper.Sketch.serialWriteEvent +=
                (text) => Dispatcher.Invoke(() =>
                {
                    myDataContext.SerialMonitor += $"{text}\n";
                    serialMonitorScrollViewer.ScrollToBottom();
                });

            // uruchomienie funkcji setup()
            EmdrWrapper.Sketch.setup();

            // okresowe uruchamianie funcji loop()
            _timer = new Timer(state => EmdrWrapper.Sketch.loop(), null, 0, 100);
        }
    }

    public class MyDataContext : DataContextBase
    {
        private string _serialMonitor;
        public string SerialMonitor {
            get => _serialMonitor;
            set => SetProperty(ref _serialMonitor, value);
        }

        public static MyDataContext DesignTime
        {
            get
            {
                var dataContext = new MyDataContext();
                dataContext.SerialMonitor = "setup\nloop\n";

                return dataContext;
            }
        }
    }
}
