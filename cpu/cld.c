#include "mos6502.h"

static int cld(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, DECIMAL, 0);
    return 2;
}

void mos6502_register_cld(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xD8, cld);
}

#ifdef _TEST

static int test_cld(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xD8);
    int ticks = mos6502_tick(cpu);

    return ticks == 2 && mos6502_get_flag(cpu, DECIMAL) == 0;
}

void test_mos6502_cld(mos6502_t *cpu)
{
    RUN_TEST(test_cld);
}

#endif