#include "mos6502.h"

static int ror_accumulator(mos6502_t *cpu)
{
    uint8_t carry = cpu->flags & 1;
    uint8_t bit0 = cpu->a & 1;
    
    carry  = carry << 7;
    cpu->a = cpu-> a >> 1;

    cpu->a |= carry;
    
    mos6502_set_flag(cpu, NEGATIVE,cpu->a & NEGATIVE); //cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO,cpu->a == 0); // cpu->a == 0);
    mos6502_set_flag(cpu,CARRY, bit0);
    return 2;
}

void mos6502_register_ror(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x6A, ror_accumulator);
}

#ifdef _TEST

static int test_ror_accumulator(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA9);
    mos6502_write8(cpu, 0x8001, 0x03);
    int ticks = mos6502_tick(cpu);
    
    mos6502_write8(cpu, 0x8002, 0x6A);

    ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0x01 && cpu->pc == 0x8003 && cpu->flags == CARRY;
}


void test_mos6502_ror()
{
    RUN_TEST(test_ror_accumulator);
}
#endif