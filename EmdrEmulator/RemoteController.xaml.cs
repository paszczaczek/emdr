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
    /// Interaction logic for RemoteControll.xaml
    /// </summary>
    public partial class RemoteController : UserControl
    {
        public RemoteController()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // w tag jest enum Name
            string tag = (string)((Button)sender).Tag;
            if (!Enum.TryParse<Name>(tag, out Name eventName))
                eventName = global::Name.UnknowCode;

            // zamieniamy EventName na IRCode
            int IRCode = EmdrWrapper.EmdrControllerSketch.RemoteController
                .EventNameToIRCode((int)eventName);
            EmdrWrapper.EmdrControllerSketch.irrecv.DecodeReturnValue(IRCode);
        }
    }
}
