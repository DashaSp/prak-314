#define S_FUNCTION_NAME  MyDrawSystem
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"
#include "matrix.h"
#include <math.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>

// Структура для хранения информации об окне OpenGL
typedef struct {
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
} WindowWorkInfo;

// Прототипы функций
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
void DrawPendulum(double angle, int width, int height);

// Создание окна OpenGL
void createWindow(WindowWorkInfo* pWWI) {
    WNDCLASS wc = {
        CS_OWNDC, WndProc, 0, 0, GetModuleHandle(NULL),
        LoadIcon(NULL, IDI_APPLICATION),
        LoadCursor(NULL, IDC_ARROW),
        (HBRUSH)GetStockObject(BLACK_BRUSH),
        NULL, "GLPendulumWindow"
    };
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(
        "GLPendulumWindow", "Pendulum Visualization (MATLAB)",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100, 600, 600,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    HDC hDC;
    HGLRC hRC;
    EnableOpenGL(hWnd, &hDC, &hRC);

    pWWI->hwnd = hWnd;
    pWWI->hdc = hDC;
    pWWI->hglrc = hRC;
}

// Обработчик сообщений окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

// Инициализация OpenGL
void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC) {
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };

    *hDC = GetDC(hWnd);
    int format = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, format, &pfd);
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}

// Очистка OpenGL
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}

// Отрисовка маятника
void DrawPendulum(double angle, int width, int height) {
    const double centerX = width / 2.0;
    const double centerY = height / 4.0;
    const double rodWidth = width / 120.0;
    const double rodLength = height / 3.0;
    const double ballRadius = rodWidth * 1.5;

    // Очистка экрана
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Установка ортографической проекции
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // Отрисовка основания (треугольник)
    glColor3ub(200, 103, 6);
    glBegin(GL_TRIANGLES);
    {
        const double triangleSize = width / 20.0;
        const double k = 0.57735; // tan(60°)/2
        glVertex2f(centerX, centerY + k * triangleSize);
        glVertex2f(centerX - triangleSize/2, centerY - k * triangleSize/2);
        glVertex2f(centerX + triangleSize/2, centerY - k * triangleSize/2);
    }
    glEnd();

    // Отрисовка стержня маятника
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

    // Отрисовка шара маятника
    glColor3ub(255, 255, 255);
    glBegin(GL_TRIANGLE_FAN);
    {
        glVertex2f(0, rodLength);
        for (double t = 0; t <= 2 * 3.1415; t += 0.1) {
            glVertex2f(ballRadius * cos(t), rodLength + ballRadius * sin(t));
        }
    }
    glEnd();

    glPopMatrix();
}

// Проверка параметров
#define MDL_CHECK_PARAMETERS
void mdlCheckParameters(SimStruct *S) {
    if (ssGetNumSFcnParams(S) != 0) {
        ssSetErrorStatus(S, "This S-Function has no parameters");
        return;
    }
}

// Инициализация размеров
#define MDL_INITIALIZE_SIZES
void mdlInitializeSizes(SimStruct *S) {
    ssSetNumSFcnParams(S, 0);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return;
    }

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, 1);  // Ожидаем 1 входной сигнал (угол)
    ssSetInputPortDirectFeedThrough(S, 0, 1);

    if (!ssSetNumOutputPorts(S, 0)) return;

    ssSetNumSampleTimes(S, 1);
    ssSetNumDWork(S, 1);
    ssSetDWorkWidth(S, 0, sizeof(WindowWorkInfo*));
    ssSetDWorkDataType(S, 0, SS_POINTER);
}

// Инициализация временных характеристик
void mdlInitializeSampleTimes(SimStruct *S) {
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}

// Инициализация (создание окна)
#define MDL_START
void mdlStart(SimStruct *S) {
    WindowWorkInfo** ppWWI = (WindowWorkInfo**)ssGetDWork(S, 0);
    *ppWWI = (WindowWorkInfo*)malloc(sizeof(WindowWorkInfo));
    createWindow(*ppWWI);
}

// Основная функция вывода
#define MDL_OUTPUTS
void mdlOutputs(SimStruct *S, int_T tid) {
    // Получение входного сигнала (угол)
    const real_T* anglePtr = (const real_T*)ssGetInputPortSignal(S, 0);
    double angle = -anglePtr[0]; // Инвертируем угол для корректного отображения

    // Получение информации об окне
    WindowWorkInfo* pWWI = *(WindowWorkInfo**)ssGetDWork(S, 0);

    // Обработка сообщений Windows
    MSG msg;
    while (PeekMessage(&msg, pWWI->hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Отрисовка маятника
    wglMakeCurrent(pWWI->hdc, pWWI->hglrc);
    
    RECT rect;
    GetClientRect(pWWI->hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    DrawPendulum(angle, width, height);
    SwapBuffers(pWWI->hdc);
}

// Завершение работы
void mdlTerminate(SimStruct *S) {
    WindowWorkInfo* pWWI = *(WindowWorkInfo**)ssGetDWork(S, 0);
    if (pWWI) {
        DisableOpenGL(pWWI->hwnd, pWWI->hdc, pWWI->hglrc);
        DestroyWindow(pWWI->hwnd);
        free(pWWI);
    }
}

#ifdef MATLAB_MEX_FILE
#include "simulink.c"
#else
#include "cg_sfun.h"
#endif
