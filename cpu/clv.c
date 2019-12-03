#include "mos6502.h"

static int clv(mos6502_t *cpu)
{  
    mos6502_set_flag(cpu, OVERFLOW, 0);
    return 2;
}

void mos6502_register_clv(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xB8, clv);
}

#ifdef _TEST

static int test_clv(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xB8);
    mos6502_set_flag(cpu, OVERFLOW, 1);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->flags == 0;
}

void test_mos6502_clv()
{
    RUN_TEST(test_clv);
}
#endif