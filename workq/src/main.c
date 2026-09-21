/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * A small k_work_q example: a workqueue is just a dedicated thread that pulls
 * work items off a queue and runs their handler functions.
 */

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

/* Blink period for the LED. */
#define BLINK_TIME_MS 500

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/*
 * A custom workqueue is two things:
 *   1. a k_work_q control block (holds the queue + its thread), and
 *   2. a stack for the thread that will run the handlers.
 *
 * The default system workqueue (k_sys_work_q) already exists; here we create
 * our own so we can show how one is put together.
 */
#define MY_WORKQ_STACK_SIZE 1024
#define MY_WORKQ_PRIORITY 5

static struct k_work_q my_workq;
static K_THREAD_STACK_DEFINE(my_workq_stack, MY_WORKQ_STACK_SIZE);

/*
 * Work items come in two flavours:
 *   - k_work            : runs as soon as it is submitted
 *   - k_work_delayable  : can be scheduled to run after a delay
 */

static void blink_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(blink_work, blink_handler);

static void tick_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(tick_work, tick_handler);

/*
 * Runs on the *system* workqueue. Toggles the LED, then reschedules itself so
 * the blink repeats. k_work_reschedule() re-arms the same delayable work.
 */
static void blink_handler(struct k_work *work)
{
    static bool led_state;
    struct k_work_delayable *dwork = k_work_delayable_from_work(work);

    if (gpio_pin_toggle_dt(&led) < 0)
    {
        return;
    }

    led_state = !led_state;
    printk("[sys_workq] LED %s\n", led_state ? "ON" : "OFF");

    k_work_reschedule(dwork, K_MSEC(BLINK_TIME_MS));
}

/*
 * Runs on our custom workqueue. Shows that a delayable work item can be bound
 * to a queue of our choosing via k_work_reschedule_for_queue().
 */
static void tick_handler(struct k_work *work)
{
    static uint32_t count;
    struct k_work_delayable *dwork = k_work_delayable_from_work(work);

    printk("[my_workq]  tick %u\n", ++count);

    k_work_reschedule_for_queue(&my_workq, dwork, K_MSEC(BLINK_TIME_MS * 2));
}

int main(void)
{
    int ret;

    if (!gpio_is_ready_dt(&led))
    {
        return 0;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0)
    {
        return 0;
    }

    /* Start the workqueue thread, giving it the stack and priority to use. */
    k_work_queue_start(&my_workq, my_workq_stack, K_THREAD_STACK_SIZEOF(my_workq_stack), MY_WORKQ_PRIORITY, NULL);

    /* Run blink_work on the default system workqueue. */
    k_work_schedule(&blink_work, K_NO_WAIT);

    /* Run tick_work on our own workqueue. */
    k_work_schedule_for_queue(&my_workq, &tick_work, K_NO_WAIT);

    /* Nothing left for main to do; both workqueues keep running. */
    return 0;
}
