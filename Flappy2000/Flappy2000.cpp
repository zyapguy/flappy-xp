// Flappy2000.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <windows.h>
#include <math.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 360

#define BIRD_WIDTH 40
#define BIRD_HEIGHT 40

#define GRAVITY 0.5f
#define FLAP_STRENGTH -8.0f

#define PIPE_WIDTH 80
#define PIPE_GAP 150
#define SCROLL_SPEED 4

bool isGameOver = false;
int score = 0;

HBITMAP hBirdBitmap = NULL;

void LoadResources();
void UpdateGame();
void RenderGame(HDC hdc);

class Bird {
public:
    float y;
    float velocity;

    Bird() : y(240), velocity(0.0f) {}

    void Flap() {
        velocity = FLAP_STRENGTH;  // Apply strong flap to counter gravity
    }

    void Update() {
        // Apply gravity to the bird's velocity
        velocity += GRAVITY;

        // Limit the maximum fall velocity
        if (velocity > 10.0f) {  // Prevent falling too fast
            velocity = 10.0f;
        }

        // Update the bird's vertical position based on velocity
        y += velocity;

        // Collision with ground
        if (y + BIRD_HEIGHT > SCREEN_HEIGHT) {
            y = SCREEN_HEIGHT - BIRD_HEIGHT;  // Prevent bird from falling below the screen
            isGameOver = true;  // End the game if bird hits the ground
        }

        // Collision with top of the screen
        if (y < 0) {
            y = 0;  // Prevent bird from going off the top of the screen
            velocity = 0;  // Stop upward movement if bird hits top
        }
    }

    void Draw(HDC hdc) const {
        if (hBirdBitmap) {
            float angle = atan2(velocity, 10) * 180.0f / 3.14159f;

            int savedDC = SaveDC(hdc);

            XFORM transform;
            memset(&transform, 0, sizeof(XFORM));
            float radians = angle * 3.14159f / 180.0f;
            float cosTheta = cos(radians);
            float sinTheta = sin(radians);

            transform.eM11 = cosTheta;
            transform.eM12 = sinTheta;
            transform.eM21 = -sinTheta;
            transform.eM22 = cosTheta;
            transform.eDx = 100 + BIRD_WIDTH / 2;
            transform.eDy = y + BIRD_HEIGHT / 2;

            SetGraphicsMode(hdc, GM_ADVANCED);
            SetWorldTransform(hdc, &transform);

            HDC hMemDC = CreateCompatibleDC(hdc);
            SelectObject(hMemDC, hBirdBitmap);
            BitBlt(hdc, -BIRD_WIDTH / 2, -BIRD_HEIGHT / 2, BIRD_WIDTH, BIRD_HEIGHT, hMemDC, 0, 0, SRCCOPY);
            DeleteDC(hMemDC);

            RestoreDC(hdc, savedDC);
        }
    }
};

class Pipe {
public:
    int x;
    int topHeight;

    Pipe() : x(0), topHeight(0) {}
    Pipe(int initialX) : x(initialX), topHeight(50 + rand() % (SCREEN_HEIGHT - PIPE_GAP - 50)) {}

    void Update() {
        x -= SCROLL_SPEED;
        if (x + PIPE_WIDTH < 0) {
            x = SCREEN_WIDTH;
            topHeight = 50 + rand() % (SCREEN_HEIGHT - PIPE_GAP - 50);
            score++;
        }
    }

    void Draw(HDC hdc) const {
        HBRUSH pipeBrush = CreateSolidBrush(RGB(0, 255, 0));
        RECT topPipe = {x, 0, x + PIPE_WIDTH, topHeight};
        RECT bottomPipe = {x, topHeight + PIPE_GAP, x + PIPE_WIDTH, SCREEN_HEIGHT - 50};
        FillRect(hdc, &topPipe, pipeBrush);
        FillRect(hdc, &bottomPipe, pipeBrush);
        DeleteObject(pipeBrush);
    }

    bool checkCollision(const Bird &bird) {
        if (x < 100 + BIRD_WIDTH && x + PIPE_WIDTH > 100 &&
            (bird.y < topHeight || bird.y + BIRD_HEIGHT > topHeight + PIPE_GAP)) {
            return true;
        }
        return false;
    }
};

/* Declare Windows procedure */
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

Bird bird;
Pipe pipes[2];

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RenderGame(hdc);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_KEYDOWN:
        if (wParam == VK_SPACE && !isGameOver) {
            bird.Flap();
        }
        if (wParam == VK_RETURN && isGameOver) {
            isGameOver = false;
            bird = Bird();
            pipes[0] = Pipe(SCREEN_WIDTH);
            pipes[1] = Pipe(SCREEN_WIDTH + SCREEN_WIDTH / 2 + PIPE_WIDTH / 2);
            score = 0;
        }
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

void LoadResources() {
    hBirdBitmap = (HBITMAP)LoadImage(NULL, "bird.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBirdBitmap) {
        MessageBox(NULL, "Failed to load bird.bmp", "Flappy Bird 2000 | Error", MB_ICONERROR);
        exit(1);
    }
}

void UpdateGame() {
    if (isGameOver)
        return;

    bird.Update();

    for (int i = 0; i < 2; ++i) {
        pipes[i].Update();
        if (pipes[i].checkCollision(bird)) {
            isGameOver = true;
        }
    }
}

void RenderGame(HDC hdc) {
    HBRUSH bgBrush = CreateSolidBrush(RGB(135, 206, 235));
    RECT rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    FillRect(hdc, &rect, bgBrush);
    DeleteObject(bgBrush);

    HBRUSH groundBrush = CreateSolidBrush(RGB(34, 139, 34));
    RECT groundRect = {0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, SCREEN_HEIGHT};
    FillRect(hdc, &groundRect, groundBrush);
    DeleteObject(groundBrush);

    bird.Draw(hdc);
    for (int i = 0; i < 2; ++i) {
        pipes[i].Draw(hdc);
    }

    char scoreText[32];
    sprintf(scoreText, "Score: %d", score);
    SetBkMode(hdc, TRANSPARENT);
    TextOut(hdc, 10, 10, scoreText, strlen(scoreText));

    if (isGameOver) {
        const char *gameOverText = "Game Over! Press Enter To Restart!";
        TextOut(hdc, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, gameOverText, strlen(gameOverText));
    }
}

int main(int argc, char* argv[])
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    pipes[0] = Pipe(SCREEN_WIDTH);
    pipes[1] = Pipe(SCREEN_WIDTH + SCREEN_WIDTH / 2 + PIPE_WIDTH / 2);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "FlappyBird";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("FlappyBird", "Flappy Bird 2000",
                             WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
                             SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOW);

    LoadResources();

    MSG msg;
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            UpdateGame();
            InvalidateRect(hwnd, NULL, FALSE);
            Sleep(16);
        }
    }

    return 0;
}