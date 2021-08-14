#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "commands.h"
#include "LUFA/Drivers/USB/USB.h"

#define DISPLAY_STATS_BMREQUEST 0x80
#define DISPLAY_STATS_BMREQUEST_TYPE (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE) // 0x21
#define READ_STATS_BMREQUEST 0x81
#define READ_STATS_BMREQUEST_TYPE (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE) // 0xA1

#define NUM_STATS 6
#define ROW_BOT_INDEX 3
#define RESET_DELAY_MS 5
#define RESET_PIN PORTB4
#define DC_PIN PORTB5

#define RECOVER_TENS_DIGIT(d) (d >> 4)
#define RECOVER_ONES_DIGIT(d) (d & 0x0F)

void SetupHardware(void);
void EVENT_USB_Device_ControlRequest(void);
static void writeCommands(const uint8_t *cmd, int n);
static void writeDigit(uint8_t digit);