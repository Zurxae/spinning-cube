#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

const float WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;


typedef struct {
    float x, y, z;
} Vec3;


SDL_FPoint screen(SDL_FPoint p) {
    p.x = (p.x + 1)/2*WINDOW_WIDTH;
    p.y = (1 - (p.y + 1)/2)*WINDOW_HEIGHT;
    return p;
}


SDL_FPoint project(Vec3 p) {
    SDL_FPoint np;
    np.x = p.x/p.z;
    np.y = p.y/p.z;
    return np;
}


Vec3 rotateX(Vec3 p, float angle) {
    float radians = angle * M_PI / 180;

    float c = cosf(radians);
    float s = sinf(radians);

    float y = p.y;
    float z = p.z;

    p.y = y*c - z*s;
    p.z = y*s + z*c;

    return p;
}


Vec3 rotateY(Vec3 p, float angle) {
    float radians = angle * M_PI / 180;

    float c = cosf(radians);
    float s = sinf(radians);

    float x = p.x;
    float z = p.z;

    p.x = x*c - z*s;
    p.z = x*s + z*c;

    return p;
}


Vec3 rotateZ(Vec3 p, float angle) {
    float radians = angle * M_PI / 180;

    float c = cosf(radians);
    float s = sinf(radians);

    float x = p.x;
    float y = p.y;

    p.x = x*c - y*s;
    p.y = x*s + y*c;

    return p;
}


void drawPoint(SDL_FPoint p) {
    int s = 20;
    SDL_FRect point = { p.x-s/2, p.y-s/2, s, s};

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &point);
}


void drawLine(SDL_FPoint a, SDL_FPoint b) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderLine(renderer, a.x, a.y, b.x, b.y);
}


SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_CreateWindowAndRenderer("Hello World", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}



SDL_AppResult SDL_AppIterate(void *appstate)
{
    static Uint64 last = 0;

    Uint64 now = SDL_GetTicksNS();
    float dt = (last == 0) ? 0.0f : (now - last) / 1e9f; 
    last = now;

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    Vec3 vertices[8] = {
        { .x =  0.5f, .y = 0.5f, .z = 0.5f},
        { .x =  0.5f, .y = -0.5f, .z = 0.5f},
        { .x =  -0.5f, .y = -0.5f, .z = 0.5f},
        { .x =  -0.5f, .y = 0.5f, .z = 0.5f},

        { .x =  0.5f, .y = 0.5f, .z = -0.5f},
        { .x =  0.5f, .y = -0.5f, .z = -0.5f},
        { .x =  -0.5f, .y = -0.5f, .z = -0.5f},
        { .x =  -0.5f, .y = 0.5f, .z = -0.5f},
    };

    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},  // front face
        {4, 5}, {5, 6}, {6, 7}, {7, 4},  // back face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}   // connecting front to back
    };

    static float angle = 0.0f;
    angle += 80.0f * dt;

    static float dz = 3;
    //dz += 0.01f;

    //for (int i = 0; i < 8; i++) {
    //    Vec3 v = rotate(vertices[i], angle);
    //    v.z += dz;
    //    drawPoint(screen(project(v)));
    //}

    for (int i = 0; i < 12; i++) {
        int a = edges[i][0];
        int b = edges[i][1];
        Vec3 va = rotateY(rotateX(vertices[a], angle), angle);
        Vec3 vb = rotateY(rotateX(vertices[b], angle), angle);
        //Vec3 va = rotateX(vertices[a], angle);
        //Vec3 vb = rotateX(vertices[b], angle);
        //Vec3 va = rotateY(vertices[a], angle);
        //Vec3 vb = rotateY(vertices[b], angle);
        //Vec3 va = rotateZ(vertices[a], angle);
        //Vec3 vb = rotateZ(vertices[b], angle);

        va.z += dz;
        vb.z += dz;

        drawLine(screen(project(va)), screen(project(vb)));
    }

    SDL_RenderPresent(renderer);
    
    // --- frame limiting ---
    Uint64 frame_end = SDL_GetTicksNS();
    float frame_duration = (frame_end - now) / 1e9f; // seconds
    float target = 1.0f / 60.0f;
    if (frame_duration < target) {
        SDL_Delay((target - frame_duration) * 1000); // SDL_Delay takes ms
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

