using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using CsGL.Util;
using CsGL;
using CsGL.OpenGL;
using System.Runtime.InteropServices;
namespace spiderpit2
{
    
    public partial class csglwindow : CsGL.OpenGL.OpenGLControl
    {
        [DllImport("openglDLLtest.dll")]
        public static extern int fnopenglDLLtest();
        public csglwindow()
        {
            InitializeComponent();
        }
        public override void glDraw()
        {
            
            fnopenglDLLtest();


            base.glDraw();
        }
       
    }
}
