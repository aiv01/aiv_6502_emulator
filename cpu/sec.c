#include "mos6502.h"

static int sec(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, CARRY, 1);
    return 2;
}

void mos6502_register_sec(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x38, sec);
}

#ifdef _TEST

static int test_sec(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, CARRY, 0);
    mos6502_write8(cpu, 0x8000, 0x38);
    int ticks = mos6502_tick(cpu);

    return ticks == 2 && mos6502_get_flag(cpu, CARRY) == 1 && cpu->pc == 0x8001;
}

void test_mos6502_sec(mos6502_t *cpu)
{
    RUN_TEST(test_sec);
}

#endif