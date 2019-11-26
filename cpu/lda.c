#include "mos6502.h"

static int lda_immediate(mos6502_t *cpu)
{
    uint8_t immediate = cpu->read(cpu, cpu->pc++);
    cpu->a = immediate;
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 2;
}

static int lda_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    cpu->a = mos6502_read8(cpu, (uint16_t)zp_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 3;
}


void mos6502_register_lda(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xA9, lda_immediate);
    mos6502_register_opcode(cpu, 0xA5, lda_zero_page);
}

#ifdef _TEST

static int test_lda_immediate(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA9);
    mos6502_write8(cpu, 0x8001, 0x55);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0x55 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_lda_immediate_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA9);
    mos6502_write8(cpu, 0x8001, 0xFF);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0xFF && cpu->pc == 0x8002 && cpu->flags == NEGATIVE;
}

static int test_lda_immediate_zero(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA9);
    mos6502_write8(cpu, 0x8001, 0x00);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0x00 && cpu->pc == 0x8002 && cpu->flags == ZERO;
}



void test_mos6502_lda()
{
    RUN_TEST(test_lda_immediate);
    RUN_TEST(test_lda_immediate_negative);
    RUN_TEST(test_lda_immediate_zero);
}
#endif
