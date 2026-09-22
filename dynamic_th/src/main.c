/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Dynamic thread example: create threads whose stack and control block are
 * allocated at runtime instead of reserving RAM statically.
 *
 * Compare with the usual static approach:
 *
 *   K_THREAD_STACK_DEFINE(my_stack, STACK_SIZE);
 *   struct k_thread my_thread;
 *   k_thread_create(&my_thread, my_stack, K_THREAD_STACK_SIZEOF(my_stack),
 *                   worker, NULL, NULL, NULL, PRIO, 0, K_NO_WAIT);
 *
 * or the one-liner K_THREAD_DEFINE() which does both at build time. Here
 * nothing is reserved until main() runs.
 */

#include <zephyr/kernel.h>

#define STACK_SIZE 1024
#define THREAD_PRIORITY 5
#define TICK_MS 1000

static void worker(void *p1, void *p2, void *p3)
{
    const char *name = p1;
    uint32_t count = 0;

    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    for (;;)
    {
        printk("[%s] tick %u\n", name, ++count);
        k_msleep(TICK_MS);
    }
}

int main(void)
{
    const char *names[2] = {"thread-a", "thread-b"};
    k_thread_stack_t *stacks[2];
    struct k_thread *threads[2];

    for (int i = 0; i < 2; i++)
    {
        /* Stack is taken from the kernel heap (CONFIG_DYNAMIC_THREAD_ALLOC). */
        stacks[i] = k_thread_stack_alloc(STACK_SIZE, 0);
        /* The thread control block is heap-allocated as well. */
        threads[i] = k_malloc(sizeof(struct k_thread));

        if (stacks[i] == NULL || threads[i] == NULL)
        {
            printk("Out of memory allocating %s\n", names[i]);
            return 0;
        }

        k_thread_create(threads[i], stacks[i], STACK_SIZE, worker, (void *)names[i], NULL, NULL, THREAD_PRIORITY, 0,
                        K_NO_WAIT);
    }

    /* Both workers run forever, so the memory is intentionally not freed. */
    return 0;
}
