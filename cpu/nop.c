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

    return ticks == 1 && cpu->flags == 0 && cpu->pc == 0x8001;
}

static int test_nop_flags(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, CARRY, 1);
    mos6502_set_flag(cpu, ZERO, 1);
    mos6502_write8(cpu, 0x8000, 0xEA);
    int ticks = mos6502_tick(cpu);

    return ticks == 1 && cpu->flags == (CARRY | ZERO) && cpu->pc == 0x8001;
}

static int test_nop_registers(mos6502_t *cpu)
{
    cpu->a = 0xFA;
    cpu->x = 0xFB;
    cpu->y = 0xFC;
    mos6502_write8(cpu, 0x8000, 0xEA);
    int ticks = mos6502_tick(cpu);

    return ticks == 1 && cpu->flags == 0 && cpu->a == 0xFA && cpu->x == 0xFB && cpu->y == 0xFC && cpu->pc == 0x8001;
}

void test_mos6502_nop(mos6502_t *cpu)
{
    RUN_TEST(test_nop);
    RUN_TEST(test_nop_flags);
    RUN_TEST(test_nop_registers);
}

#endif