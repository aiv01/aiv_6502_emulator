#include "mos6502.h"

static int tya(mos6502_t *cpu)
{
    cpu->a = cpu->y;
    cpu->y = 0;
    return 2;
}

void mos6502_register_tya(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x98, tya);
}

#ifdef _TEST

static int test_tya(mos6502_t *cpu)
{
    cpu->y = 3;
    mos6502_write8(cpu, 0x8000, 0x98);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 3 && cpu->y == 0 && cpu->pc == 0x8001;
}

void test_mos6502_tya()
{
    RUN_TEST(test_tya);
}

#endif