.. zephyr:code-sample:: workq
   :name: Workqueue
   :relevant-api: workqueue_apis gpio_interface

   Use the workqueue API to toggle an LED and run periodic work.

Overview
********

This sample shows how to use the :ref:`workqueue API <workqueue_api>`.

The source code shows how to:

#. Start a custom workqueue (``k_work_queue_start``) backed by its own thread
   and stack
#. Define delayable work items (``K_WORK_DELAYABLE_DEFINE``)
#. Bind work to the default system workqueue with ``k_work_schedule``
#. Bind work to a custom workqueue with ``k_work_schedule_for_queue``
#. Reschedule a delayable work item from inside its own handler

A workqueue is simply a dedicated thread that pulls work items off a queue and
executes their handler functions, so the handlers always run in thread context
rather than interrupt context.

Requirements
************

Your board must:

#. Have an LED connected via a GPIO pin.
#. Have the LED configured using the ``led0`` devicetree alias.

Building and Running
********************

Build and flash the sample for the nRF5340 DK as follows:

.. zephyr-app-commands::
   :zephyr-app: workq
   :board: nrf5340dk/nrf5340/cpuapp
   :goals: build flash
   :compact:

After flashing, the LED starts to blink every 500 ms. Messages with the current
LED state (from the system workqueue) and a tick counter (from the custom
workqueue) are printed on the console.
