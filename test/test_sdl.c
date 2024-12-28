#include <SDL.h>
#include <stdbool.h>
// 假设这些宏定义已经在你的代码中设置
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// 绘制内存使用情况的函数
void DrawMemoryUsage(SDL_Renderer* renderer, size_t total_memory, size_t used_memory) {
    // 计算已使用内存和空闲内存的比例
    float used_ratio = (float)used_memory / total_memory;
    float free_ratio = 1.0f - used_ratio;

    // 计算矩形的宽度
    int used_width = (int)(SCREEN_WIDTH * used_ratio);
    int free_width = SCREEN_WIDTH - used_width;

    // 设置绘制颜色
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // 红色用于已使用内存
    SDL_Rect used_rect = {0, SCREEN_HEIGHT / 2, used_width, SCREEN_HEIGHT / 2}; // 已使用内存的矩形
    SDL_RenderFillRect(renderer, &used_rect); // 绘制已使用内存的矩形

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // 绿色用于空闲内存
    SDL_Rect free_rect = {used_width, SCREEN_HEIGHT / 2, free_width, SCREEN_HEIGHT / 2}; // 空闲内存的矩形
    SDL_RenderFillRect(renderer, &free_rect); // 绘制空闲内存的矩形
}

// 在你的主循环中调用这个函数
int main(int argc, char* argv[]) {
    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // 创建窗口
    SDL_Window* window = SDL_CreateWindow("Memory Visualization", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // 创建渲染器
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 主循环
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // 清除屏幕
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // 假设这些值是你的内存使用情况
        size_t total_memory = 1024 * 1024 * 1024; // 1GB
        size_t used_memory = 512 * 1024 * 1024; // 512MB

        // 绘制内存使用情况
        DrawMemoryUsage(renderer, total_memory, used_memory);

        // 更新屏幕
        SDL_RenderPresent(renderer);
    }

    // 清理
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}