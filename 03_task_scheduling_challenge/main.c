#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include "bsp.h"

#define DEFAULT_THREAD_STACK_SIZE (1024u)
#define INPUT_PRIO (6u)
#define LED_PRIO   (5u)

K_THREAD_STACK_DEFINE(_thread_in_stack,  DEFAULT_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(_thread_led_stack, DEFAULT_THREAD_STACK_SIZE);

static struct k_thread _thread_in, _thread_led;
static k_tid_t _thread_in_id = NULL, _thread_led_id = NULL;

int32_t _delayTimeMs = 1000;

static void _thread_in_enter(void *, void *, void *);
static void _thread_led_enter(void *, void *, void *);

int main(void)
{
	bsp_rgb_led_init();

	_thread_in_id = k_thread_create(
		&_thread_in,
		_thread_in_stack,
		K_THREAD_STACK_SIZEOF(_thread_in_stack),
		_thread_in_enter,
		NULL, NULL, NULL,
		INPUT_PRIO,
		0,
		K_NO_WAIT
	);

	_thread_led_id = k_thread_create(
		&_thread_led,
		_thread_led_stack,
		K_THREAD_STACK_SIZEOF(_thread_led_stack),
		_thread_led_enter,
		NULL, NULL, NULL,
		LED_PRIO,
		0,
		K_NO_WAIT
	);

	while (1) {
		k_msleep(10000);
	}

	return 0;
}

static void _thread_in_enter(void *dummy1, void *dummy2, void *dummy3) {
	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);
	char c;
	char buf[10];
	uint8_t bufIdx = 0;
	printf("Enter LED delay(ms):");
	while(1) {
		c = bsp_getchar();
		if(c == '\n' && bufIdx != 0) {
			buf[bufIdx] = '\0';
			bufIdx = 0;
			_delayTimeMs = atoi(buf);
			printf("\nLed delay updated to(ms): %d", _delayTimeMs);
			printf("\nEnter LED delay(ms):");
		} else 	if(c < '0' || c > '9') {

		} else {
			buf[bufIdx] = c;
			bufIdx++;
			putchar(c);
		}
	}
}
static void _thread_led_enter(void *dummy1, void *dummy2, void *dummy3) {
	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);
	while(1) {
		bsp_rgb_led_change();
		k_msleep(_delayTimeMs);
	}
}
