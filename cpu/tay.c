#include "mos6502.h"

static int tay_transfer(mos6502_t *cpu)
{
    cpu->y = cpu->a;
    return 2;
}


void mos6502_register_tay(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xA8, tay_transfer);
}

#ifdef _TEST

static int test_tay_transfer(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA8);
    cpu->a = 0xA;
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->y == 0xA;
}


void test_mos6502_tay()
{
    RUN_TEST(test_tay_transfer);
}
#endif
