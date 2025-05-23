#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdio.h>

const double INITIAL_X1 = 10.0;
const double INITIAL_X2 = 2.0;
const double TIME_STEP = 0.001;
const int WINDOW_WIDTH = 16 * 40;
const int WINDOW_HEIGHT = 9 * 40;

struct SystemState {
    double x1, x2;
    double n1, n2;
    double u;

    SystemState() : x1(INITIAL_X1), x2(INITIAL_X2), n1(INITIAL_X1), n2(0), u(0) {}

    void update() {

        double dx1 = x2;
        double dx2 = 4.9 * x1 - 0.25 * u;
        double du = 819.6 * n1 + 408 * n2;
        double dn1 = 102 * (x1 - n1) + n2;
        double dn2 = -200 * n1 + 204.9 * x1 - 0.25 * u;

        x1 += dx1 * TIME_STEP;
        x2 += dx2 * TIME_STEP;
        n1 += dn1 * TIME_STEP;
        n2 += dn2 * TIME_STEP;
        u = du;
    }

    void reset() {
        x1 = n1 = INITIAL_X1;
        x2 = INITIAL_X2;
        n2 = u = 0;
    }
};

SystemState systemState;
double currentTime = 0;
bool isAutoRunning = false;
int simulationSpeed = 1000;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
void Render(HDC hdc);
void Reshape(int width, int height);
void DrawPendulum(double angle);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int iCmdShow) {
    WNDCLASS wc = {
        CS_OWNDC, WndProc, 0, 0, hInstance,
        LoadIcon(NULL, IDI_APPLICATION),
        LoadCursor(NULL, IDC_ARROW),
        (HBRUSH)GetStockObject(BLACK_BRUSH),
        NULL, "GLWindow"
    };
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(
        "GLWindow", "Pendulum Simulation - Press A/Q/S",
        WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
        100, 100, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);

    HDC hDC;
    HGLRC hRC;
    EnableOpenGL(hWnd, &hDC, &hRC);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    Reshape(WINDOW_WIDTH, WINDOW_HEIGHT);

    MSG msg;
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            if (isAutoRunning) {
                currentTime += TIME_STEP;
                systemState.update();
                Sleep(static_cast<int>(simulationSpeed * TIME_STEP));
            }
            Render(hDC);
        }
    }


    DisableOpenGL(hWnd, hDC, hRC);
    DestroyWindow(hWnd);
    return msg.wParam;
}


void Render(HDC hdc) {
    glClear(GL_COLOR_BUFFER_BIT);

    DrawPendulum(-systemState.n1);

    glFlush();
    SwapBuffers(hdc);
}


void DrawPendulum(double angle) {
    const double centerX = WINDOW_WIDTH / 2.0;
    const double centerY = WINDOW_HEIGHT / 4.0;
    const double rodWidth = WINDOW_WIDTH / 120.0;
    const double rodLength = WINDOW_HEIGHT / 3.0;
    const double ballRadius = rodWidth * 1.5;


    glColor3ub(200, 103, 6);
    glBegin(GL_TRIANGLES);
    {
        const double triangleSize = WINDOW_WIDTH / 20.0;
        const double k = 0.57735;  // tan(60°)/2
        glVertex2f(centerX, centerY + k * triangleSize);
        glVertex2f(centerX - triangleSize/2, centerY - k * triangleSize/2);
        glVertex2f(centerX + triangleSize/2, centerY - k * triangleSize/2);
    }
    glEnd();


    glPushMatrix();
    glTranslatef(centerX, centerY, 0);
    glRotated(angle, 0, 0, 1.0);

    glColor3ub(215, 81, 30);
    glBegin(GL_QUADS);
    {
        glVertex2f(-rodWidth/2, 0);
        glVertex2f(rodWidth/2, 0);
        glVertex2f(rodWidth/2, rodLength);
        glVertex2f(-rodWidth/2, rodLength);
    }
    glEnd();


    glColor3ub(255, 255, 255);
    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex2f(0, rodLength);
        for (double t = 0; t <= 2 * 3.1415; t += 2 * 3.1415 / 100.0) {
            glVertex2f(ballRadius * cos(t), rodLength + ballRadius * sin(t));
        }
    }
    glEnd();

    glPopMatrix();
}


void Reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    return 0;
                case 'A': case 'a':
                    isAutoRunning = !isAutoRunning;
                    break;
                case 'Q': case 'q':
                    systemState.reset();
                    currentTime = 0;
                    break;
                case 'S': case 's':
                    simulationSpeed = 1000 - simulationSpeed;
                    break;
                case VK_SPACE:
                    for (int i = 0; i < 5; i++) systemState.update();
                    break;
            }
            return 0;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC) {
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(pfd), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };

    *hDC = GetDC(hWnd);
    int format = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, format, &pfd);
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}
