#include "mos6502.h"

static int eor_immediate(mos6502_t *cpu)
{
    uint8_t immediate = mos6502_read8(cpu, cpu->pc++);
    cpu->a ^= immediate;
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 2;
}

static int eor_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = mos6502_read8(cpu, cpu->pc++);
    cpu->a ^= mos6502_read8(cpu, (uint16_t)zp_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 3;
}

static int eor_zero_page_x(mos6502_t *cpu)
{
    uint8_t zp_address = mos6502_read8(cpu, cpu->pc++);
    uint8_t x_val = cpu->x;
    cpu->a ^= mos6502_read8(cpu, (uint16_t)(zp_address + x_val));
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 4;
}

static int eor_absolute(mos6502_t *cpu)
{
    uint8_t abs_low_address = mos6502_read8(cpu, cpu->pc++);
    uint8_t abs_high_address = mos6502_read8(cpu, cpu->pc++);
    
    uint16_t abs_address = (((uint16_t)abs_high_address) << 8) | (uint16_t)abs_low_address;
    
    cpu->a ^= mos6502_read8(cpu, (uint16_t)abs_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 4;
}

static int eor_absolute_x(mos6502_t *cpu)
{
    uint8_t low_address = mos6502_read8(cpu, cpu->pc++);
    uint16_t high_address = (uint16_t)mos6502_read8(cpu, cpu->pc++) << 8;
    
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
    uint8_t low_address = mos6502_read8(cpu, cpu->pc++);
    uint16_t high_address = (uint16_t)mos6502_read8(cpu, cpu->pc++) << 8;
    
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
    uint8_t indirect_address = mos6502_read8(cpu, cpu->pc++);
    
    uint8_t x_val = cpu->x;
    indirect_address += x_val;
    
    uint16_t data_address = mos6502_read16(cpu, (uint16_t)indirect_address);
    cpu->a ^= mos6502_read8(cpu, data_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    
    return 6;
}

static int eor_indirect_y(mos6502_t *cpu)
{
    uint8_t indirect_address = mos6502_read8(cpu, cpu->pc++);
    
    uint16_t data_address = mos6502_read16(cpu, (uint16_t)indirect_address);
    uint16_t high_address = data_address & 255 << 8;

    uint8_t y_val = cpu->y;
    data_address += y_val;
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
    mos6502_register_opcode(cpu, 0x45, eor_zero_page);
    mos6502_register_opcode(cpu, 0x55, eor_zero_page_x);
    mos6502_register_opcode(cpu, 0x4D, eor_absolute);
    mos6502_register_opcode(cpu, 0x5D, eor_absolute_x);
    mos6502_register_opcode(cpu, 0x59, eor_absolute_y);
    mos6502_register_opcode(cpu, 0x41, eor_indirect_x);
    mos6502_register_opcode(cpu, 0x51, eor_indirect_y);
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

static int test_eor_zero_page(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0055, 0x27);
    mos6502_write8(cpu, 0x8000, 0x45);
    mos6502_write8(cpu, 0x8001, 0x55);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->a == 0x27 && cpu->flags == 0 && cpu->pc == 0x8002;
}

static int test_eor_zero_page_zero(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0055, 0x00);
    mos6502_write8(cpu, 0x8000, 0x45);
    mos6502_write8(cpu, 0x8001, 0x55);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->a == 0x00 && cpu->flags == ZERO && cpu->pc == 0x8002;
}

static int test_eor_zero_page_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0055, 0xFF);
    mos6502_write8(cpu, 0x8000, 0x45);
    mos6502_write8(cpu, 0x8001, 0x55);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->a == 0xFF && cpu->flags == NEGATIVE && cpu->pc == 0x8002;
}

static int test_eor_zero_page_x(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0057, 0x32);
    cpu->x = 2;

    mos6502_write8(cpu, 0x8000, 0x55);
    mos6502_write8(cpu, 0x8001, 0x55);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0x32 && cpu->flags == 0 && cpu->pc == 0x8002;
}

static int test_eor_zero_page_x_zero(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0057, 0x00);
    cpu->x = 2;

    mos6502_write8(cpu, 0x8000, 0x55);
    mos6502_write8(cpu, 0x8001, 0x55);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0x00 && cpu->flags == ZERO && cpu->pc == 0x8002;
}

static int test_eor_zero_page_x_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0057, 0xFF);
    cpu->x = 2;

    mos6502_write8(cpu, 0x8000, 0x55);
    mos6502_write8(cpu, 0x8001, 0x55);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0xFF && cpu->flags == NEGATIVE && cpu->pc == 0x8002;
}

static int test_eor_absolute(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x4457, 0x27);

    mos6502_write8(cpu, 0x8000, 0x4D);
    mos6502_write8(cpu, 0x8001, 0x57);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0x27 && cpu->flags == 0 && cpu->pc == 0x8003;
}

static int test_eor_absolute_zero(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x4457, 0x00);

    mos6502_write8(cpu, 0x8000, 0x4D);
    mos6502_write8(cpu, 0x8001, 0x57);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0x00 && cpu->flags == ZERO && cpu->pc == 0x8003;
}

static int test_eor_absolute_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x4457, 0xFF);

    mos6502_write8(cpu, 0x8000, 0x4D);
    mos6502_write8(cpu, 0x8001, 0x57);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0xFF && cpu->flags == NEGATIVE && cpu->pc == 0x8003;
}

static int test_eor_absolute_x(mos6502_t *cpu)
{
    cpu->x = 2;
    mos6502_write8(cpu, 0x4459, 0x27);

    mos6502_write8(cpu, 0x8000, 0x5D);
    mos6502_write8(cpu, 0x8001, 0x57);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0x27 && cpu->flags == 0 && cpu->pc == 0x8003;
}

static int test_eor_absolute_x_page_boundary(mos6502_t *cpu)
{
    cpu->x = 2;
    mos6502_write8(cpu, 0x4501, 0x27);

    mos6502_write8(cpu, 0x8000, 0x5D);
    mos6502_write8(cpu, 0x8001, 0xFF);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0x27 && cpu->flags == 0 && cpu->pc == 0x8003;
}

static int test_eor_absolute_x_zero(mos6502_t *cpu)
{
    cpu->x = 2;
    mos6502_write8(cpu, 0x4459, 0x00);

    mos6502_write8(cpu, 0x8000, 0x5D);
    mos6502_write8(cpu, 0x8001, 0x57);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0x00 && cpu->flags == ZERO && cpu->pc == 0x8003;
}

static int test_eor_absolute_x_negative(mos6502_t *cpu)
{
    cpu->x = 2;
    mos6502_write8(cpu, 0x4459, 0xFF);

    mos6502_write8(cpu, 0x8000, 0x5D);
    mos6502_write8(cpu, 0x8001, 0x57);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0xFF && cpu->flags == NEGATIVE && cpu->pc == 0x8003;
}

static int test_eor_absolute_y(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x4459, 0x27);

    mos6502_write8(cpu, 0x8000, 0x59);
    mos6502_write8(cpu, 0x8001, 0x57);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0x27 && cpu->flags == 0 && cpu->pc == 0x8003;
}

static int test_eor_absolute_y_page_boundary(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x4501, 0x27);

    mos6502_write8(cpu, 0x8000, 0x59);
    mos6502_write8(cpu, 0x8001, 0xFF);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0x27 && cpu->flags == 0 && cpu->pc == 0x8003;
}

static int test_eor_absolute_y_zero(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x4459, 0x00);

    mos6502_write8(cpu, 0x8000, 0x59);
    mos6502_write8(cpu, 0x8001, 0x57);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0x00 && cpu->flags == ZERO && cpu->pc == 0x8003;
}

static int test_eor_absolute_y_negative(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x4459, 0xFF);

    mos6502_write8(cpu, 0x8000, 0x59);
    mos6502_write8(cpu, 0x8001, 0x57);
    mos6502_write8(cpu, 0x8002, 0x44);

    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->a == 0xFF && cpu->flags == NEGATIVE && cpu->pc == 0x8003;
}

static int test_eor_indirect_x(mos6502_t *cpu)
{
    cpu->x = 2;
    mos6502_write8(cpu, 0x0059, 0x01);
    mos6502_write8(cpu, 0x005A, 0x44);

    mos6502_write8(cpu, 0x4401, 0x27);

    mos6502_write8(cpu, 0x8000, 0x41);
    mos6502_write8(cpu, 0x8001, 0x57);

    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0x27 && cpu->flags == 0 && cpu->pc == 0x8002;
}

static int test_eor_indirect_x_zero(mos6502_t *cpu)
{
    cpu->x = 2;
    mos6502_write8(cpu, 0x0059, 0x01);
    mos6502_write8(cpu, 0x005A, 0x44);

    mos6502_write8(cpu, 0x4401, 0x00);

    mos6502_write8(cpu, 0x8000, 0x41);
    mos6502_write8(cpu, 0x8001, 0x57);

    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0x00 && cpu->flags == ZERO && cpu->pc == 0x8002;
}

static int test_eor_indirect_x_negative(mos6502_t *cpu)
{
    cpu->x = 2;
    mos6502_write8(cpu, 0x0059, 0x01);
    mos6502_write8(cpu, 0x005A, 0x44);

    mos6502_write8(cpu, 0x4401, 0xFF);

    mos6502_write8(cpu, 0x8000, 0x41);
    mos6502_write8(cpu, 0x8001, 0x57);

    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0xFF && cpu->flags == NEGATIVE && cpu->pc == 0x8002;
}

static int test_eor_indirect_y(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x0057, 0x01);
    mos6502_write8(cpu, 0x0058, 0x44);

    mos6502_write8(cpu, 0x4403, 0x27);

    mos6502_write8(cpu, 0x8000, 0x51);
    mos6502_write8(cpu, 0x8001, 0x57);

    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0x27 && cpu->flags == 0 && cpu->pc == 0x8002;
}

static int test_eor_indirect_y_page_boundary(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x0057, 0xFF);
    mos6502_write8(cpu, 0x0058, 0x44);

    mos6502_write8(cpu, 0x4501, 0x27);

    mos6502_write8(cpu, 0x8000, 0x51);
    mos6502_write8(cpu, 0x8001, 0x57);

    int ticks = mos6502_tick(cpu);
    return ticks == 6 && cpu->a == 0x27 && cpu->flags == 0 && cpu->pc == 0x8002;
}

static int test_eor_indirect_y_zero(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x0057, 0x01);
    mos6502_write8(cpu, 0x0058, 0x44);

    mos6502_write8(cpu, 0x4403, 0x00);

    mos6502_write8(cpu, 0x8000, 0x51);
    mos6502_write8(cpu, 0x8001, 0x57);

    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0x00 && cpu->flags == ZERO && cpu->pc == 0x8002;
}

static int test_eor_indirect_y_negative(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x0057, 0x01);
    mos6502_write8(cpu, 0x0058, 0x44);

    mos6502_write8(cpu, 0x4401, 0xFF);

    mos6502_write8(cpu, 0x8000, 0x51);
    mos6502_write8(cpu, 0x8001, 0x57);

    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0xFF && cpu->flags == NEGATIVE && cpu->pc == 0x8002;
}

void test_mos6502_eor()
{
    RUN_TEST(test_eor_immediate);
    RUN_TEST(test_eor_immediate_negative);
    RUN_TEST(test_eor_immediate_zero);

    RUN_TEST(test_eor_zero_page);
    RUN_TEST(test_eor_zero_page_negative);
    RUN_TEST(test_eor_zero_page_zero);

    RUN_TEST(test_eor_zero_page_x);
    RUN_TEST(test_eor_zero_page_x_negative);
    RUN_TEST(test_eor_zero_page_x_zero);

    RUN_TEST(test_eor_absolute);
    RUN_TEST(test_eor_absolute_negative);
    RUN_TEST(test_eor_absolute_zero);

    RUN_TEST(test_eor_absolute_x);
    RUN_TEST(test_eor_absolute_x_page_boundary);
    RUN_TEST(test_eor_absolute_x_negative);
    RUN_TEST(test_eor_absolute_x_zero);

    RUN_TEST(test_eor_absolute_y);
    RUN_TEST(test_eor_absolute_y_page_boundary);
    RUN_TEST(test_eor_absolute_y_negative);
    RUN_TEST(test_eor_absolute_y_zero);

    RUN_TEST(test_eor_indirect_x);
    RUN_TEST(test_eor_indirect_x_negative);
    RUN_TEST(test_eor_indirect_x_zero);

    RUN_TEST(test_eor_indirect_y);
    RUN_TEST(test_eor_absolute_y_page_boundary);
    RUN_TEST(test_eor_absolute_y_negative);
    RUN_TEST(test_eor_absolute_y_zero);
}
#endif
