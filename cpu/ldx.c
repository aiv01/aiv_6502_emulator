#include "mos6502.h"

static int ldx_immediate(mos6502_t *cpu)
{
    uint8_t immediate = cpu->read(cpu, cpu->pc++);
    cpu->x = immediate;
    mos6502_set_flag(cpu, NEGATIVE, cpu->x & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->x == 0);
    return 2;
}

static int ldx_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    cpu->x = mos6502_read8(cpu, (uint16_t)zp_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->x & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->x == 0);
    return 3;
}

static int ldx_zero_page_y(mos6502_t *cpu)
{    
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    uint16_t new_address = zp_address + cpu->y;
    cpu->x = mos6502_read8(cpu, new_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->x & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->x == 0);
    uint8_t boundary_page = (new_address >> 8);    
    return 4 + boundary_page;
}

static int ldx_absolute(mos6502_t *cpu)
{
    uint8_t low = cpu->read(cpu, cpu->pc++);
    uint8_t high = cpu->read(cpu, cpu->pc++);
    uint16_t address = (high << 8) | low;   
    cpu->x = mos6502_read8(cpu, address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->x & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->x == 0);  
    return 4;
}

static int ldx_absolute_y(mos6502_t *cpu)
{
    uint8_t low = cpu->read(cpu, cpu->pc++);
    uint8_t high = cpu->read(cpu, cpu->pc++);
    uint16_t address = (high << 8) | low;
    uint16_t new_address = address + cpu->y;
    cpu->x = mos6502_read8(cpu, address + cpu->y);
    mos6502_set_flag(cpu, NEGATIVE, cpu->x & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->x == 0);
    uint8_t boundary_page = (new_address >> 8) - high;
    return 4 + boundary_page;
}


void mos6502_register_ldx(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xA2, ldx_immediate);
    mos6502_register_opcode(cpu, 0xA6, ldx_zero_page);
    mos6502_register_opcode(cpu, 0xB6, ldx_zero_page_y);
    mos6502_register_opcode(cpu, 0xAE, ldx_absolute);
    mos6502_register_opcode(cpu, 0xBE, ldx_absolute_y);
}

#ifdef _TEST

static int test_ldx_immediate(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA2);
    mos6502_write8(cpu, 0x8001, 0x55);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->x == 0x55 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_ldx_immediate_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA2);
    mos6502_write8(cpu, 0x8001, 0xFF);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->x == 0xFF && cpu->pc == 0x8002 && cpu->flags == NEGATIVE;
}

static int test_ldx_immediate_zero(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA2);
    mos6502_write8(cpu, 0x8001, 0x00);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->x == 0x00 && cpu->pc == 0x8002 && cpu->flags == ZERO;
}

static int test_ldx_zero_page(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0034, 0x09);
    mos6502_write8(cpu, 0x8000, 0xA6);
    mos6502_write8(cpu, 0x8001, 0x34);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->x == 0x09 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_ldx_zero_page_same_code(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x00A6, 0x45);
    mos6502_write8(cpu, 0x8000, 0xA6);
    mos6502_write8(cpu, 0x8001, 0xA6);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->x == 0x45 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_ldx_zero_page_zero(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA6);
    mos6502_write8(cpu, 0x8001, 0x04);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->x == 0x00 && cpu->pc == 0x8002 && cpu->flags == ZERO;
}

static int test_ldx_zero_page_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0034, 0xFF);
    mos6502_write8(cpu, 0x8000, 0xA6);
    mos6502_write8(cpu, 0x8001, 0x34);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->x == 0xFF && cpu->pc == 0x8002 && cpu->flags == NEGATIVE;
}

static int test_ldx_zero_page_y(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x0036, 0x7f);
    mos6502_write8(cpu, 0x8000, 0xB6);
    mos6502_write8(cpu, 0x8001, 0x34);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->x == 0x7f && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_ldx_zero_page_y_same_code(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x00B6, 0x7f);
    mos6502_write8(cpu, 0x8000, 0xB6);
    mos6502_write8(cpu, 0x8001, 0xB4);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->x == 0x7f && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_ldx_zero_page_y_negative(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x00D4, 0x80);
    mos6502_write8(cpu, 0x8000, 0xB6);
    mos6502_write8(cpu, 0x8001, 0xD2);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->x == 0x80 && cpu->pc == 0x8002 && cpu->flags == NEGATIVE;
}

static int test_ldx_zero_page_y_page_boundary(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x0101, 0x44);
    mos6502_write8(cpu, 0x8000, 0xB6);
    mos6502_write8(cpu, 0x8001, 0xFF);
    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->x == 0x44 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_ldx_zero_page_y_page_boundary_negative(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x0101, 0xF0);
    mos6502_write8(cpu, 0x8000, 0xB6);
    mos6502_write8(cpu, 0x8001, 0xFF);
    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->x == 0xF0 && cpu->pc == 0x8002 && cpu->flags == NEGATIVE;
}

static int test_ldx_absolute(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0705, 0x34);
    mos6502_write8(cpu, 0x8000, 0xAE);
    mos6502_write16(cpu, 0x8001, 0x0705);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->x == 0x34 && cpu->pc == 0x8003 && cpu->flags == 0;
}

static int test_ldx_absolute_same_code(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x00AE, 0x34);
    mos6502_write8(cpu, 0x8000, 0xAE);
    mos6502_write16(cpu, 0x8001, 0x00AE);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->x == 0x34 && cpu->pc == 0x8003 && cpu->flags == 0;
}

static int test_ldx_absolute_zero(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xAE);
    mos6502_write16(cpu, 0x8001, 0x0087);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->x == 0x00 && cpu->pc == 0x8003 && cpu->flags == ZERO;
}

static int test_ldx_absolute_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0707, 0xFF);
    mos6502_write8(cpu, 0x8000, 0xAE);
    mos6502_write16(cpu, 0x8001, 0x0707);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->x == 0xFF && cpu->pc == 0x8003 && cpu->flags == NEGATIVE;
}

static int test_ldx_absolute_y(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x0803, 0x77);
    mos6502_write8(cpu, 0x8000, 0xBE);
    mos6502_write16(cpu, 0x8001, 0x0801);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->x == 0x77 && cpu->pc == 0x8003 && cpu->flags == 0;
}

static int test_ldx_absolute_y_same_code(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x00BE, 0x77);
    mos6502_write8(cpu, 0x8000, 0xBE);
    mos6502_write16(cpu, 0x8001, 0x00BC);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->x == 0x77 && cpu->pc == 0x8003 && cpu->flags == 0;
}

static int test_ldx_absolute_y_negative(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x0803, 0x87);
    mos6502_write8(cpu, 0x8000, 0xBE);
    mos6502_write16(cpu, 0x8001, 0x0801);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->x == 0x87 && cpu->pc == 0x8003 && cpu->flags == NEGATIVE;
}

static int test_ldx_absolute_y_page_boundary(mos6502_t *cpu)
{
    cpu->y = 2;
    mos6502_write8(cpu, 0x0601, 0x6E);
    mos6502_write8(cpu, 0x8000, 0xBE);
    mos6502_write16(cpu, 0x8001, 0x05ff);
    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->x == 0x6e && cpu->pc == 0x8003 && cpu->flags == 0;
}



void test_mos6502_ldx()
{
    RUN_TEST(test_ldx_immediate);
    RUN_TEST(test_ldx_immediate_negative);
    RUN_TEST(test_ldx_immediate_zero);
    RUN_TEST(test_ldx_zero_page);
    RUN_TEST(test_ldx_zero_page_same_code);
    RUN_TEST(test_ldx_zero_page_negative);
    RUN_TEST(test_ldx_zero_page_zero);
    RUN_TEST(test_ldx_zero_page_y);
    RUN_TEST(test_ldx_zero_page_y_same_code);
    RUN_TEST(test_ldx_zero_page_y_negative);
    RUN_TEST(test_ldx_zero_page_y_page_boundary);
    RUN_TEST(test_ldx_zero_page_y_page_boundary_negative);
    RUN_TEST(test_ldx_absolute);
    RUN_TEST(test_ldx_absolute_same_code);
    RUN_TEST(test_ldx_absolute_negative);
    RUN_TEST(test_ldx_absolute_zero);
    RUN_TEST(test_ldx_absolute_y);
    RUN_TEST(test_ldx_absolute_y_same_code);
    RUN_TEST(test_ldx_absolute_y_negative);
    RUN_TEST(test_ldx_absolute_y_page_boundary);
}
#endif
