#include "./kprint.h"

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8

// When DLAB setting is 0:
#define DATA_REGISTER(port) ((port) + 0)
#define INTERRUPT_ENABLE_REGISTER(port) ((port) + 1)

// When DLAB setting is 1:
#define DATA_REGISTER_LSB(port) ((port) + 0)
#define DATA_REGISTER_MSB(port) ((port) + 1)

// When DLAB setting is 0 or 1:
#define INTERUPT_IDENTIFICATION_REGISTER(port) ((port) + 2)
#define LINE_CONTROL_REGISTER(port) ((port) + 3)
#define MODEM_CONTROL_REGISTER(port) ((port) + 4)
#define LINE_STATUS_REGISTER(port) ((port) + 5)
#define MODEM_STATUS_REGISTER(port) ((port) + 6)
#define SCRATCH_REGISTER(port) ((port) + 7)

static int port;

// Configures serial for: 38400 8N1 serial communication
// Baud: 115200 / 3 = 38400, divisor = 3
// 8 Data bits, no parity bit, 1 stop bit.
void init_serial() {
  port = COM1;

  // Disable all interrupts
  port_byte_out(INTERRUPT_ENABLE_REGISTER(port), 0x00);
  // Start line protocol by specifying the dlab bit Enable dlab bit in preparation for setting baud divisor
  port_byte_out(LINE_CONTROL_REGISTER(port), 0x80);
  // Set baud rate
  // Set 38400 baud, divisor = 3
  port_byte_out(DATA_REGISTER_LSB(port), 0x03); // 3 low byte
  port_byte_out(DATA_REGISTER_MSB(port), 0x00); // 0 high byte
  // Set how many binary bits, parity, and stop bits
  port_byte_out(LINE_CONTROL_REGISTER(port), 0x03);
  // Enable FIFO
  port_byte_out(INTERUPT_IDENTIFICATION_REGISTER(port), 0xC7);
  port_byte_out(MODEM_CONTROL_REGISTER(port), 0x0B);
}

int is_transmit_empty() {
  return (port_word_in(LINE_STATUS_REGISTER(port)) & 0x20);
}

void write_serial_char(char c) {
  static int initialized = 0;

  if (!initialized) {
    init_serial();
    initialized = 1;
  }

  while(is_transmit_empty() == 0);

  port_byte_out(DATA_REGISTER(port), c);
}

void kprint_char(char c) {
  write_serial_char(c);
}

void kprintf(char *message) {
  char *ptr = message;
  while (*ptr) {
    kprint_char(*ptr);
    ptr++;
  }
}
