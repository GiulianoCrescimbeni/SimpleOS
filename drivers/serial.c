#include <drivers/io.h>

/* The I/O ports */

/* All the I/O ports are calculated relative to the data port. This is because
* all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
* order, but they start at different values.
*/

#define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/* The I/O port commands */

/* SERIAL_LINE_ENABLE_DLAB:
* Tells the serial port to expect first the highest 8 bits on the data port,
* then the lowest 8 bits will follow
*/
#define SERIAL_LINE_ENABLE_DLAB         0x80

/** serial_configure_baud_rate:
*  Sets the speed of the data being sent. The default speed of a serial
*  port is 115200 bits/s. The argument is a divisor of that number, hence
*  the resulting speed becomes (115200 / divisor) bits/s.
*
*  @param com      The COM port to configure
*  @param divisor  The divisor
*/
void serial_configure_baud_rate(unsigned short com, unsigned short divisor) {
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
    outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);
}

/** serial_configure_line:
*  Configures the line of the given serial port. The port is set to have a
*  data length of 8 bits, no parity bits, one stop bit and break control
*  disabled.
*
*  @param com  The serial port to configure
*/
void serial_configure_line(unsigned short com) {
    /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     * Content: | d | b | prty  | s | dl  |
     * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
    *
    * d:    Enables (d = 1) or disables (d = 0) DLAB (Divisor Latch Access Bit)
    * b:    If break control is enabled (b = 1) or disabled (b = 0)
    * prty: The number of parity bits to use
    * s:    The number of stop bits to use (s = 0 equals 1, s = 1 equals 1.5 or 2)
    * dl:   Describes the length of the data
    */
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
    /* 0x03: default cofiguration fir a lenght of 8 bit, no parity bit
    one stop bit and brake control disabled */
}

/** serial_is_transmit_fifo_empty:
*  Checks whether the transmit FIFO queue is empty or not for the given COM
*  port.
*
*  @param  com The COM port
*  @return 0 if the transmit FIFO queue is not empty
*          1 if the transmit FIFO queue is empty
*/
int serial_is_transmit_fifo_empty(unsigned int com) {
    /* 0x20 = 0010 0000 */
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
    // if bit 5 is 1, the transmit queue is empty
}

/** serial_write:
*  Writes a string to the serial port. The function waits until the
*  transmit FIFO queue is empty before writing each character.
*
*  @param string  The string to write
*  @param len     The length of the string
*  @return        The number of characters written
*/
int serial_write(char* string, int len) {
    serial_configure_baud_rate(SERIAL_COM1_BASE, 3); //38400 baud
    serial_configure_line(SERIAL_COM1_BASE);
    for (int i = 0; i < len; i++) {
        // waits for the transmit queue to be empty
        while (!serial_is_transmit_fifo_empty(SERIAL_COM1_BASE));

        // send the char
        outb(SERIAL_DATA_PORT(SERIAL_COM1_BASE), string[i]);
    }

    return len;
}