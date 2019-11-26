#include "mos6502.h"

static int bpl(mos6502_t *cpu)
{
    int out = 0;
    int8_t distance = mos6502_read8(cpu, (uint16_t)cpu->pc);
    int16_t initial_page = cpu->pc;
    int flag = mos6502_get_flag(cpu, CARRY);
    if (!flag)
    {
        cpu->pc++;
        out = 2;
    }
    else
    {
        cpu->pc += distance;
        initial_page = initial_page >> 8;
        initial_page = initial_page & 0xFF;
        int16_t final_page = cpu->pc;
        final_page = final_page >> 8;
        final_page = final_page & 0xFF;
        if (initial_page == final_page)
            out = 3;
        else            
            out = 4;
    }
    return out;
}

void mos6502_register_bpl(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x10, bpl);
}

#ifdef _TEST

static int test_bpl_branch(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, CARRY, 0x1);
    mos6502_write8(cpu, 0x8000, 0x10);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->flags == 1;
}

void test_mos6502_bpl()
{
    RUN_TEST(test_bpl_branch);
}
#endif