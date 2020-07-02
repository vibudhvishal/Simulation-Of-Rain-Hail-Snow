#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>


#define MAX_PARTICLES 1000
#define WCX        640
#define WCY        480
#define RAIN    0
#define SNOW    1
#define    HAIL    2


float slowdown = 2.0;
float velocity = 0.0;
float zoom = -40.0;
float pan = 0.0;
float tilt = 0.0;
float hailsize = 0.1;

int loop;
int fall;

//floor colors
float r = 0.0;
float g = 0.0;
float b = 0.0;
float ground_points[21][21][3];
float ground_colors[21][21][4];
float accum = -10.0;

typedef struct {
  // Life
  bool alive;    // is the particle alive?
  float life;    // particle lifespan
  float fade; // decay
  // color
  float red;
  float green;
  float blue;
  // Position/direction
  float xpos;
  float ypos;
  float zpos;
  // Velocity/Direction, only goes down in y dir
  float vel;
  // Gravity
  float gravity;
}particles;

// Paticle System
particles par_sys[MAX_PARTICLES];
void renderbitmap(float x,float y,void *font,char *str)
{
    char *c;
    glRasterPos2f(x,y);
    for(c=str;*c!='\0';c++)
    {
        glutBitmapCharacter(font,*c);
    }
}
void display1()
{
   // glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    char buf1[100]={0},buf2[100]={0},buf3[100]={0},buf4[100]={0},buf5[100]={0},buf6[100]={0},buf7[100]={0},buf8[100]={0},buf9[100]={0},buf10[100]={0},buf11[100]={0};
    sprintf(buf1,"Simulation of Rain Hail Snow");
    sprintf(buf2,"SUBMITTED BY");
    sprintf(buf3,"VIBUDH VISHAL                    1PE16CS175");
    sprintf(buf4,"SIDDHANTH G PAI           1PE16CS153");
    sprintf(buf5,"UNDER THE GUIDANCE OF");
    sprintf(buf6,"Prof Evlin Vidyu Latha");
    sprintf(buf7,"Prof Dept of CSE ");
    sprintf(buf8,"PESIT,BANGALORE ");
    sprintf(buf9,"Prof  Nivedita ");
    sprintf(buf10,"Prof Dept of CSE ");
    sprintf(buf11,"PESIT,BANGALORE ");
    glColor3f(1.0,0.0,1.0);
    renderbitmap(-20,90,GLUT_BITMAP_TIMES_ROMAN_24,buf1);
    glColor3f(1.0,1.0,1.0);
    renderbitmap(-10,60,GLUT_BITMAP_TIMES_ROMAN_24,buf2);
    glColor3f(0.0,1.0,1.0);
    renderbitmap(-20,53,GLUT_BITMAP_HELVETICA_18,buf3);
    glColor3f(0.0,1.0,1.0);
    renderbitmap(-20,48,GLUT_BITMAP_HELVETICA_18,buf4);
    glColor3f(1.0,1.0,1.0);
    renderbitmap(-19,30,GLUT_BITMAP_TIMES_ROMAN_24,buf5);
    glColor3f(0.0,1.0,1.0);
    renderbitmap(-50,23,GLUT_BITMAP_HELVETICA_18,buf6);
    glColor3f(0.0,1.0,1.0);
    renderbitmap(-50,19,GLUT_BITMAP_HELVETICA_12,buf7);
    glColor3f(0.0,1.0,1.0);
    renderbitmap(-50,15,GLUT_BITMAP_HELVETICA_12,buf8);
    glColor3f(0.0,1.0,1.0);
    renderbitmap(30,23,GLUT_BITMAP_HELVETICA_18,buf9);
    glColor3f(0.0,1.0,1.0);
    renderbitmap(30,19,GLUT_BITMAP_HELVETICA_12,buf10);
    glColor3f(0.0,1.0,1.0);
    renderbitmap(30,15,GLUT_BITMAP_HELVETICA_12,buf11);
    glFlush();
    glutPostRedisplay();
}

void normal_keys(unsigned char key, int x, int y) {
  if (key == 'r') { // Rain
    fall = RAIN;
    glutPostRedisplay();
  }
  if (key == 'h') { // Hail
    fall = HAIL;
    glutPostRedisplay();
  }
  if (key == 's') { // Snow
    fall = SNOW;
    glutPostRedisplay();
  }
  if (key == '=') { //really '+' - make hail bigger
    hailsize += 0.01;
  }
  if (key == '-') { // make hail smaller
    if (hailsize > 0.1) hailsize -= 0.01;
  }
  if (key == ',') { // really '<' slow down
    if (slowdown > 4.0) slowdown += 0.01;
  }
  if (key == '.') { // really '>' speed up
    if (slowdown > 1.0) slowdown -= 0.01;
  }
  if (key == 'q') { // QUIT
    exit(0);
  }
}

void special_keys(int key, int x, int y) {
  if (key == GLUT_KEY_UP) {
    zoom += 1.0;
  }
  if (key == GLUT_KEY_DOWN) {
    zoom -= 1.0;
  }
  if (key == GLUT_KEY_RIGHT) {
    pan += 1.0;
  }
  if (key == GLUT_KEY_LEFT) {
    pan -= 1.0;
  }
  if (key == GLUT_KEY_PAGE_UP) {
    tilt -= 1.0;
  }
  if (key == GLUT_KEY_PAGE_DOWN) {
    tilt += 1.0;
  }
}


// Initialize/Reset Particles - give them their attributes
void initParticles(int i) {
    par_sys[i].alive = true;
    par_sys[i].life = 1.0;
    par_sys[i].fade = float(rand()%100)/1000.0f+0.003f;

    par_sys[i].xpos = (float) (rand() % 21) - 10;
    par_sys[i].ypos = 10.0;
    par_sys[i].zpos = (float) (rand() % 21) - 10;

    par_sys[i].red = 0.5;
    par_sys[i].green = 0.5;
    par_sys[i].blue = 1.0;

    par_sys[i].vel = velocity;
    par_sys[i].gravity = -3.8;//-0.8;

}

void init( ) {
  int x, z;

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

  // Ground Verticies
    // Ground Colors
    for (z = 0; z < 21; z++) {
      for (x = 0; x < 21; x++) {
        ground_points[x][z][0] = x - 10.0;
        ground_points[x][z][1] = accum;
        ground_points[x][z][2] = z - 10.0;

        ground_colors[z][x][0] = r; // red value
        ground_colors[z][x][1] = g; // green value
        ground_colors[z][x][2] = b; // blue value
        ground_colors[z][x][3] = 0.0; // acummulation factor
      }
    }

    // Initialize particles
    for (loop = 0; loop < MAX_PARTICLES; loop++) {
        initParticles(loop);
    }
}

// For Rain
void drawRain() {
  float x, y, z;
  for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
    if (par_sys[loop].alive == true) {
      x = par_sys[loop].xpos;
      y = par_sys[loop].ypos;
      z = par_sys[loop].zpos + zoom;

      // Draw particles
      glColor3f(0.5, 0.5, 1.0);
      glBegin(GL_LINES);
        glVertex3f(x, y, z);
        glVertex3f(x, y+0.5, z);
      glEnd();

      // Update values
      //Move
      // Adjust slowdown for speed!
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*100);
      par_sys[loop].vel += par_sys[loop].gravity;
      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      if (par_sys[loop].ypos <= -10) {
        par_sys[loop].life = -1.0;
      }
      //Revive
      if (par_sys[loop].life < 0.0) {
        initParticles(loop);
      }
    }
  }
}

// For Hail
void drawHail() {
  float x, y, z;

  for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
    if (par_sys[loop].alive == true) {
      x = par_sys[loop].xpos;
      y = par_sys[loop].ypos;
      z = par_sys[loop].zpos + zoom;

      // Draw particles
      glColor3f(0.8, 0.8, 0.9);
      glBegin(GL_QUADS);
        // Front
        glVertex3f(x-hailsize, y-hailsize, z+hailsize); // lower left
        glVertex3f(x-hailsize, y+hailsize, z+hailsize); // upper left
        glVertex3f(x+hailsize, y+hailsize, z+hailsize); // upper right
        glVertex3f(x+hailsize, y-hailsize, z+hailsize); // lower left
        //Left
        glVertex3f(x-hailsize, y-hailsize, z+hailsize);
        glVertex3f(x-hailsize, y-hailsize, z-hailsize);
        glVertex3f(x-hailsize, y+hailsize, z-hailsize);
        glVertex3f(x-hailsize, y+hailsize, z+hailsize);
        // Back
        glVertex3f(x-hailsize, y-hailsize, z-hailsize);
        glVertex3f(x-hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z-hailsize);
        //Right
        glVertex3f(x+hailsize, y+hailsize, z+hailsize);
        glVertex3f(x+hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z+hailsize);
        //Top
        glVertex3f(x-hailsize, y+hailsize, z+hailsize);
        glVertex3f(x-hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y+hailsize, z-hailsize);
        glVertex3f(x+hailsize, y+hailsize, z+hailsize);
        //Bottom
        glVertex3f(x-hailsize, y-hailsize, z+hailsize);
        glVertex3f(x-hailsize, y-hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z-hailsize);
        glVertex3f(x+hailsize, y-hailsize, z+hailsize);
      glEnd();

      // Update values
      //Move up
      if (par_sys[loop].ypos <= -10) {
        par_sys[loop].vel = par_sys[loop].vel * -1.0;
      }
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000); // * 1000
      par_sys[loop].vel += par_sys[loop].gravity;

      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      //Revive
      if (par_sys[loop].life < 0.0) {
        initParticles(loop);
      }
    }
  }
}

// For Snow
void drawSnow() {
  float x, y, z;
  for (loop = 0; loop < MAX_PARTICLES; loop=loop+2) {
    if (par_sys[loop].alive == true) {
      x = par_sys[loop].xpos;
      y = par_sys[loop].ypos;
      z = par_sys[loop].zpos + zoom;

      // Draw particles
      glColor3f(1.0, 1.0, 1.0);
      glPushMatrix();
      glTranslatef(x, y, z);
      glutSolidSphere(0.2, 16, 16);
      glPopMatrix();

      // Update values
      //Move
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*1000);
      par_sys[loop].vel += par_sys[loop].gravity;
      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      if (par_sys[loop].ypos <= -10) {
        int zi = z - zoom + 10;
        int xi = x + 10;
        ground_colors[zi][xi][0] = 0.7;
        ground_colors[zi][xi][2] = 0.8;
        ground_colors[zi][xi][3] = 0.6;
        if (ground_colors[zi][xi][3] > 0.5) {
         ground_points[xi][zi][3] += 0.5;
        }
        par_sys[loop].life = -1.0;
      }

      //Revive
      if (par_sys[loop].life < 0.0) {
        initParticles(loop);
      }
    }
  }
}

// Draw Particles
void drawScene( ) {
  int i, j;
  float x, y, z;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();


  glRotatef(pan, 0.0, 1.0, 0.0);
  glRotatef(tilt, 1.0, 0.0, 0.0);

  // GROUND?!
  glColor3f(r, g, b);
  glBegin(GL_QUADS);
    // along z - y const
    for (i = -10; i+1 < 11; i++) {
      // along x - y const
     for (j = -10; j+1 < 11; j++) {
        glColor3fv(ground_colors[i+10][j+10]);
        glVertex3f(ground_points[j+10][i+10][0],
              ground_points[j+10][i+10][1],
              ground_points[j+10][i+10][2] + zoom);
        glColor3f(1.0,0.0,0.0);
        glVertex3f(ground_points[j+1+10][i+10][0],
              ground_points[j+1+10][i+10][1],
              ground_points[j+1+10][i+10][2] + zoom);
        glColor3f(1.0,0.0,0.0);
        glVertex3f(ground_points[j+1+10][i+1+10][0],
              ground_points[j+1+10][i+1+10][1],
              ground_points[j+1+10][i+1+10][2] + zoom);
        glColor3fv(ground_colors[i+1+10][j+10]);
        glVertex3f(ground_points[j+10][i+1+10][0],
              ground_points[j+10][i+1+10][1],
              ground_points[j+10][i+1+10][2] + zoom);
      }

    }
  glEnd();
  // Which Particles
  if (fall == RAIN) {
    drawRain();
  }else if (fall == HAIL) {
    drawHail();
  }else if (fall == SNOW) {
    drawSnow();
  }

  glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45, (float) w / (float) h, .1, 200);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void idle ( ) {
  glutPostRedisplay();
}
void myinit()
{
    glClearColor(0.0,0.0,0.0,0.0);
    glMatrixMode(GLUT_SINGLE|GLUT_RGB);
    glLoadIdentity();
    glOrtho(-90.0,100.0,-15.0,100.0,0.0,1.0);
}
int main (int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(WCX, WCY);
  glutCreateWindow("Simulation of Rain Hail and Snow");
  init();
  glutDisplayFunc(drawScene);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(normal_keys);
  glutSpecialFunc(special_keys);
  glutIdleFunc(idle);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(1000,800);
  glutCreateWindow("frontpage");
  myinit();
  glutDisplayFunc(display1);
  glutMainLoop();
  return 0;
}
