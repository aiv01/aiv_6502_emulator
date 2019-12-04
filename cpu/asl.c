#include "mos6502.h"

static int asl_accumulator(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, CARRY, cpu->a & 0x80);
    cpu->a <<= 1;
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 2;
}

static int asl_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    uint8_t value = mos6502_read8(cpu, (uint16_t)zp_address);
    cpu->a = value << 1;
    mos6502_set_flag(cpu, CARRY, value & 0x80);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 5;
}

static int asl_zero_page_X(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    uint8_t value = mos6502_read8(cpu, (uint16_t)(zp_address + cpu->x));
    cpu->a = value << 1;
    mos6502_set_flag(cpu, CARRY, value & 0x80);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 6;
}

static int asl_absolute(mos6502_t *cpu)
{
    uint8_t value = mos6502_read8(cpu, mos6502_read16(cpu, cpu->pc));
    cpu->pc += 2;
    cpu->a = value << 1;
    mos6502_set_flag(cpu, CARRY, value & 0x80);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 6;
}

static int asl_absolute_X(mos6502_t *cpu)
{
    uint16_t abs_address = mos6502_read16(cpu, cpu->pc) + (uint16_t)cpu->x;
    cpu->pc += 2;
    uint8_t value = mos6502_read8(cpu, abs_address);
    cpu->a = value << 1;
    mos6502_set_flag(cpu, CARRY, value & 0x80);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 7;
}

void mos6502_register_asl(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x0A, asl_accumulator);
    mos6502_register_opcode(cpu, 0x06, asl_zero_page);
    mos6502_register_opcode(cpu, 0x16, asl_zero_page_X);
    mos6502_register_opcode(cpu, 0x0E, asl_absolute);
    mos6502_register_opcode(cpu, 0x1E, asl_absolute_X);
}

#ifdef _TEST

static int test_asl_accumulator_carry(mos6502_t *cpu)
{
    cpu->a = 0b10100000; 
    mos6502_write8(cpu, 0x8000, 0x0A);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0b01000000 && cpu->pc == 0x8001 && cpu->flags == CARRY;
}

static int test_asl_accumulator_zero_and_carry(mos6502_t *cpu)
{
    cpu->a = 0b10000000;
    mos6502_write8(cpu, 0x8000, 0x0A);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0 && cpu->pc == 0x8001 && cpu->flags == (ZERO | CARRY);
}

static int test_asl_accumulator_negative(mos6502_t *cpu)
{
    cpu->a = 0b01000000;
    mos6502_write8(cpu, 0x8000, 0x0A);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0b10000000 && cpu->pc == 0x8001 && cpu->flags == NEGATIVE;
}

static int test_asl_accumulator_no_flags(mos6502_t *cpu)
{
    cpu->a = 0b00111111;
    mos6502_write8(cpu, 0x8000, 0x0A);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0b01111110 && cpu->pc == 0x8001 && cpu->flags == 0;
}

static int test_asl_zero_page_curry(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x01, 0b10000001); 
    mos6502_write8(cpu, 0x8000, 0x06);
    mos6502_write8(cpu, 0x8001, 0x01);
    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0b00000010 && cpu->pc == 0x8002 && cpu->flags == CARRY;
}

static int test_asl_zero_page_zeroflag(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x02, 0);
    mos6502_write8(cpu, 0x8000, 0x06);
    mos6502_write8(cpu, 0x8001, 0x02);
    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0 && cpu->pc == 0x8002 && cpu->flags == ZERO;
}

static int test_asl_zero_page_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x03, 0b01000110); 
    mos6502_write8(cpu, 0x8000, 0x06);
    mos6502_write8(cpu, 0x8001, 0x03);
    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0b10001100 && cpu->pc == 0x8002 && cpu->flags == NEGATIVE;
}

static int test_asl_zero_page_X_carry_and_negative(mos6502_t *cpu)
{
    cpu->x = 10;
    mos6502_write8(cpu, 0xf, 0b11111111); 
    mos6502_write8(cpu, 0x8000, 0x16);
    mos6502_write8(cpu, 0x8001, 0x05);
    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0b11111110 && cpu->pc == 0x8002 && cpu->flags == (CARRY | NEGATIVE);
}

static int test_asl_zero_page_X_zero_and_carry(mos6502_t *cpu)
{
    cpu->x = 1;
    mos6502_write8(cpu, 0xa, 0b10000000); 
    mos6502_write8(cpu, 0x8000, 0x16);
    mos6502_write8(cpu, 0x8001, 0x09);
    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0 && cpu->pc == 0x8002 && cpu->flags == (ZERO | CARRY);
}

static int test_asl_absolute_no_flags(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x4032, 0b00000001); 
    mos6502_write8(cpu, 0x8000, 0x0E);
    mos6502_write8(cpu, 0x8001, 0x32);  
    mos6502_write8(cpu, 0x8002, 0x40);  
    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0b00000010 && cpu->pc == 0x8003 && cpu->flags == 0;
}

static int test_asl_absolute_carry(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x3000, 0b10011111); 
    mos6502_write8(cpu, 0x8000, 0x0E);
    mos6502_write8(cpu, 0x8001, 0x00);  
    mos6502_write8(cpu, 0x8002, 0x30);  
    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0b00111110 && cpu->pc == 0x8003 && cpu->flags == CARRY;
}

static int test_asl_absolute_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0xAAAA, 0b01011111); 
    mos6502_write8(cpu, 0x8000, 0x0E);
    mos6502_write8(cpu, 0x8001, 0xAA);  
    mos6502_write8(cpu, 0x8002, 0xAA);  
    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0b10111110 && cpu->pc == 0x8003 && cpu->flags == NEGATIVE;
}

static int test_asl_absolute_X_zero_and_carry(mos6502_t *cpu)
{
    cpu->x= 1;
    mos6502_write8(cpu, 0x0006, 0b10000000); 
    mos6502_write8(cpu, 0x8000, 0x1E);
    mos6502_write8(cpu, 0x8001, 0x05);  
    mos6502_write8(cpu, 0x8002, 0x00);  
    int ticks = mos6502_tick(cpu);
    return ticks == 7 && cpu->a == 0 && cpu->pc == 0x8003 && cpu->flags == (ZERO | CARRY);
}

static int test_asl_absolute_X_negative(mos6502_t *cpu)
{
    cpu->x= 10;
    mos6502_write8(cpu, 0x000A, 0b01011111); 
    mos6502_write8(cpu, 0x8000, 0x1E);
    mos6502_write8(cpu, 0x8001, 0x00);  
    mos6502_write8(cpu, 0x8002, 0x00);  
    int ticks = mos6502_tick(cpu);
    return ticks == 7 && cpu->a == 0b10111110 && cpu->pc == 0x8003 && cpu->flags == NEGATIVE;
}

static int test_asl_absolute_X_no_flags(mos6502_t *cpu)
{
    cpu->x= 5;
    mos6502_write8(cpu, 0x010F, 0b00111111); 
    mos6502_write8(cpu, 0x8000, 0x1E);
    mos6502_write8(cpu, 0x8001, 0x0A);  
    mos6502_write8(cpu, 0x8002, 0x01);  
    int ticks = mos6502_tick(cpu);
    return ticks == 7 && cpu->a == 0b01111110 && cpu->pc == 0x8003 && cpu->flags == 0;
}

void test_mos6502_asl()
{
    RUN_TEST(test_asl_accumulator_carry);
    RUN_TEST(test_asl_accumulator_zero_and_carry);
    RUN_TEST(test_asl_accumulator_negative);
    RUN_TEST(test_asl_accumulator_no_flags);
    RUN_TEST(test_asl_zero_page_curry);
    RUN_TEST(test_asl_zero_page_zeroflag);
    RUN_TEST(test_asl_zero_page_negative);
    RUN_TEST(test_asl_zero_page_X_carry_and_negative);
    RUN_TEST(test_asl_zero_page_X_zero_and_carry);
    RUN_TEST(test_asl_absolute_no_flags);
    RUN_TEST(test_asl_absolute_carry);
    RUN_TEST(test_asl_absolute_negative);
    RUN_TEST(test_asl_absolute_X_zero_and_carry);
    RUN_TEST(test_asl_absolute_X_negative);
    RUN_TEST(test_asl_absolute_X_no_flags);
}
#endif