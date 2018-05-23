/**
 * @file    rsi_os_none.c
 * @version 0.1
 * @date    15 Aug,2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contain os handlers for os less platforms.
 *
 *  @section Description  This file contains function handlers for os less platforms.
 *
 *
 */



#ifndef RSI_WITH_OS
/**
 * Include files
 * */
#include <rsi_driver.h>

/*==============================================*/
/**
 * @fn          rsi_reg_flags_t rsi_critical_section_entry()
 * @brief       This API's to enter critical section
 * @param[in]   none,
 * @return      flags, interrupt status before entering critical section
 *
 *
 * @section description
 * This API disable interrupt to enter crtical section.
 *
 *
 */
rsi_reg_flags_t rsi_critical_section_entry()
{
  rsi_reg_flags_t xflags;
  //! hold interrupt status before entering critical section
  //! disable interrupts
  xflags = 0;

  //! return stored interrupt status
  return (xflags);
}
/*==============================================*/
/**
 * @fn         rsi_critical_section_exit(rsi_reg_flags_t xflags)
 * @brief      This API to exit critical section
 * @param[in]  xflags, interrupt status to restore interrupt on exit from critical section
 * @return     None
 *
 *
 * @section description
 * This API to exit/restore critical section.
 *
 *
 */

void  rsi_critical_section_exit(rsi_reg_flags_t xflags)
{
  //! restore interrupts while exiting critical section
}
/*==============================================*/
/**
 * @fn        void * rsi_virtual_to_physical_address(void *virtual_address)
 * @brief     This API to map virtual address to physical address
 * @param[in] virtual_address, pointer to virtual address
 * @return    void *, physical address
 *
 *
 * @section description
 * This API to map virtual adress to physical address.
 *
 *
 */
void * rsi_virtual_to_physical_address(void *virtual_address)
{
  return (virtual_address);
}
/*==============================================*/
/**
 * @fn        void *rsi_physical_to_virtual_address(void *physical_address)
 * @brief     This API to map physical address virtual address
 * @param[in] physical_address, pointer to physical_address
 * @return    void *, pointer to virtual address
 *
 *
 * @section description
 * This API to map physical address to virtual address
 *
 *
 */
void *rsi_physical_to_virtual_address(void *physical_address)
{
  return (physical_address);
}



/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

rsi_error_t rsi_mutex_create(rsi_mutex_handle_t *mutex)
{
  /* Create dummy mutxe pointer because OS not present */
  RSI_NO_OS_ATOMIC_MUTEX_SET(*mutex,RSI_NO_OS_MUTEX_UNLOCKED);
  return ( RSI_ERROR_NONE );
}
/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

rsi_error_t rsi_mutex_lock(volatile rsi_mutex_handle_t *mutex)
{
  while(RSI_NO_OS_ATOMIC_MUTEX_CHECK(*mutex,RSI_NO_OS_MUTEX_LOCKED));
  RSI_NO_OS_ATOMIC_MUTEX_SET(*mutex,RSI_NO_OS_MUTEX_LOCKED);
  return ( RSI_ERROR_NONE );
}
/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

rsi_error_t rsi_mutex_unlock(volatile rsi_mutex_handle_t *mutex)
{
  RSI_NO_OS_ATOMIC_MUTEX_SET(*mutex,RSI_NO_OS_MUTEX_UNLOCKED);
  return ( RSI_ERROR_NONE );
}
/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

rsi_error_t rsi_mutex_destory(rsi_mutex_handle_t *mutex)
{
  return ( RSI_ERROR_NONE );
}
/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

rsi_error_t rsi_semaphore_create(rsi_semaphore_handle_t *semaphore,uint32_t count)
{
  *semaphore = count;
  return ( RSI_ERROR_NONE );
}
/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

rsi_error_t rsi_semaphore_destroy(rsi_semaphore_handle_t *semaphore)
{
  *semaphore = 0;
  return ( RSI_ERROR_NONE );
}
/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

rsi_error_t rsi_semaphore_wait(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms )
{
  volatile uint32_t *event;
  event = (uint32_t *)semaphore;
  if (0UL != *event)
  {
    RSI_ATOMIC_DECREMENT(*event);
    return (RSI_ERROR_NONE);
  }
  if( 0UL == timeout_ms )
  {
    while( 0UL == *event )
    {
      //! if any events pending
      //! call rsi schedular
      rsi_scheduler(&rsi_driver_cb->scheduler_cb);
    }
    RSI_ATOMIC_DECREMENT( *event );
    return ( RSI_ERROR_NONE );
  }
  while( ( 0UL != timeout_ms ) && ( 0UL == *event ) )
  {
    //! if any events pending
    //! call rsi schedular
    rsi_scheduler(&rsi_driver_cb->scheduler_cb);
    rsi_delay_ms(1);
    timeout_ms--;
  }
  if ( 0UL == *event )
  {
    return ( RSI_ERROR_TIMEOUT );
  }
  else
  {
    RSI_ATOMIC_DECREMENT(*event);
  }
  return ( RSI_ERROR_NONE );
}
/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

rsi_error_t rsi_semaphore_post(rsi_semaphore_handle_t *semaphore)
{
  volatile uint32_t *event;

  event = (uint32_t *)semaphore;
  RSI_ATOMIC_INCREMENT(*event);
  return ( RSI_ERROR_NONE );
}
/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   ,
 * @param[out]
 * @return
 *
 *
 * @section description
 * This
 *
 *
 */

rsi_error_t rsi_semaphore_reset(rsi_semaphore_handle_t *semaphore)
{
  volatile uint32_t *event;

  event = ( uint32_t * )semaphore;
  *event = 0;
  return ( RSI_ERROR_NONE );
}

#endif
