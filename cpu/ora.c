#include "mos6502.h"

static int ora_immediate(mos6502_t *cpu)
{
    uint8_t immediate = cpu->read(cpu, cpu->pc++);
    cpu->a = cpu->a | immediate;
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 2;
}


static int ora_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    cpu->a = cpu->a | mos6502_read8(cpu, (uint16_t)zp_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 3;
}


void mos6502_register_ora(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x09, ora_immediate);
    mos6502_register_opcode(cpu, 0x05, ora_zero_page);
}


#ifdef _TEST

static int test_ora_immediate(mos6502_t *cpu)
{
    cpu->a = 0x15;
    mos6502_write8(cpu, 0x8000, 0x09);
    mos6502_write8(cpu, 0x8001, 0x10);
    int ticks = mos6502_tick(cpu);
    
    printf("ticks = %d", ticks);
    return ticks == 2 && cpu->a == 0x15 && cpu->pc == 0x8002 && cpu->flags == 0;
}


void test_mos6502_ora()
{
    RUN_TEST(test_ora_immediate);
}
#endif
