#include "mos6502.h"

static int tax_transfer(mos6502_t *cpu)
{
    cpu->x = cpu->a;
    return 2;
}


void mos6502_register_tax(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xAA, tax_transfer);
}

#ifdef _TEST

static int test_tax_transfer(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xAA);
    cpu->a = 0xA;
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->x == 0xA;
}


void test_mos6502_tax()
{
    RUN_TEST(test_tax_transfer);
}
#endif
