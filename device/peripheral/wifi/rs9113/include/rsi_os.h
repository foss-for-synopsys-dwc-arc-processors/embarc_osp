/**
 * @file      rsi_os.h
 * @version   0.1
 * @date      15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief :  This file contains OS related functionality defines and function definations
 *
 *  @section Description  This file contains OS related functionality defines and function definations
 *
 *
 */

#ifndef  RSI_OS_H
#define  RSI_OS_H
/******************************************************
 * *                      Macros
 * ******************************************************/
//! Macro to increment a value
#define RSI_ATOMIC_INCREMENT( value )  { \
  ( value )++; \
}

//! Macro to decrement a value
#define RSI_ATOMIC_DECREMENT( value )   { \
  (value)--; \
}
//! Error none (success)
#define  RSI_ERR_NONE              (0)

//! Error returned when invalid arguments are given
#define  RSI_ERR_INVALID_ARGS      (1)

//! Error returned when timeout error occurs
#define  RSI_ERR_TIMEOUT           (3)

//! Mutex unlock value
#define RSI_NO_OS_MUTEX_UNLOCKED    (0)

//! Mutex lock value
#define RSI_NO_OS_MUTEX_LOCKED      (1)

//! Macro to set the mutex lock
#define RSI_NO_OS_ATOMIC_MUTEX_SET(mutex, value) (mutex) = value

//! Macro for checking whether mutex is locked or not
#define RSI_NO_OS_ATOMIC_MUTEX_CHECK(mutex,value) (((mutex) == value)? 1:0)
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
typedef uint32_t rsi_reg_flags_t;
//! Handle to manage Semaphores.
typedef uint32_t rsi_semaphore_handle_t;
//! Handle to manage Mutex.
typedef uint32_t rsi_mutex_handle_t;

//! Task handler
typedef void* rsi_task_handle_t;

//! Task function
typedef void (*rsi_task_function_t)(void* function);
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
/* --------- CRITICAL SECTION FUNCTIONS --------- */
rsi_reg_flags_t  rsi_critical_section_entry(void);
void rsi_critical_section_exit(rsi_reg_flags_t  flags);
/* --------------  MUTEX FUNCTIONS -------------- */
rsi_error_t  rsi_mutex_create(rsi_mutex_handle_t *p_mutex);
rsi_error_t  rsi_mutex_lock(volatile rsi_mutex_handle_t *p_mutex);
rsi_error_t  rsi_mutex_unlock(volatile rsi_mutex_handle_t *p_mutex);
rsi_error_t  rsi_mutex_destory(rsi_mutex_handle_t *p_mutex);

/*  ------------- SEMAPHORE FUNCTIONS ----------- */
rsi_error_t  rsi_semaphore_create(rsi_semaphore_handle_t *p_sem,uint32_t  cnt);
rsi_error_t  rsi_semaphore_destroy(rsi_semaphore_handle_t *p_sem);
rsi_error_t  rsi_semaphore_wait(rsi_semaphore_handle_t *p_sem,uint32_t  timeout);
rsi_error_t  rsi_semaphore_post(rsi_semaphore_handle_t  *p_sem);
rsi_error_t  rsi_semaphore_reset(rsi_semaphore_handle_t  *p_sem);

/*  ------------- TASK FUNCTIONS ----------- */
rsi_error_t rsi_task_create( rsi_task_function_t task_function,uint8_t *task_name,
                              uint32_t stack_size, void *parameters,
                              uint32_t task_priority,rsi_task_handle_t  *task_handle);

void rsi_task_delete(rsi_task_handle_t *task_handle);

void rsi_start_os_scheduler(void);
/*  ---------- OS MEMORY MAPPING FUNCTIONS -------- */
void  *rsi_virtual_to_physical_address(void  *x);
void  *rsi_physical_to_virtual_address(void  *x);
#endif
