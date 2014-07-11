using System;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace SampleGrabber
{
    public class StringRedir : TextWriter
    {
        private TextWriter _orgWriter;
        // Redirecting Console output to RichtextBox
        private readonly RichTextBox _outBox;

        public StringRedir(RichTextBox textBox)
        {
            _orgWriter = Console.Out;
            _outBox = textBox;
        }

        public override void WriteLine(string x)
        {
            _outBox.AppendText(x);
        }

        public override Encoding Encoding
        {
            get { return Encoding.UTF8; }
        }

        protected override void Dispose(bool disposing)
        {
            Console.SetOut(_orgWriter);	// Redirect Console back to original TextWriter. 
            base.Dispose(disposing);
        }
    }
}