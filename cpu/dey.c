#include "mos6502.h"

static int dey(mos6502_t *cpu)
{
    cpu->y--;
    mos6502_set_flag(cpu, NEGATIVE, cpu->y & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->y == 0);
    return 2;
}

void mos6502_register_dey(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x88, dey);
}

#ifdef _TEST

static int test_dey(mos6502_t *cpu)
{
    cpu -> y= 0x04;
    mos6502_write8(cpu, 0x8000, 0x88);     
    int ticks = mos6502_tick(cpu);

    return  cpu-> y == 0x03 && ticks == 2;
}

void test_mos6502_dey()
{
    RUN_TEST(test_dey);
}
#endif