#include "main.h"

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
    MCUSR &= ~_BV(WDRF);
    wdt_disable();
    // Disable clock division
    clock_prescale_set(clock_div_1);

    // Set PORTB pin directions and values
    DDRB = _BV(DDB5) | _BV(DDB4) | _BV(DDB2) | _BV(DDB1) | _BV(DDB0);
    PORTB = _BV(RESET_PIN);
    // Initialize SPI
    SPCR = _BV(SPE) | _BV(MSTR);

    // Reset display
    PORTB &= ~_BV(RESET_PIN);
    _delay_ms(RESET_DELAY_MS);
    PORTB |= _BV(RESET_PIN);

    // Initialize display
    writeCommands(INIT_DISPLAY, sizeof(INIT_DISPLAY));

    // Draw background
    PORTB |= _BV(DC_PIN);
    writeCommands(BACKGROUND, sizeof(BACKGROUND));

    USB_Init();
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
    static uint8_t prevStats[FIXED_CONTROL_ENDPOINT_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
    switch (USB_ControlRequest.bRequest)
    {
        case READ_STATS_BMREQUEST:
            if (USB_ControlRequest.bmRequestType == READ_STATS_BMREQUEST_TYPE)
            {
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_Stream_LE(&prevStats, sizeof(prevStats));
                Endpoint_ClearIN();
                Endpoint_ClearStatusStage();
            }
            break;
        case DISPLAY_STATS_BMREQUEST:
            if (USB_ControlRequest.bmRequestType == DISPLAY_STATS_BMREQUEST_TYPE)
            {
                uint8_t stats[FIXED_CONTROL_ENDPOINT_SIZE];
                Endpoint_ClearSETUP();
                Endpoint_Read_Control_Stream_LE(&stats, sizeof(stats));
                Endpoint_ClearOUT();
                Endpoint_ClearStatusStage();

                // Setup display to draw top row
                PORTB &= ~_BV(DC_PIN);
                writeCommands(SET_DIGIT_ROW_TOP, sizeof(SET_DIGIT_ROW_TOP));

                const uint8_t *set_col_cmd = SET_DIGIT_COL;
                for (uint8_t i=0; i < NUM_STATS; i++) {
                    if (i == ROW_BOT_INDEX) {
                        // Setup display to draw bottom row
                        PORTB &= ~_BV(DC_PIN);
                        writeCommands(SET_DIGIT_ROW_BOT, sizeof(SET_DIGIT_ROW_BOT));
                    }
                    if (stats[i] != prevStats[i]) {
                        // Setup display to draw in tens column
                        PORTB &= ~_BV(DC_PIN);
                        writeCommands(set_col_cmd, SET_DIGIT_COL_LEN);
                        // Increment column
                        set_col_cmd += SET_DIGIT_COL_LEN;

                        // Draw tens digit
                        PORTB |= _BV(DC_PIN);
                        writeDigit(RECOVER_TENS_DIGIT(stats[i]));

                        // Setup display to draw in ones column
                        PORTB &= ~_BV(DC_PIN);
                        writeCommands(set_col_cmd, SET_DIGIT_COL_LEN);
                        // Increment column
                        set_col_cmd += SET_DIGIT_COL_LEN;

                        // Draw ones digit
                        PORTB |= _BV(DC_PIN);
                        writeDigit(RECOVER_ONES_DIGIT(stats[i]));
                        prevStats[i] = stats[i];
                    } else
                        set_col_cmd += SET_DIGIT_COL_LEN * 2;
                }
            }
            break;
    }
}

static void writeDigit(uint8_t digit) {
    // Lookup table for digit
    switch (digit) {
        case DIGIT_0:
            writeCommands(DIGITS, DIGIT_WIDTH);
            break;
        case DIGIT_1:
            writeCommands(DIGITS + (DIGIT_WIDTH * DIGIT_1), DIGIT_WIDTH);
            break;
        case DIGIT_2:
            writeCommands(DIGITS + (DIGIT_WIDTH * DIGIT_2), DIGIT_WIDTH);
            break;
        case DIGIT_3:
            writeCommands(DIGITS + (DIGIT_WIDTH * DIGIT_3), DIGIT_WIDTH);
            break;
        case DIGIT_4:
            writeCommands(DIGITS + (DIGIT_WIDTH * DIGIT_4), DIGIT_WIDTH);
            break;
        case DIGIT_5:
            writeCommands(DIGITS + (DIGIT_WIDTH * DIGIT_5), DIGIT_WIDTH);
            break;
        case DIGIT_6:
            writeCommands(DIGITS + (DIGIT_WIDTH * DIGIT_6), DIGIT_WIDTH);
            break;
        case DIGIT_7:
            writeCommands(DIGITS + (DIGIT_WIDTH * DIGIT_7), DIGIT_WIDTH);
            break;
        case DIGIT_8:
            writeCommands(DIGITS + (DIGIT_WIDTH * DIGIT_8), DIGIT_WIDTH);
            break;
        case DIGIT_9:
            writeCommands(DIGITS + (DIGIT_WIDTH * DIGIT_9), DIGIT_WIDTH);
            break;
        case DIGIT_BLANK:
            writeCommands(DIGITS + (DIGIT_WIDTH * DIGIT_BLANK), DIGIT_WIDTH);
            break;
    }
}

static void writeCommands(const uint8_t *cmd, int n) {
    // Read commands from PROGMEM and write to SPI
    while(n--) {
        SPDR = pgm_read_byte_near(cmd++);
        while (!(SPSR & (1 << SPIF)));
    }
}