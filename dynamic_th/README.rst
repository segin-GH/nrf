.. zephyr:code-sample:: dynamic_th
   :name: Dynamic Threads
   :relevant-api: thread_apis

   Create two threads using dynamically allocated stacks and control blocks.

Overview
********

This sample shows how to create threads without reserving their memory at
build time. A thread normally needs two things baked into the image:

#. A stack (``K_THREAD_STACK_DEFINE``), and
#. A control block (``struct k_thread``).

The convenience macro ``K_THREAD_DEFINE`` creates both statically. This sample
instead allocates them at runtime:

#. Allocate the stack from the kernel heap with ``k_thread_stack_alloc``
   (requires ``CONFIG_DYNAMIC_THREAD`` and ``CONFIG_DYNAMIC_THREAD_ALLOC``).
#. Allocate the control block with ``k_malloc``.
#. Start the thread with ``k_thread_create``.

The sample starts two worker threads that each print a tick every second and
run forever. Because they never terminate, their memory is intentionally never
freed; a terminating thread should release its stack with
``k_thread_stack_free`` and its control block with ``k_free``.

Requirements
************

No specific hardware is required; the sample only uses the kernel and the
console.

Building and Running
********************

Build and flash the sample for the nRF5340 DK as follows:

.. zephyr-app-commands::
   :zephyr-app: dynamic_th
   :board: nrf5340dk/nrf5340/cpuapp
   :goals: build flash
   :compact:

After flashing, the console prints interleaved tick messages from both
dynamically created threads.
