#include "mos6502.h"

static int ror_accumulator(mos6502_t *cpu)
{
    uint8_t carry = cpu->flags & 1;
    uint8_t bit0 = cpu->a & 1;
    
    carry  = carry << 7;
    cpu->a = cpu-> a >> 1;

    cpu->a |= carry;
    
    mos6502_set_flag(cpu, NEGATIVE,cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO,cpu->a == 0); 
    mos6502_set_flag(cpu,CARRY, bit0);
    return 2;
}

static int ror_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    uint8_t data = mos6502_read8(cpu,(uint16_t)zp_address);
    uint8_t carry = cpu->flags & 1;
    uint8_t bit0 = data & 1;
    
    carry  = carry << 7;
    data = data >> 1;

    data |= carry;
    
    mos6502_write8(cpu,(uint16_t)zp_address,data);

    mos6502_set_flag(cpu, NEGATIVE,data & NEGATIVE);
    mos6502_set_flag(cpu, ZERO,data == 0); 
    mos6502_set_flag(cpu,CARRY, bit0);

    return 5;
}

static int ror_zero_page_x(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    zp_address=zp_address+cpu->x;
    uint8_t data = mos6502_read8(cpu,(uint16_t)zp_address);
    uint8_t carry = cpu->flags & 1;
    uint8_t bit0 = data & 1;
    
    carry  = carry << 7;
    data = data >> 1;

    data |= carry;
    
    mos6502_write8(cpu,(uint16_t)zp_address,data);

    mos6502_set_flag(cpu, NEGATIVE,data & NEGATIVE);
    mos6502_set_flag(cpu, ZERO,data == 0); 
    mos6502_set_flag(cpu,CARRY, bit0);

    return 6;
}

static int ror_absolute(mos6502_t *cpu)
{
    uint8_t addressL = cpu->read(cpu,cpu->pc++);
    uint8_t addressH = cpu->read(cpu,cpu->pc++);
    uint16_t address = ((uint16_t) addressH << 8) | (uint16_t)addressL;  

    uint8_t data = mos6502_read8(cpu,address);
    uint8_t carry = cpu->flags & 1;
    uint8_t bit0 = data & 1;
    
    carry  = carry << 7;
    data = data >> 1;

    data |= carry;
    
    mos6502_write8(cpu,address,data);

    mos6502_set_flag(cpu, NEGATIVE,data & NEGATIVE);
    mos6502_set_flag(cpu, ZERO,data == 0); 
    mos6502_set_flag(cpu,CARRY, bit0);

    return 6;
}

static int ror_absolute_x(mos6502_t *cpu)
{
    uint8_t addressL = cpu->read(cpu,cpu->pc++);
    uint8_t addressH = cpu->read(cpu,cpu->pc++);
    addressL=addressL+cpu->x;

    uint16_t address = ((uint16_t) addressH << 8) | (uint16_t)addressL; 
    
    uint8_t data = mos6502_read8(cpu,address);
    uint8_t carry = cpu->flags & 1;
    uint8_t bit0 = data & 1;
    
    carry  = carry << 7;
    data = data >> 1;

    data |= carry;
    
    mos6502_write8(cpu,address,data);

    mos6502_set_flag(cpu, NEGATIVE,data & NEGATIVE);
    mos6502_set_flag(cpu, ZERO,data == 0); 
    mos6502_set_flag(cpu,CARRY, bit0);

    return 7;
}

void mos6502_register_ror(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x6A, ror_accumulator);
    mos6502_register_opcode(cpu, 0x66, ror_zero_page);
    mos6502_register_opcode(cpu, 0x76, ror_zero_page_x);
    mos6502_register_opcode(cpu, 0x6E, ror_absolute);
    mos6502_register_opcode(cpu, 0x7E, ror_absolute_x);
}

#ifdef _TEST

static int test_ror_accumulator(mos6502_t *cpu)
{
    cpu->a = 0x03;

    mos6502_write8(cpu, 0x8000, 0x6A);

    int ticks = mos6502_tick(cpu);

    return ticks == 2 && cpu->a == 0x01 && cpu->pc == 0x8001 && cpu->flags == CARRY;
}

static int test_ror_zero_page(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0044, 0x03);
    
    mos6502_write8(cpu, 0x8000, 0x66);
    mos6502_write8(cpu, 0x8001, 0x44);

    int ticks = mos6502_tick(cpu);

    uint8_t data = mos6502_read8(cpu, 0x044);

    return ticks == 5 && data == 0x01 && cpu->pc == 0x8002 && cpu->flags == CARRY;
}

static int test_ror_zero_page_x(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0044, 0x03);
    
    cpu->x = 0x03;

    mos6502_write8(cpu, 0x8000, 0x76);
    mos6502_write8(cpu, 0x8001, 0x41);

    int ticks = mos6502_tick(cpu);

    uint8_t data = mos6502_read8(cpu, 0x0044);

    return ticks == 6 && data == 0x01 && cpu->pc == 0x8002 && cpu->flags == CARRY;
}

static int test_ror_absolute(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x2744, 0x03);
    
    mos6502_write8(cpu, 0x8000, 0x6E);
    mos6502_write8(cpu, 0x8001, 0x44);
    mos6502_write8(cpu, 0x8002, 0x27);

    int ticks = mos6502_tick(cpu);

    uint8_t data = mos6502_read8(cpu, 0x2744);

    return ticks == 6 && data == 0x01 && cpu->pc == 0x8003 && cpu->flags == CARRY;
}

static int test_ror_absolute_x(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x2744, 0x03);
    
    cpu->x = 0x03;

    mos6502_write8(cpu, 0x8000, 0x7E);
    mos6502_write8(cpu, 0x8001, 0x41);
    mos6502_write8(cpu, 0x8002, 0x27);

    int ticks = mos6502_tick(cpu);

    uint8_t data = mos6502_read8(cpu, 0x2744);

    return ticks == 7 && data == 0x01 && cpu->pc == 0x8003 && cpu->flags == CARRY;
}

void test_mos6502_ror()
{
    RUN_TEST(test_ror_accumulator);
    RUN_TEST(test_ror_zero_page);
    RUN_TEST(test_ror_zero_page_x);
    RUN_TEST(test_ror_absolute);
    RUN_TEST(test_ror_absolute_x);
}
#endif