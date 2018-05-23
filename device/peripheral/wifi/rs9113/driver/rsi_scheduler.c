/**
 * @file       rsi_scheduler.c
 * @version    0.1
 * @date       2015-Aug-15
 *
 * Copyright(C) Redpine Signals 2012
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief This file contain function for scheduler implementation
 *
 * @section Description
 * This file contains function related to scheduler implementation
 *
 *
 */

/**
 * Includes
 */
#include "rsi_driver.h"
/**
 * Global Variables
 */

/*====================================================*/
/**
 * @fn          uint16_t rsi_scheduler_init(rsi_scheduler_cb_t *scheduler_cb)
 * @brief       Initializes scheduler
 * @param[in]   rsi_scheduler_cb_t scheduler_cb, pointer to scheduler structure
 * @return      void
 * @section description
 * This function is used initialize scheduler
 */
void rsi_scheduler_init(rsi_scheduler_cb_t *scheduler_cb)
{
  //! Set no event
  scheduler_cb->event_map = 0x0;

  //! Set no mask
  scheduler_cb->mask_map = 0xFFFFFFFF;
#ifdef RSI_WITH_OS
   //! create semaphore
	rsi_semaphore_create(&scheduler_cb->scheduler_sem, 0);
#endif
  return;
}


uint32_t rsi_get_event(rsi_scheduler_cb_t *scheduler_cb)
{
  uint32_t       active_int_event_map;

  rsi_reg_flags_t flags;

  //! Disable all the interrupts
  flags = rsi_critical_section_entry();

  //! Get current event map after applying mask
  active_int_event_map = (scheduler_cb->event_map & scheduler_cb->mask_map);

  //! Enable all the interrupts
  rsi_critical_section_exit(flags);

  return active_int_event_map;

}


/*====================================================*/
/**
 * @fn          void rsi_scheduler(rsi_scheduler_cb_t *scheduler_cb)
 * @brief       scheduler to handle events
 * @param[in]   rsi_scheduler_cb_t *scheduler_cb, pointer to scheduler cb structure
 * @return      void
 * @section description
 * This scheduler function handles events
 */
void rsi_scheduler(rsi_scheduler_cb_t *scheduler_cb)
{
  uint32_t       event_no;
  rsi_event_cb_t *temp_event;


#ifdef RSI_WITH_OS
  while(rsi_get_event(scheduler_cb))
#else
  if(rsi_get_event(scheduler_cb))
#endif
  {
    //! Find event event
    event_no = rsi_find_event(rsi_get_event(scheduler_cb));

    //! Get event handler
    temp_event = &rsi_driver_cb->event_list[event_no];

    if(temp_event->event_handler)
    {
      //! Call event handler
      temp_event->event_handler();
    }
    else
    {
      while(1); //If Unregistered Events are raised, event handler is NULL
    }
  }
#ifdef RSI_WITH_OS
   rsi_semaphore_wait(&scheduler_cb->scheduler_sem, 0);
#endif
}

