#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "../src/tlsf/tlsf.h"

#define TEST_POOL_SIZE 32 * 1024 * 1024 
#define NUM_ALLOCATIONS 1500
#define RETAIN_PERCENTAGE 70
#define NUM_LOOPS 10

#define PROB_4KB_OR_LESS 0.80
#define PROB_4KB_TO_40KB (1 - PROB_4KB_OR_LESS - PROB_ABOVE_40KB)
#define PROB_ABOVE_40KB 0.005



int test_basic_malloc_free() {
    tlsf_t tlsf = tlsf_create_with_pool(malloc(TEST_POOL_SIZE), TEST_POOL_SIZE);
    void *ptr = tlsf_malloc(tlsf, 128);
    if (!ptr) {
        printf("Failed to allocate memory.\n");
        return 1;
    }
    tlsf_free(tlsf, ptr);
    printf("Basic malloc/free test passed.\n");
    tlsf_destroy(tlsf);
    return 0;
}

int test_memalign() {
    tlsf_t tlsf = tlsf_create_with_pool(malloc(TEST_POOL_SIZE), TEST_POOL_SIZE);
    void *ptr = tlsf_memalign(tlsf, 16, 256);
    if (!ptr) {
        printf("Failed to allocate aligned memory.\n");
        return 1;
    }
    if (((uintptr_t)ptr) % 16 != 0) {
        printf("Memory is not aligned properly.\n");
        tlsf_free(tlsf, ptr);
        tlsf_destroy(tlsf);
        return 1;
    }
    tlsf_free(tlsf, ptr);
    printf("Memalign test passed.\n");
    tlsf_destroy(tlsf);
    return 0;
}

int test_realloc() {
    tlsf_t tlsf = tlsf_create_with_pool(malloc(TEST_POOL_SIZE), TEST_POOL_SIZE);
    void *ptr = tlsf_malloc(tlsf, 128);
    if (!ptr) {
        printf("Failed to allocate memory.\n");
        return 1;
    }
    ptr = tlsf_realloc(tlsf, ptr, 256);
    if (!ptr) {
        printf("Failed to reallocate memory.\n");
        tlsf_free(tlsf, ptr);
        return 1;
    }
    tlsf_free(tlsf, ptr);
    printf("Realloc test passed.\n");
    tlsf_destroy(tlsf);
    return 0;
}

void *ptrs[NUM_LOOPS][NUM_ALLOCATIONS] = {0}; 
size_t sizes[NUM_LOOPS][NUM_ALLOCATIONS] = {0}; 

int test_fragmentation() {
    tlsf_t tlsf = tlsf_create_with_pool(malloc(TEST_POOL_SIZE), TEST_POOL_SIZE);
    if (!tlsf) {
        fprintf(stderr, "Failed to create TLSF memory pool.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));
    size_t total_allocated = 0;



    for (int loop = 0; loop < NUM_LOOPS; ++loop) {
        //printf("\nLoop %d:\n", loop + 1);

        for (int step = 0; step < NUM_ALLOCATIONS; ++step) {
            if (rand() % 100 < 30) { 
                //size_t alloc_size = (rand() % 4096) + 256;
                double prob = (double)rand() / RAND_MAX; // 生成一个0到1之间的随机数
                size_t alloc_size = 0;
                if (prob < PROB_4KB_OR_LESS) {
                    alloc_size = (rand() % 4096) + 256; // 256B到4KB
                } else if (prob < PROB_4KB_OR_LESS + PROB_4KB_TO_40KB) {
                    alloc_size = (rand() % (40000 - 4096)) + 4096; // 4KB到40KB
                } else {
                    alloc_size = (rand() % (450 * 1024 - 40000)) + 40000; // 40KB到450KB
                }
                ptrs[loop][step] = tlsf_malloc(tlsf, alloc_size);
                if (!ptrs[loop][step]) {
                    fprintf(stderr, "Failed to allocate memory.\n");
                    return 1;
                }
                sizes[loop][step] = alloc_size;
                total_allocated += alloc_size;
                //printf("Allocated: %zu bytes at %p\n", alloc_size, ptrs[loop][step]);
            } else { 
                int release_loop = rand() % (loop + 1);
                int release_step = rand() % (step + 1);
                if (ptrs[release_loop][release_step]) {
                    size_t freed_size = sizes[release_loop][release_step];
                    tlsf_free(tlsf, ptrs[release_loop][release_step]);
                    total_allocated -= freed_size;
                    //printf("Freed: %zu bytes at %p\n", freed_size, ptrs[release_loop][release_step]);
                    ptrs[release_loop][release_step] = NULL; 
                }
            }
        }
    }

    // 确保至少保留了 RETAIN_PERCENTAGE 百分比的内存块未被释放
    int retained_count = NUM_LOOPS * NUM_ALLOCATIONS * RETAIN_PERCENTAGE / 100;
    for (int loop = 0; loop < NUM_LOOPS && retained_count > 0; ++loop) {
        for (int i = 0; i < NUM_ALLOCATIONS && retained_count > 0; ++i) {
            if (ptrs[loop][i]) {
                --retained_count;
                ptrs[loop][i] = NULL; // 标记为不释放
            }
        }
    }

    // 释放剩余的内存块
    for (int loop = 0; loop < NUM_LOOPS; ++loop) {
        for (int i = 0; i < NUM_ALLOCATIONS; ++i) {
            if (ptrs[loop][i]) {
                size_t freed_size = sizes[loop][i];
                tlsf_free(tlsf, ptrs[loop][i]);
                total_allocated -= freed_size;
            }
        }
    }

    // 打印内存使用状态
    //tlsf_print_memory_status(tlsf);
    //print_free_blocks(tlsf);
    // 获取空闲内存信息
    FreeMemoryInfo free_info;
    tlsf_get_free_info(tlsf, &free_info);

    printf("Total allocated: %zu bytes\n", total_allocated);
    printf("Max free block: %zu bytes\n", free_info.max_free_block);
    printf("Total free: %zu bytes\n", free_info.total_free);

    tlsf_destroy(tlsf);
    free(malloc(TEST_POOL_SIZE));

    return 0;
}


int main() {
    const int num_tests = 100; // 定义要运行测试的次数
    for (int i = 0; i < num_tests; ++i) {
        printf("Running test_fragmentation: %d/%d\n", i + 1, num_tests);
        if (test_fragmentation() != 0) {
            fprintf(stderr, "test_fragmentation failed on iteration %d.\n", i + 1);
            return 1; // 如果任何一次测试失败，则退出程序
        }
        sleep(1);
    }
    printf("All tests passed.\n");
    return 0;
}