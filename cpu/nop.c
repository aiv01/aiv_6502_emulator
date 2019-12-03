#include "mos6502.h"

static int nop(mos6502_t *cpu)
{
    return 1;
}

void mos6502_register_nop(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xEA, nop);
}

#ifdef _TEST

static int test_nop(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xEA);
    int ticks = mos6502_tick(cpu);

    return ticks == 1 && cpu->flags == 0;
}

void test_mos6502_nop(mos6502_t *cpu)
{
    RUN_TEST(test_nop);
}

#endif