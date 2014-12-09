#ifndef   __basic_io_H__
#define   __basic_io_H__

#include <io.h>


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "system.h"
#include "sys/alt_irq.h"

//  for GPIO
#define inport(base)                                  IORD(base, 0) 
#define outport(base, data)                           IOWR(base, 0, data)
#define get_pio_dir(base)                             IORD(base, 1) 
#define set_pio_dir(base, data)                       IOWR(base, 1, data)
#define get_pio_irq_mask(base)                        IORD(base, 2) 
#define set_pio_irq_mask(base, data)                  IOWR(base, 2, data)
#define get_pio_edge_cap(base)                        IORD(base, 3) 
#define set_pio_edge_cap(base, data)                  IOWR(base, 3, data)

//  for SEG7 Display
#define seg7_show(base,data)                          IOWR(base, 0, data)

//  for Time Delay
#define msleep(msec)                                  usleep(1000*msec);
#define Sleep(sec)                                    msleep(1000*sec);

//  for Switch
#define SWITCH_PIO_NAME "/dev/switch_pio"
#define SWITCH_PIO_TYPE "altera_avalon_pio"
//#define SWITCH_PIO_BASE 0x00681090
//#define SWITCH_PIO_DO_TEST_BENCH_WIRING 1
//#define SWITCH_PIO_DRIVEN_SIM_VALUE 0x0000
#define SWITCH_PIO_HAS_TRI 0
#define SWITCH_PIO_HAS_OUT 0
#define SWITCH_PIO_HAS_IN 1
#define SWITCH_PIO_CAPTURE 0
#define SWITCH_PIO_EDGE_TYPE "NONE"
#define SWITCH_PIO_IRQ_TYPE "NONE"
//#define SWITCH_PIO_FREQ 50000000

//  for key
#define BUTTON_PIO_NAME "/dev/button_pio"
#define BUTTON_PIO_TYPE "altera_avalon_pio"
//#define BUTTON_PIO_BASE 0x00681080
//#define BUTTON_PIO_IRQ 2
//#define BUTTON_PIO_DO_TEST_BENCH_WIRING 1
//#define BUTTON_PIO_DRIVEN_SIM_VALUE 0x0000
#define BUTTON_PIO_HAS_TRI 0
#define BUTTON_PIO_HAS_OUT 0
#define BUTTON_PIO_HAS_IN 1
#define BUTTON_PIO_CAPTURE 1
#define BUTTON_PIO_EDGE_TYPE "FALLING"
#define BUTTON_PIO_IRQ_TYPE "EDGE"
//#define BUTTON_PIO_FREQ 50000000

#endif
