#include "mos6502.h"

static int Dex(mos6502_t *cpu)
{
    cpu->x--;
    mos6502_set_flag(cpu, NEGATIVE, cpu->x & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->x == 0);
    return 2;
}

void mos6502_register_dex(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xCA, Dex);
}

#ifdef _TEST

static int test_Dex(mos6502_t *cpu)
{
    cpu -> x= 0x04;
    mos6502_write8(cpu, 0x8000, 0xCA);     
    int ticks = mos6502_tick(cpu);

    return  cpu-> x == 0x03 && ticks == 2;
}

void test_mos6502_dex()
{
    RUN_TEST(test_Dex);
}
#endif