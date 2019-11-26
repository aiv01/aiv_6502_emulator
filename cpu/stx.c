#include "mos6502.h"


static int stx_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
  
    mos6502_write8(cpu, (uint16_t)zp_address, (uint8_t) cpu -> x);
    return 3;
}


void mos6502_register_stx(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x86, stx_zero_page);
}

#ifdef _TEST

static int test_stx_zero(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x86);
   
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->x == 0x00 && cpu->pc == 0x8002 && cpu->flags == ZERO;
}

#endif