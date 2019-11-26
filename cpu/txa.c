#include "mos6502.h"

static int txa(mos6502_t *cpu)
{
    cpu->a = cpu->x;
    cpu->x = 0;
    return 2;
}

void mos6502_register_txa(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x8A, txa);
}

#ifdef _TEST

static int test_txa(mos6502_t *cpu)
{
    cpu->x = 3;
    mos6502_write8(cpu, 0x8000, 0x8A);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 3 && cpu->x == 0 && cpu->pc == 0x8001;
}

void test_mos6502_txa()
{
    RUN_TEST(test_txa);
}

#endif