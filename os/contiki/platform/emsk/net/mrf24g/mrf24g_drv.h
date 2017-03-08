/*
 * Copyright (c) 2014, Swedish Institute of Computer Science.
 *
 * Copyright (c) 2017, Synopsys, Inc.
 * Modified for port to ARC processor by Wayne Ren wei.ren@synopsys.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file mrf24g_drv.h
 * \brief mrf24g wifi driver head file for contiki(ipv4)
 */

#ifndef _MRF24G_DRV_H_
#define _MRF24G_DRV_H_

#include "contiki.h"

#ifdef __cplusplus
extern "C" {
#endif

PROCESS_NAME(mrf24g_process);
extern process_event_t wnic_event;

extern uint8_t mrf24g_output(void);
extern int get_network_up_status(void);
extern void uip_dump_ipstat(void);

#if UIP_CONF_IPV6_IP64
#include "ip64-driver.h"

extern const struct ip64_driver mrf24g_ip64_driver;
#endif

#ifdef __cplusplus
}
#endif

#endif	/* _MRF24G_DRV_H_ */
