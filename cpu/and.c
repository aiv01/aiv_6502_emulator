#include "mos6502.h"


static int and_immediate(mos6502_t *cpu)
{
    uint8_t immediate = cpu->read(cpu, cpu->pc++);
    cpu->a=cpu->a & immediate;
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 2;
}

void mos6502_register_and(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x29, and_immediate);
}

#ifdef _TEST

static int test_and_immediate(mos6502_t *cpu)
{
    cpu->a = 0x55;
    mos6502_write8(cpu, 0x8000, 0x29);
    mos6502_write8(cpu, 0x8001, 0x29);
    int ticks = mos6502_tick(cpu);
    
    return cpu->a == 1 && ticks == 2 && cpu->flags == 0;
}

void test_mos6502_and()
{
    RUN_TEST(test_and_immediate);
}
#endif