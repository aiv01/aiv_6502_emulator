#include "mos6502.h"

static int sta_absolute(mos6502_t *cpu)
{
    uint16_t address = mos6502_read16(cpu, cpu->pc);
    cpu->pc+=2;
    cpu->a = mos6502_read16(cpu, address);

    return 4;
}

void mos6502_register_sta(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x8D, sta_absolute);
}

#ifdef _TEST

static int test_sta_absolute(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x8D);
    mos6502_write8(cpu, 0x8001, 0x00);
    mos6502_write8(cpu, 0x8002, 0x44);
    mos6502_write8(cpu, 0x4400, 0xA);
    int ticks = mos6502_tick(cpu);

    return ticks == 4 && cpu->a == 0xA && cpu->pc == 0x8003 && cpu->flags == 0;
}

void test_mos6502_sta()
{
    RUN_TEST(test_sta_absolute);
}
#endif