//michael hester
//Includes the following homeworks:
//Subdivision circle, antialiased lines, triangle, clipping, transformations
//
//I wasn't in the last class so im not sure if i'm missing anything.

#include<stdlib.h>
#include<stdio.h>
#include<gl\glut.h>
#define Width 600
#define Height 600
#include<time.h>
#include<math.h>
#define lowclipx -500
#define hiclipx 500
#define lowclipy -200
#define hiclipy 200

#define radius 400
#define polysize 12
#define npoints 30
#define orbittime 10
#define gmul 20000
#define PI		3.14159265358979323846
#define segments 200
time_t previous;
time_t current ;
float m_time; //track the current time in seconds
int rez;
float depth = 0;


//used in the bouncing points.
struct point
{
       float x;
       float y;
       float dx;
       float dy;
       float ax; float ay;
       float cr; float cg; float cb;
       };
//used to symbolize a vertex for the subdivision circle and for transformations
struct vertex
{
       float x; float y; };
       
//a matrix
typedef float row[3];
typedef row matrix[3];
matrix xform;
matrix temp;

//set the xform matrix to the identity
void identity()
{
     xform[0][0] = 1;
     xform[1][1] = 1;
     xform[2][2] = 1;
     xform[0][1] = xform[0][2] = xform[1][0] = xform[1][2] = xform[2][0] = xform[2][1] = 0;
}
void tempidentity()
{
     temp[0][0] = 1;
     temp[1][1] = 1;
     temp[2][2] = 1;
     temp[0][1] = temp[0][2] = temp[1][0] = temp[1][2] = temp[2][0] = temp[2][1] = 0;
}
void postmultiply()  //xform * temp.  so i can stack transforms
{
     matrix hold;
     for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                    hold[i][j] = xform[i][j];
             
     //do the multiplication
     float sumspot;
     for(int i = 0; i < 3; i++)
             for(int j = 0; j < 3; j++)
             {       sumspot = 0;
                    for(int k = 0; k < 3; k++)
                            sumspot += hold[i][k] * temp[k][j];
                    xform[i][j] = sumspot;
             }
}
                    
                             

float setposition(float x, float y)
{
      temp[0][2] = x;
      temp[1][2] = y;
}
float setrotation(float theta) //in radians
{
      temp[0][0] = cos(theta);
      temp[1][1] = cos(theta);
      temp[0][1] = -sin(theta);
      temp[1][0] = sin(theta);   
}
float setscale(float sx, float sy)
{
      temp[0][0] *= sx;
      temp[1][1] *= sy;
}
//postmultiply matrix by vertex [x, y, 1] returned in vertex passed.
void applytransform(vertex & v)
{
     //3x3 matrix multiplcaiton
     vertex vtemp;  //x, y, 1
     float scale;
     vtemp.x = xform[0][0] * v.x + xform[0][1] * v.y + xform[0][2] * 1;
     vtemp.y = xform[1][0] * v.x + xform[1][1] * v.y + xform[1][2] * 1;
     scale = xform[2][0] * v.x + xform[2][1] * v.y + xform[2][2] * 1;
     v.x = vtemp.x * scale;
     v.y = vtemp.y * scale;
}
     
//distance between two vertices      
float distance(vertex a, vertex b)
{
      float xx = a.x -b.x;
      float yy = a.y - b.y;
      return sqrt(xx*xx+yy*yy);
}

//an edge construct used for the scanline triangle algorithim
struct edge
{
      float x, y, a, b, dx, cx; //dx is set to the change in x as y changes by 1.  cx is used later
       bool inarray;
       };
void sortedges(edge & e)
{
     float tempx, tempy;
     if(e.b < e.y)
     {tempy = e.y;
      e.y = e.b;
      e.b = tempy;
      tempx = e.x;
      e.x = e.a;
      e.a = tempx;
      
     }
}
     

struct triangle
{
       edge a;
       edge b;
       edge c;
};     




//this global array holds the points for the randomly generated points. 
point points[npoints];
//this trangle connects 3 of the above points.
triangle z;


//clip detection
bool clip(int cx, int x, int cy, int y, int a, int b)
{
     //if this point a b is within the clip range
     return ((cx <= a && a <= x) && (cy <= b && b <= y));
}
//transformed vertex functions look like the opengl functions
void glTransformedVertex2i(int x, int y) 
{
       vertex v; v.x = x; v.y = y;
       applytransform(v);
       glVertex2i((int)v.x, (int) v.y);
}
void glTransformedVertex2f(float x, float y) 
{
       vertex v; v.x = x; v.y = y;
       applytransform(v);
       glVertex2i((float)v.x, (float) v.y);
}     

//this doline function is the basis for both normal lines (the circle) and the triangle drawing functions
void doline(float x, float y, float tx, float ty)
{
     //rez is the screen resolution
     
     
    
     
     float dy = ty - y;
     float dx = tx - x;
    
     
     
     glBegin(GL_POINTS);
     
     for(int i =0; i < rez; i++)
     {
             //interpolate x and y along the vector
            if(clip(lowclipx, hiclipx, lowclipy, hiclipy, x + dx*(i/(rez * 1.0)), y+ dy*(i/(rez * 1.0))))
            { glTransformedVertex2f(x + dx*(i/(rez * 1.0)), y+ dy*(i/(rez * 1.0)));}
     }
     glEnd();
}
//this copy of the above draws an antialiased line by blending the vertices surrounding the line's position
void doaaline(float x, float y, float tx, float ty)
{
     //rez is the screen resolution
     
     float dy = ty - y;
     float dx = tx - x;
     int aarez = 3;
     float aaflare = 2; //increase to accelerate the falloff
    
     
     float ax; float ay;
      float xtx = tx - x;
      float yty = ty-y;
     float d = sqrt(xtx*xtx+yty*yty);
     for(int i =0; i < rez; i++)
     {
             //interpolate x and y along the vector
             ax =  x + dx*(i/(rez * 1.0));
             ay = y+ dy*(i/(rez * 1.0));
             
             for(int ai = int(ax - aarez); ai <= int(ax+aarez); ai++)
             {
                     for(int aj = int(ay -aarez); aj <= int(ay+aarez); aj++)
                     {
                           //calculate the color based on the distance from the point ax and ay
                           //
                           //P3(ai aj)
                           //p1 (x, y)
                           //p2 (tx, ty)
                          if(!clip(lowclipx, hiclipx, lowclipy, hiclipy, ai, aj)) continue;
                           
                           float u = ((ai - x)*(tx - x) + (aj -y)* (ty - y))/(d*d);
                           float zx = x + u *(tx - x);
                           float zy = y + u *(ty - y);
                           float aizx = ai-zx;
                           float ajzy = aj-zy;
                           
                           float intensity = sqrt(aizx*aizx + ajzy*ajzy);
                           
                           //distance derived from equations found at
                           //http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
                           
                           intensity = 1/intensity;
                           //intensity falls off with the square root, but it needs to be accelerated.
                           intensity /= aaflare;
                           //printf("%f\n", intensity);
                           //intensity += .9;
                           //intensity = (intensity < 0) ? 0 : intensity;
                           //intensity = (intensity > 1) ? 1 : intensity;
                           glColor3f(intensity, intensity, intensity);
                          glBegin(GL_POINTS); 
                          glTransformedVertex2i(ai, aj);
                          
                           glEnd();
                           //glColor3f(1,1,1);
                     }   
             }
            
     }
    
}

void swap(edge &a, edge &b) //swap edges a and b
{
     edge temp = a;
     a = b;
     b = temp;
}             

//boundary test, used in triangle drawing function
bool between(float bound1, float bound2, float t)
{
     if(bound1 <= t && t < bound2) return true;
     if(bound2 <= t && t < bound1) return true;
     return false;
}
//prints a string text at rasterposition x,y
void drawstring(float x, float y, const char * text)
{
      
      
      vertex textpos; textpos.x = 0; textpos.y = 0;
     applytransform(textpos);
     glRasterPos2f(textpos.x,textpos.y);
     for(int i = 0; text[i] != '\0'; i++)
             glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
}
      
//scanline triangle.. because i have a line function already which works in floating point instead of
//integer, this triangle function is slightly easier than the real scanline funciton, but it works similarly.
      
void dotriangle(triangle &t)
{
     
    //3
   sortedges(t.a);
    sortedges(t.b);
    sortedges(t.c);
    
   
    
    //calculate dx now.
    
    t.a.dx = (t.a.x-t.a.a)/(t.a.y - t.a.b);
    t.a.cx = t.a.x;
    
    
    t.b.dx = (t.b.x-t.b.a)/(t.b.y - t.b.b);
    t.b.cx = t.b.x;
    t.c.dx = (t.c.x-t.c.a)/(t.c.y - t.c.b);
    t.c.cx = t.c.x;
    
    
    int edge1 = -1; int edge2 = -1;
   
    float lowx; float highx;
    
    
    for(int y = -Height; y < Height; y++)
    {
            //check each edge to see if we can select it
            //we can select an edge if it's y value is <= the current, but its b value > than the current
            edge1 = -1;
            edge2 = -1;
            if(between(t.a.y, t.a.b, y))
            {
                     //we can always select this edge first if it is available.
                     //when i draw the lines with doline, the order of x and y don't matter. 
                     edge1 = 1;
            }
             if(between(t.b.y, t.b.b, y))
            {
                     //we can always select this edge first if it is available.
                     //when i draw the lines with doline, the order of x and y don't matter. 
                     if(edge1 == -1) edge1 = 2;
                     else edge2 = 2;
                     
            }
             if(between(t.c.y, t.c.b, y))
            {
                     //we can always select this edge first if it is available.
                     //when i draw the lines with doline, the order of x and y don't matter. 
                     if(edge1 == -1) edge1 = 3;
                     else edge2 = 3;
                     //c will almost always be edge 2.
            }
            
            if(edge1 == 1 && edge2 == 2)
            {
                     //set negative x and positive x
                     //glColor3f(1,0,0);
                     lowx = (t.a.cx);
                     highx = (t.b.cx);
                     t.a.cx+=t.a.dx;
                     t.b.cx+=t.b.dx;
                     
                     doline(lowx, y, highx, y);
            }
            if(edge1 == 2 && edge2 == 3)
            {
                    //  glColor3f(0,1,0);
                     //set negative x and positive x
                     lowx = (t.b.cx);
                     highx = (t.c.cx);
                     t.b.cx+=t.b.dx;
                     t.c.cx+=t.c.dx;
                     doline(lowx, y, highx, y);
            }
            if(edge1 == 1 && edge2 == 3)
            {
                     //set negative x and positive x
                    //  glColor3f(0,0,1);
                     lowx = (t.a.cx);
                     highx = (t.c.cx);
                     t.c.cx+=t.c.dx;
                     t.a.cx+=t.a.dx;
                     doline(lowx, y, highx, y);
            }
            //glColor3f(1,1,1);
    }//for y
}//void dotriangle
                          
//draw a triangle with vertices a, b, c subdivided along edge BC by depth
//to get the circle, draw using vertices 0,0 1,0 0, 1 -1, 0 0, -1 with 0,0 as the common point
void subdividetriangle(vertex a, vertex b, vertex c, int depth)
//if depth is zero then draw the triangle, else subdivide the edge between point b and c, normalize it
//and draw two triangles
{
      triangle t;
      if (depth == 0)
      {
              
              t.a.x = a.x;
              t.a.y = a.y;
              t.a.a = b.x;
              t.a.b = b.y;
              t.b.x = b.x;
              t.b.y = b.y;
              t.b.a = c.x;
              t.b.b = c.y;
              t.c.x = c.x;
              t.c.y = c.y;
              t.c.a = a.x;
              t.c.b = a.y;
              glColor3f(float(rand()/65535.0), float(rand()/65535.0), float(rand()/65535.0));
              dotriangle(t);
              return;
      }  
      //else subdivide b and c
      vertex d;
      float r = distance(a, b);
       //use this distance to normalize
      d.x = (b.x + c.x) /2;
      d.y = (b.y + c.y) /2;
      float normald = distance(a, d);
      d.x = (d.x / normald) * r;
      d.y = (d.y / normald) * r;
      //draw two triangles
      subdividetriangle(a, b, d, depth -1);
      subdividetriangle(a, c, d, depth -1);
} 

//the main display driver
void display()
{
     current = clock();
     double dt = (double)difftime(current, previous);
     dt /= CLOCKS_PER_SEC; //this gives the time difference between frames
     m_time += dt;
     //increment a set time to use throughout
     
     glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
     glColor3f(1,1,1);
     //reset the transform matrix and use it
     identity();
     
     //draw the border for clipping
     doline(lowclipx, lowclipy, lowclipx, hiclipy);
     doline(lowclipx, hiclipy, hiclipx, hiclipy);
     doline(hiclipx, hiclipy, hiclipx, lowclipy);
     doline(hiclipx, lowclipy, lowclipx, lowclipy);
     
     //draw the subdivided circle
     vertex vo; vertex va; vertex vb; vertex vc; vertex vd;
     vo.x = 0; vo.y = 0;
     va.x = 0; va.y = 300;
     vb.x = 300; vb.y = 0;
     vc.x = -300; vc.y = 0;
     vd.x = 0; vd.y = -300;
     
     
    
   //draw circle
   glColor3f(1, 1, 1);
   
  
   
   
   //draw the text "clipping and transform"
   glColor3f(1,1,1);
   drawstring(0,0,"clipping and transform");
  
   //point (cos(t), sin(t)) will plot a point at angle t, starting at 1,0 going
   // clockwise as t increases.
   //the point's distance to the origin of the angle is 1.  
   //TO draw a circle, I need only draw a line connecting
   //all the angles from 0 to 2*PI in radians.  for the 
   //purposes of this program, I only need to have
   //the circle be of high enough steps (segments below) that it looks 
   //like a circle and not a polygon.  This is solely determined
   //by the size of the circle drawn.  I have to zoom in on an edge to 
   //tell its not a "circle"
   for(int i = 0; i < segments; i++)
   {
         float x = radius*cos(PI * 2 *float(i)/segments);
         float y = radius*sin(PI * 2 * float(i)/segments);
         float x2 = radius*cos(PI * 2 *float(i+1)/segments);
         float y2 = radius*sin(PI * 2 * float(i+1)/segments);
         doline(x, y, x2, y2);
         
   }  
   
   glColor3f(0, 1 ,0);
   //polygon is green like earth
   //the earth was deleted from the program at when the scanline triangle function was created, the
   //random points still gravitate towards it like before, but to the position where the "earth" would have been
   
   
   
   if (m_time > orbittime) m_time = 0;
   depth += dt;
   if(depth > 4.2) depth = 0;
   previous = current;
       
   float x = radius*cos(PI * 2 *(m_time/orbittime ));
   float y = radius*sin(PI * 2 * (m_time/orbittime ));
   float px = x + cos(PI * 2 *(m_time/.5 * segments)/segments);
   float py = y + sin(PI * 2 * (m_time/.5 * segments)/segments);
   
   glColor3f(1,1,1);
   glPointSize(1.0);
   //this is the original test for the line algorithim.
   doline(-200, 200, 200, 200);
   //draw the "pentagon", which in this case can be any polygon, set by the polysize definition above
   for(int ft = 0; ft < polysize; ft++)
   {
           float asx = radius*cos(PI * 2 *(m_time/orbittime ) + ((float)ft / polysize)*(2*PI));
           float asy = radius*sin(PI * 2 * (m_time/orbittime ) +((float)ft / polysize)*(2*PI));
           float aex = radius*cos(PI * 2 *(m_time/orbittime ) + ((float)(ft+1) / polysize)*(2*PI));
           float aey = radius*sin(PI * 2 * (m_time/orbittime ) +((float)(ft+1) / polysize)*(2*PI));
           
           glColor3f(1,1,1);
           //uses antialiased lines
           doaaline(asx, asy, aex, aey);
           glPointSize(5.0);
           glBegin(GL_POINTS);
           if(clip(lowclipx, hiclipx, lowclipy, hiclipy, asx, asy)){
           glTransformedVertex2f(asx, asy); }
           glColor3f(1,1,1);
           glEnd();
           glPointSize(1.0);
   }
   glPointSize(3.0);
   
   //points gravitate toward the location where earth would have been.
   //the gravity vector is scaled by the square of the distance.
   //(x - px) / distance ^2, and the delta is altered by this
   
   
   //***** never set npoints less than 3 else there will be no triangle, and the program would crash
   //due to a segmentation fault.
    
   //draw a robot arm with a clock on it.
   vertex arm1, arm2, arm3, arm4;
   arm1.x = 0; arm2.x = 0; arm1.y = 30; arm2.y = -30;
   arm3.x = 150; arm4.x = 150; arm3.y = 30; arm4.y = -30;
   //draw the first arm piece.
   tempidentity();
   identity();
   //2pi in 10 seconds
   setrotation((m_time/10)*2*PI);
   postmultiply();
   subdividetriangle(arm1, arm2, arm3, 0);
   subdividetriangle(arm2,arm3, arm4, 0);
   //draw next segment
   tempidentity();
   setposition(150,0);
   setrotation((m_time/10)*3*PI);
   setscale(.80, .80);
   postmultiply();
   subdividetriangle(arm1, arm2, arm3, 0);
   subdividetriangle(arm2,arm3, arm4, 0);
    tempidentity();
   setposition(150,0);
   setrotation((m_time/10)*3*PI);
   setscale(.50, .50);
   postmultiply();
   
   subdividetriangle(arm1, arm2, arm3, 0);
   subdividetriangle(arm2,arm3, arm4, 0);
   glColor3f(1,1,1);
   identity();
   glFlush();  
   glutSwapBuffers();   //double buffered... we're always drawing on the back buffer.
}
void initGL(GLvoid)
{
    //there really is nothing to do here except initialize the time tracker.
    m_time = 0;
    
    //build the array of points
    for(int i = 0; i < npoints; i++)
    {
            float r = float(rand())/65535 * radius;
            float t = float(rand())/65535 * 2*PI;
            points[i].x = r*cos(t);
            points[i].y = r*sin(t);
             float dr = float(rand())/65535 * 0;
            float dt = float(rand())/65535 * 2*PI;
            points[i].dx = dr*cos(dt);
            points[i].dy = dr*sin(dt);
            points[i].cr = float(rand())/65535.0;
            points[i].cg = float(rand())/65535.0;
            points[i].cb = float(rand())/65535.0;
            z.a.x = points[0].x;
 
    }    
    //-200 -200, 140, -180, 0, 120
    
   
}

static void reshape(int w, int h) 
{	
    printf("w %d h %d  w>h %d w-h/2 %d\n", w, h, w>h, (w-h)/2);

     if (h==0)
       {
                     h = 1; 
       }
       glViewport(0,0,w,h);
       glMatrixMode(GL_PROJECTION); //set to projection matrix
       glLoadIdentity();
       //i want the window to be square regardless of the size of the window...
       //but i cant figure that out so....
       //the window is defualt to square
       //changing the window off of a square shape will effect the scanline algorithims.
             
        gluOrtho2D (-h, h, -w, w);  
        //the window is 600x600, but that makes my circle too small
        
        rez = h+w;
       
       glMatrixMode(GL_MODELVIEW); //reset to the model view  
    
     
}

int main(int argc, char **argv)
{	//f. initialize a drawing area
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Michael Hester: Clipping, antialiasing, subdivision circle, transformations");
	initGL();
	previous = 0;

    //g. specify event callback functions
    glutReshapeFunc(reshape); 
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMainLoop();
    return 0;
}
