#include "utilities.h"

typedef union {
    uint16_t a;
    uint8_t b;
}endian_t;

bool is_little_endian(void)
{
  endian_t test;
  test.a = 0x56;
  return (test.b == 0x56);
}