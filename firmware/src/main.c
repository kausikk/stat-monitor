#include "main.h"

static uint8_t prevReport[FIXED_CONTROL_ENDPOINT_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};

int main(void)
{
    SetupHardware();
    GlobalInterruptEnable();
    for (;;)
        USB_USBTask();
}

void SetupHardware(void)
{
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
    /* Disable clock division */
    clock_prescale_set(clock_div_1);

    // Initialize SPI
    DDRB = 0b00110111;
    PORTB = (1 << 4);
    SPCR = 0b01010000;

    // Reset display
    PORTB &= ~(1 << 4);
    _delay_ms(10);
    PORTB |= (1 << 4);

    // Initialize display
    writeCommands(INIT_DISPLAY, sizeof(INIT_DISPLAY));
    writeCommands(SETUP_FULL_DRAW, sizeof(SETUP_FULL_DRAW));

    // Draw background
    PORTB |= (1 << 5);
    writeCommands(BACKGROUND, sizeof(BACKGROUND));

    USB_Init();
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
    switch (USB_ControlRequest.bRequest)
    {
        case READ_STATS_BMREQUEST:
            if (USB_ControlRequest.bmRequestType == READ_STATS_BMREQUEST_TYPE)
            {
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_Stream_LE(&prevReport, sizeof(prevReport));
                Endpoint_ClearIN();
                Endpoint_ClearStatusStage();
            }
            break;
        case DISPLAY_STATS_BMREQUEST:
            if (USB_ControlRequest.bmRequestType == DISPLAY_STATS_BMREQUEST_TYPE)
            {
                Endpoint_ClearSETUP();
                Endpoint_Read_Control_Stream_LE(&prevReport, sizeof(prevReport));
                Endpoint_ClearOUT();
                Endpoint_ClearStatusStage();
            }
            break;
    }
}

void writeCommands(const uint8_t *cmd, int n) {
    while(n--) {
        SPDR = pgm_read_byte_near(cmd++);
        while (!(SPSR & (1 << SPIF)));
    }
}