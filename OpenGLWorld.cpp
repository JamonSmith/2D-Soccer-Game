#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

using namespace std;

const double pi = 2.0 * acos(0.0);

int colorIndex = 0;

void buffer_size_callback(GLFWwindow* win, int width, int height)
{
    if (height <= 0)
    {
        height = 1;
    }

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        colorIndex = 1;
    }
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        colorIndex = 2;
    }
    else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        colorIndex = 3;
    }
    else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        colorIndex = 4;
    }
}

void drawCircle(double centerx, double centery, double r, int seg)
{
    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(centerx, centery);
    for (int i = 0; i <= seg; i++)
    {
        double ang = 2.0f * i * pi / seg;
        double x = centerx + r * cos(ang);
        double y = centery + r * sin(ang);
        glVertex2f(x, y);
    }

    glEnd();
}

double clamp(double val, double minVal, double maxVal)
{
    if (val <= minVal)
    {
        return minVal;
    }

    if (val >= maxVal)
    {
        return maxVal;
    }

    return val;
}

int main()
{
    // Initialize GLFW
    if (!glfwInit()) 
    {
        cout << "Failed to init GLFW\n";
        return -1;
    }

    // Tell OpenGL what version we want
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    int winW = 800;
    int winH = 600;

    // Create a window
    GLFWwindow* window = glfwCreateWindow(winW, winH, "OpenGL Test", nullptr, nullptr);
    if (!window) 
    {
        cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    glfwSetFramebufferSizeCallback(window, buffer_size_callback);

    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);
    buffer_size_callback(window, width, height);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) 
    {
        cout << "Failed to init GLEW\n";
        return -1;
    }

    // Print OpenGL version
    cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
    cout << "______________________________________________________" << endl;
    cout << endl;

    // Square starting coordinates and movement speed
    double posx = -0.5;
    double posy = 0.0;
    double speed = 0.0005;

    // goal 1 coordinates
    double goal1x = -1.0;
    double goal1y = 0.0;

    // goal 2 coordinates
    double goal2x = 1.0;
    double goal2y = 0.0;

    // ball starting coordinates
    double ballx = 0.0;
    double bally = 0.0;

    // ball velocity (vertical and horizontal)
    double ballvx = 0.0;
    double ballvy = 0.0;

    // game characteristics
    bool goalScored = false;
    int yellowScore = 0;
    int blueScore = 0;

    // Main render loop
    while (!glfwWindowShouldClose(window)) 
    {
        // close the game window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        
        // scene background color
        glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // d pad movement
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            posx -= speed;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            posx += speed;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            posy -= speed;
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            posy += speed;
        }

        // bounds checking for the square
        if (posx >= 0.95)
        {
            posx = 0.95;
        }
        
        if (posx <= -0.95)
        {
            posx = -0.95;
        }
        
        if (posy >= 0.95)
        {
            posy = 0.95;
        }
        
        if (posy <= -0.95)
        {
            posy = -0.95;
        }

        // bounds checking for the ball
        if (ballx >= 0.9)
        {
            ballx = 0.9;
            ballvx *= -1;
        }

        if (ballx <= -0.9)
        {
            ballx = -0.9;
            ballvx *= -1;
        }

        if (bally >= 0.9)
        {
            bally = 0.9;
            ballvy *= -1;
        }

        if (bally <= -0.9)
        {
            bally = -0.9;
            ballvy *= -1;
        }

        // friction to slow the ball
        ballvx *= 0.999;
        ballvy *= 0.999;

        // kill ball velocity after certain speed
        if (abs(ballvx) < 0.00001)
        {
            ballvx = 0.0;
        }

        if (abs(ballvy) < 0.00001)
        {
            ballvy = 0.0;
        }

        // detect collision between ball and square
        double left = posx - 0.1;
        double right = posx + 0.1;
        double bottom = posy - 0.1;
        double top = posy + 0.1;

        double closestX = clamp(ballx, left, right);
        double closestY = clamp(bally, bottom, top);

        double dx = ballx - closestX;
        double dy = bally - closestY;


        if (pow(dx, 2.0) + pow(dy, 2.0) <= pow(0.1, 2.0))
        {
           double len = sqrt(pow(dx, 2.0) + pow(dy, 2.0));

            if (len != 0)
            {
                double nx = dx / len;
                double ny = dy / len;

                double hit = 0.001;

                ballvx += hit * nx;
                ballvy += hit * ny;
            }
        }

        ballx += ballvx;
        bally += ballvy;

        // detect collsion between goal 1 and ball
        double goal1Left = goal1x - 0.001;
        double goal1Right = goal1x + 0.001;
        double goal1Bottom =goal1y - 0.33;
        double goal1Top = goal1y + 0.33;

        double closestGoal1X = clamp(ballx, goal1Left, goal1Right);
        double closestGoal1Y = clamp(bally, goal1Bottom, goal1Top);

        double goal1dx = ballx - closestGoal1X;
        double goal1dy = bally - closestGoal1Y;

        if (pow(goal1dx, 2.0) + pow(goal1dy, 2.0) <= pow(0.1, 2.0))
        {
            cout << "GOAL FOR BLUE!!" << endl;
            cout << endl;
            blueScore++;

            ballx = 0.0;
            bally = 0.0;
            ballvx = 0.0;
            ballvy = 0.0;

            posx = -0.5;
            posy = 0.0;

            cout << "Yellow " << yellowScore << " - " << blueScore << " Blue" << endl; 
            cout << "______________________________________________________" << endl;
            cout << endl;
        }

        // detect collsion between goal 2 and ball
        double goal2Left = goal2x - 0.001;
        double goal2Right = goal2x + 0.001;
        double goal2Bottom = goal2y - 0.33;
        double goal2Top = goal2y + 0.33;

        double closestGoal2X = clamp(ballx, goal2Left, goal2Right);
        double closestGoal2Y = clamp(bally, goal2Bottom, goal2Top);

        double goal2dx = ballx - closestGoal2X;
        double goal2dy = bally - closestGoal2Y;

        if (pow(goal2dx, 2.0) + pow(goal2dy, 2.0) <= pow(0.1, 2.0))
        {
            cout << "GOAL FOR YELLOW!!" << endl;
            cout << endl;
            yellowScore++;

            ballx = 0.0;
            bally = 0.0;
            ballvx = 0.0;
            ballvy = 0.0;

            posx = -0.5;
            posy = 0.0;

            cout << "Yellow " << yellowScore << " - " << blueScore << " Blue" << endl;
            cout << "______________________________________________________" << endl;
            cout << endl;
        }

        if (yellowScore == 7 || blueScore == 7)
        {
            cout << "Game Over!!" << endl;
            cout << endl;
            
            if (yellowScore == 7)
            {
                cout << "Yellow Wins!!" << endl;
                cout << "______________________________________________________" << endl;
                cout << endl;
            }
            else if (blueScore == 7)
            {
                cout << "Blue Wins!!" << endl;
                cout << "______________________________________________________" << endl;
                cout << endl;
            }

            glfwSetWindowShouldClose(window, true);
        }

        // draw field lines
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glColor3f(1.0, 1.0, 1.0);

        glBegin(GL_LINES);
        glVertex2f(0.0, 1.0);
        glVertex2f(0.0, -1.0);
        glEnd();

        drawCircle(0.0, 0.0, 0.03, 32);

        // draw square
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(posx, posy, 0.0f);

        glColor3f(1.0, 1.0, 1.0);
        if (colorIndex == 1)
        {
            glColor3f(1.0f, 0.0f, 0.0f);
        }
        else if (colorIndex == 2)
        {
            glColor3f(0.0f, 1.0f, 0.0f);
        }
        else if (colorIndex == 3)
        {
            glColor3f(0.0f, 0.0f, 1.0f);
        }
        else if (colorIndex == 4)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
        }

        glBegin(GL_QUADS);
        glVertex2f(-0.1, 0.1);
        glVertex2f(0.1, 0.1);
        glVertex2f(0.1, -0.1);
        glVertex2f(-0.1, -0.1);
        glEnd();

        // draw ball
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor3f(1.0, 0.0, 1.0);
        drawCircle(ballx, bally, 0.1, 32);

        // draw goal 1
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(goal1x, goal1y, 0.0f);

        glColor3f(206.0 / 255.0, 1.0, 0.0);
        glBegin(GL_QUADS);
        glVertex2f(-0.01, 0.33);
        glVertex2f(0.01, 0.33);
        glVertex2f(0.01, -0.33);
        glVertex2f(-0.01, -0.33);
        glEnd();

        //draw goal 2
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(goal2x, goal2y, 0.0f);

        glColor3f(0.0, 1.0, 1.0);
        glBegin(GL_QUADS);
        glVertex2f(-0.01, 0.33);
        glVertex2f(0.01, 0.33);
        glVertex2f(0.01, -0.33);
        glVertex2f(-0.01, -0.33);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
