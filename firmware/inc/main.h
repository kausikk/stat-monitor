#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <commands.h>
#include <LUFA/Drivers/USB/USB.h>

#define DISPLAY_STATS_BMREQUEST 0x80
#define DISPLAY_STATS_BMREQUEST_TYPE (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE) // 0x21
#define READ_STATS_BMREQUEST 0x81
#define READ_STATS_BMREQUEST_TYPE (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE) // 0xA1

void SetupHardware(void);
void EVENT_USB_Device_ControlRequest(void);
void writeCommands(const uint8_t *cmd, int n);