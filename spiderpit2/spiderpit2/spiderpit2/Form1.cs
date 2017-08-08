using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
namespace spiderpit2
{
    public partial class Form1 : Form
    {
        Thread openGLrenderThread;
        csglwindow myglwindow; //= new csglwindow();
        public Form1()
        {
            InitializeComponent();
              myglwindow = new csglwindow();
              myglwindow.Parent = this;
              myglwindow.Dock = DockStyle.Fill;
            CheckForIllegalCrossThreadCalls = false;
            openGLrenderThread = new Thread(renderloop);
            openGLrenderThread.Start();
        }


        void renderloop()
        {
           
           // myglwindow.SuspendLayout();
           
            //myglwindow.ResumeLayout();
            while (true)
            {
                myglwindow.glDraw();
                myglwindow.Refresh();

            }

        }
    }
}