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
    // Disable watchdog if enabled by bootloader/fuses
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
    // Disable clock division
    clock_prescale_set(clock_div_1);

    // Set PORTB pins and initialize SPI
    DDRB = _BV(DDB5) | _BV(DDB4) | _BV(DDB2) | _BV(DDB1) | _BV(DDB0);
    PORTB = _BV(RESET_PIN);
    SPCR = _BV(SPE) | _BV(MSTR);

    // Reset display
    PORTB &= ~_BV(RESET_PIN);
    _delay_ms(RESET_DELAY_MS);
    PORTB |= _BV(RESET_PIN);

    // Initialize display
    writeCommands(INIT_DISPLAY, sizeof(INIT_DISPLAY));
    writeCommands(SETUP_FULL_DRAW, sizeof(SETUP_FULL_DRAW));

    // Clear background
    PORTB |= _BV(DC_PIN);
    writeCommands(BACKGROUND, sizeof(BACKGROUND))

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
    while(n--)
        transfer(pgm_read_byte_near(cmd++));
}

inline static void transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
}