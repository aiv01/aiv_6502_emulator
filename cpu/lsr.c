#include "mos6502.h"

static int lsr_accumulator(mos6502_t *cpu)
{
    // read the cpu a reg value and store it in a var
    uint8_t accumulator = cpu->read(cpu, cpu->a);
    // check bit 0 of the accumulator val and store it to be used as carry
    uint8_t carry = accumulator & 1;
    // load accumulator shifted value into a reg
    cpu->a = accumulator >> 1; // this should also set bit 7 to 0
    mos6502_set_flag(cpu, CARRY, carry);    // set carry as bit 0
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 1;
}

static int lsr_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    cpu->a = mos6502_read8(cpu, (uint16_t)zp_address);
    // read the cpu a reg value and store it in a var
    uint8_t accumulator = cpu->read(cpu, cpu->a);
    // check bit 0 of the accumulator val and store it to be used as carry
    uint8_t carry = accumulator & 1;
    // load accumulator shifted value into a reg
    cpu->a = accumulator >> 1; // this should also set bit 7 to 0
    mos6502_set_flag(cpu, CARRY, carry); // set carry as bit 0
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 2;
}

void mos6502_register_lsr(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x4A, lsr_accumulator);
    mos6502_register_opcode(cpu, 0x46, lsr_zero_page);
}

#ifdef _TEST

static int test_lsr_accumulator(mos6502_t *cpu)
{
    cpu->write(cpu, cpu->a, 0x02);
    mos6502_write8(cpu, 0x8000, 0x4A);
    int ticks = mos6502_tick(cpu);
    return cpu->a == 1 && ticks == 1 && cpu->pc == 0x8001 && cpu->flags == 0;
}

static int test_lsr_accumulator_shift_zero(mos6502_t *cpu)
{
    cpu->write(cpu, cpu->a, 0x00);
    mos6502_write8(cpu, 0x8000, 0x4A);
    int ticks = mos6502_tick(cpu);
    return cpu->a == 0 && ticks == 1 && cpu->pc == 0x8001 && cpu->flags == ZERO;
}

static int test_lsr_accumulator_not_negative(mos6502_t *cpu)
{
    cpu->write(cpu, cpu->a, 0xFF);
    mos6502_write8(cpu, 0x8000, 0x4A);
    int ticks = mos6502_tick(cpu);
    return cpu->a == 127 && ticks == 1 && cpu->pc == 0x8001 && cpu->flags == CARRY;
}

static int test_lsr_accumulator_zero(mos6502_t *cpu)
{
    cpu->write(cpu, cpu->a, 0x01);
    mos6502_write8(cpu, 0x8000, 0x4A);
    int ticks = mos6502_tick(cpu);
    return cpu->a == 0 && ticks == 1 && cpu->pc == 0x8001 && cpu->flags == (CARRY | ZERO);
}

static int test_lsr_accumulator_carry(mos6502_t *cpu)
{
    cpu->write(cpu, cpu->a, 0x03);
    mos6502_write8(cpu, 0x8000, 0x4A);
    int ticks = mos6502_tick(cpu);
    return cpu->a == 1 && ticks == 1 && cpu->pc == 0x8001 && cpu->flags == CARRY;
}

void test_mos6502_lsr()
{
    RUN_TEST(test_lsr_accumulator);
    RUN_TEST(test_lsr_accumulator_shift_zero);
    RUN_TEST(test_lsr_accumulator_not_negative);
    RUN_TEST(test_lsr_accumulator_zero);
    RUN_TEST(test_lsr_accumulator_carry);
}
#endif