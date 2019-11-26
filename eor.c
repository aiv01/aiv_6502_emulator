#include "mos6502.h"

static int eor_immediate(mos6502_t *cpu)
{
    uint8_t immediate = cpu->read(cpu, cpu->pc++);
    cpu->a ^= immediate;
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 2;
}

static int eor_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    cpu->a ^= mos6502_read8(cpu, (uint16_t)zp_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 3;
}

static int eor_zero_page_x(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    uint8_t x_val = cpu->x;
    cpu->a ^= mos6502_read8(cpu, (uint16_t)(zp_address + x_val));
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 4;
}

static int eor_absolute(mos6502_t *cpu)
{
    uint8_t abs_low_address = cpu->read(cpu, cpu->pc++);
    uint8_t abs_high_address = cpu->read(cpu, cpu->pc++);
    
    uint16_t abs_address = (((uint16_t)abs_high_address) << 8) | (uint16_t)abs_low_address;
    
    cpu->a ^= mos6502_read8(cpu, (uint16_t)abs_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 4;
}

static int eor_absolute_x(mos6502_t *cpu)
{
    uint8_t low_address = cpu->read(cpu, cpu->pc++);
    uint16_t high_address = (uint16_t)(cpu->read(cpu, cpu->pc++)) << 8;
    
    uint8_t x_val = cpu->x;
    uint16_t abs_address = high_address | (uint16_t)(low_address);

    abs_address += (uint16_t)x_val;
    cpu->a ^= mos6502_read8(cpu, (uint16_t)abs_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    
    uint16_t abs_address_high = abs_address & 255 << 8;
    if (abs_address_high > high_address)
    {
        return 5;
    }
    
    return 4;
}

static int eor_absolute_y(mos6502_t *cpu)
{
    uint8_t low_address = cpu->read(cpu, cpu->pc++);
    uint16_t high_address = (uint16_t)(cpu->read(cpu, cpu->pc++)) << 8;
    
    uint8_t y_val = cpu->y;
    uint16_t abs_address = ((uint16_t)high_address) | (uint16_t)(low_address);

    abs_address += (uint16_t)y_val;
    cpu->a ^= mos6502_read8(cpu, (uint16_t)abs_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    
    uint16_t abs_address_high = abs_address & 255 << 8;
    if (abs_address_high > high_address)
    {
        return 5;
    }
    
    return 4;
}

static int eor_indirect_x(mos6502_t *cpu)
{
    uint8_t indirect_address = cpu->read(cpu, cpu->pc++);
    
    uint8_t x_val = cpu->x;
    indirect_address += indirect_address + x_val;

    uint16_t data_address = mos6502_read16(cpu, (uint16_t)indirect_address);
    cpu->a ^= mos6502_read8(cpu, data_address);

    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    
    return 6;
}

static int eor_indirect_y(mos6502_t *cpu)
{
    uint8_t indirect_address = cpu->read(cpu, cpu->pc++);
    
    uint16_t data_address = mos6502_read16(cpu, (uint16_t)indirect_address);
    uint16_t high_address = data_address << 8;

    uint8_t y_val = cpu->y;
    data_address += data_address + y_val;
    cpu->a ^= mos6502_read8(cpu, data_address);

    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    
    uint16_t high_data_address = data_address & 255 << 8;
    if (high_data_address > high_address)
    {
        return 6;
    }
    
    return 5;
}

void mos6502_register_eor(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x49, eor_immediate);
}


#ifdef _TEST

static int test_eor_immediate(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x49);
    mos6502_write8(cpu, 0x8001, 0x55);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0x55 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_eor_immediate_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x49);
    mos6502_write8(cpu, 0x8001, 0xFF);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0xFF && cpu->pc == 0x8002 && cpu->flags == NEGATIVE;
}

static int test_eor_immediate_zero(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x49);
    mos6502_write8(cpu, 0x8001, 0x00);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0x00 && cpu->pc == 0x8002 && cpu->flags == ZERO;
}

void test_mos6502_eor()
{
    RUN_TEST(test_eor_immediate);
    RUN_TEST(test_eor_immediate_negative);
    RUN_TEST(test_eor_immediate_zero);
}
#endif
