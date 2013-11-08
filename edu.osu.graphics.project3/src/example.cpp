#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include "Image_File_IO.h"

#define PI 3.14159265358979323846
#define PI_DIV_180 0.017453292519943296
#define deg PI_DIV_180

/*  Set initial size of display window.  */
GLsizei winWidth = 500, winHeight = 500;

/*  Set camera position and viewing size.  */
GLdouble trot = 30;
GLdouble prot = 30;
double r = 1.0;
GLfloat nRange = 3.0f;

/*  Set mouse tracking.  */
int mouseDown = 0;
int startX = 0;
int startY = 0;

/*  Set environment booleans.  */
int depthTesting = 1;
int orthoProj = 1;
int drawTeapot = 0;
int displayAxes = 1;
int displayAxesInColor = 1;

/*  Window size tracker.  */
GLint curWidth;
GLint curHeight;

void init()
{
    glClearColor (1.0, 1.0, 1.0, 0.0);     //  Set color of display window to white.
}

void drawAxes(GLfloat length)
{
    if (displayAxesInColor == 1)
    {
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3f (1.0, 0.0, 0.0);
        glVertex3f (0,0,0);
        glVertex3f (length,0,0);
        glColor3f (0.0, 1.0, 0.0);
        glVertex3f (0,0,0);
        glVertex3f (0,length,0);
        glColor3f (0.0, 0.0, 1.0);
        glVertex3f (0,0,0);
        glVertex3f (0,0,length);
        glEnd();

        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1,0x00FF);

        glBegin(GL_LINES);
        glColor3f (1.0, 0.0, 0.0);
        glVertex3f (0,0,0);
        glVertex3f (-length,0,0);
        glColor3f (0.0, 1.0, 0.0);
        glVertex3f (0,0,0);
        glVertex3f (0,-length,0);
        glColor3f (0.0, 0.0, 1.0);
        glVertex3f (0,0,0);
        glVertex3f (0,0,-length);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }
    else
    {
        glColor3f (0.0, 0.0, 0.0);
        glLineWidth(2);
        glBegin(GL_LINES);
        glVertex3f (0,0,0);
        glVertex3f (length,0,0);
        glVertex3f (0,0,0);
        glVertex3f (0,length,0);
        glVertex3f (0,0,0);
        glVertex3f (0,0,length);
        glEnd();

        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1,0x00FF);

        glBegin(GL_LINES);
        glVertex3f (0,0,0);
        glVertex3f (-length,0,0);
        glVertex3f (0,0,0);
        glVertex3f (0,-length,0);
        glVertex3f (0,0,0);
        glVertex3f (0,0,-length);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }
}

void displayFcn()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //  Clear display window.

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //  Set camera position and orientation.
    gluLookAt(r*cos(prot*deg)*cos(trot*deg),r*sin(prot*deg),r*cos(prot*deg)*sin(trot*deg), 0,0,0, 0,1,0);

    if (displayAxes == 1) drawAxes(2*nRange/3);    //  Draw the coordinate axes.

    //  Draw the teapot.
    if (drawTeapot == 1)
    {
        glColor3f (1.0, 0.0, 0.0);    //  set color to Red
        glutSolidTeapot(1);

        glLineWidth(1);
        glColor3f (0.0, 0.0, 0.0);    //  set color to Black
        glutWireTeapot(1);
    }

    glutSwapBuffers();  //  Display the image.
}

void setProjection(GLint newWidth, GLint newHeight)
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();

    if (orthoProj == 1)
    {
        if (newWidth <= newHeight)
            glOrtho(-nRange, nRange, -nRange*newHeight/newWidth, nRange*newHeight/newWidth, -nRange, nRange);
        else
            glOrtho(-nRange*newWidth/newHeight, nRange*newWidth/newHeight, -nRange, nRange, -nRange, nRange);
    }
    else
    {
        gluPerspective(60.0f, (float)newWidth/(float)newHeight, 0.1f, 100.0f);
    }

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
    if (newHeight == 0) newHeight = 1;

    glViewport (0, 0, newWidth, newHeight);
    setProjection(newWidth, newHeight);
    curWidth = newWidth;
    curHeight = newHeight;
}

void keyFcn(unsigned char key, int x, int y)
{
    if (orthoProj == 0)
    {
        switch (key)
        {
        case '+':
            r -= .1;
            if (r < 0.2) r =0.2;
            break;

        case '-':
            r += .1;
            if (r > 10) r =10;
            break;

        default:
            break;
        }
        glutPostRedisplay();
    }
}

void SpKeyFcn(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        prot -= 2;
        break;

    case GLUT_KEY_DOWN:
        prot += 2;
        break;

    case GLUT_KEY_LEFT:
        trot -= 2;
        break;

    case GLUT_KEY_RIGHT:
        trot += 2;
        break;

    case GLUT_KEY_F1:
        WriteTGA("testTGA.tga");
        break;

    case GLUT_KEY_F2:
        WriteBMP("testBMP.bmp");
        break;

    default:
        break;
    }

    if (prot < -90) prot = -90;
    if (prot > 90) prot = 90;
    if (trot > 360) trot -= 360;
    if (trot < 0) trot += 360;

    glutPostRedisplay();
}

void menu_select(int mode)
{
    switch (mode)
    {
    case 1:
        if (displayAxes == 1)
        {
            glutChangeToMenuEntry(1, "Turn Axes On", 1);
            displayAxes = 0;
        }
        else
        {
            glutChangeToMenuEntry(1, "Turn Axes Off", 1);
            displayAxes = 1;
        }
        break;

    case 2:
        if (depthTesting == 1)
        {
            glutChangeToMenuEntry(3, "Enable Depth Testing", 2);
            glDisable(GL_DEPTH_TEST);
            depthTesting = 0;
        }
        else
        {
            glutChangeToMenuEntry(3, "Disable Depth Testing", 2);
            glEnable(GL_DEPTH_TEST);
            depthTesting = 1;
        }
        break;

    case 3:
        if (orthoProj == 1)
        {
            glutChangeToMenuEntry(4, "Set to Orthogonal", 3);
            orthoProj = 0;
            r = 5;
            setProjection(curWidth, curHeight);
        }
        else
        {
            glutChangeToMenuEntry(4, "Set to Perspective", 3);
            orthoProj = 1;
            r = 1;
            setProjection(curWidth, curHeight);
        }
        break;

    case 4:
        if (drawTeapot == 1)
        {
            glutChangeToMenuEntry(5, "Show Teapot", 4);
            drawTeapot = 0;
        }
        else
        {
            glutChangeToMenuEntry(5, "Hide Teapot", 4);
            drawTeapot = 1;
        }
        break;

    case 5:
        displayAxesInColor = 1;
        break;

    case 6:
        displayAxesInColor = 0;
        break;

    case 7:
        trot = 30;
        prot = 30;
        break;

    case 1000:
        exit(0);
        break;

    default:
        break;
    }
    glutPostRedisplay();
}

void detectUpDown(int button, int state, int x, int y)
{
    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
    {
        mouseDown = 1;
        startX = x;
        startY = y;
    }
    else
        mouseDown = 0;
}

void detectMove(int x, int y)
{
    if (mouseDown == 1)
    {
        trot += (x-startX)/2;
        if (trot > 360) trot -= 360;
        if (trot < 0) trot += 360;
        startX = x;

        prot += (y-startY)/2;
        if (prot < -90) prot = -90;
        if (prot > 90) prot = 90;
        startY = y;

        glutPostRedisplay();
    }
}

void MakeMenu()
{
    int submenu = glutCreateMenu(menu_select);
    glutAddMenuEntry("Color", 5);
    glutAddMenuEntry("Black", 6);

    glutCreateMenu(menu_select);
    glutAddMenuEntry("Turn Axes Off", 1);
    glutAddSubMenu("Draw Axes in", submenu);
    glutAddMenuEntry("Disable Depth Testing", 2);
    glutAddMenuEntry("Set to Perspective", 3);
    glutAddMenuEntry("Show Teapot", 4);
    glutAddMenuEntry("Reset Position", 7);
    glutAddMenuEntry("Quit", 1000);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);                                      //  Initiate a GLUT window.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);   //  Set the diaplay modes.
    glutInitWindowPosition(50, 50);                             //  Set the position of the upper left corner of the window.
    glutInitWindowSize(winWidth, winHeight);                    //  Set the window size, in pixels.
    glutCreateWindow("GLUT Example Program #1 Complete");       //  Put a title in the window title bar.
    glEnable(GL_DEPTH_TEST);                                    //  Turn on depth testing.

    init();                                                     //  Do all of the things that need to be done only once.
    MakeMenu();                                                 //  Create the menu.
    glutDisplayFunc(displayFcn);                                //  Set callback for (re)display.
    glutReshapeFunc(winReshapeFcn);                             //  Set the callback for (re)sizing.
    glutSpecialFunc(SpKeyFcn);                                  //  Set the callback for special keys.
    glutKeyboardFunc(keyFcn);                                   //  Set the callback for regular keys.
    glutMouseFunc(detectUpDown);                                //  Set the callback for mouse clicks.
    glutMotionFunc(detectMove);                                 //  Set the callback for mouse motion.

    glutMainLoop();                                             //  Start the program's infinite event loop.
}
