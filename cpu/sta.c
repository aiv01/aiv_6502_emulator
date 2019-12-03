#include "mos6502.h"

static int sta_zeropage(mos6502_t *cpu)
{
    uint8_t zp_address = mos6502_read8(cpu, cpu->pc++);
    mos6502_write8(cpu, (uint16_t)zp_address, cpu->a);
    return 3;
}

static int sta_zeropage_x(mos6502_t *cpu)
{
    uint8_t zp_address = (mos6502_read8(cpu, cpu->pc++)) + cpu->x;
    mos6502_write8(cpu, (uint16_t)zp_address, cpu->a);
    return 4;
}

static int sta_absolute(mos6502_t *cpu)
{
    uint16_t address = mos6502_read16(cpu, cpu->pc);
    cpu->pc+=2;
    mos6502_write8(cpu, address, cpu->a);
    return 4;
}

static int sta_absolute_x(mos6502_t *cpu)
{
    uint16_t address = (mos6502_read16(cpu, cpu->pc)) + cpu->x;
    cpu->pc+=2;
    mos6502_write8(cpu, address, cpu->a);
    return 5;
}

static int sta_absolute_y(mos6502_t *cpu)
{
    uint16_t address = (mos6502_read16(cpu, cpu->pc)) + cpu->y;
    cpu->pc+=2;
    mos6502_write8(cpu, address, cpu->a);
    return 5;
}

static int sta_indirect_x(mos6502_t *cpu)
{
    uint8_t zp_address = (mos6502_read8(cpu, cpu->pc++)) + cpu->x;
    uint16_t address = mos6502_read16(cpu, (uint16_t)zp_address);
    mos6502_write8(cpu, address, cpu->a);
    return 6;
}

static int sta_indirect_y(mos6502_t *cpu)
{
    uint8_t zp_address = mos6502_read8(cpu, cpu->pc++);
    uint16_t address = (mos6502_read16(cpu, (uint16_t)zp_address)) + cpu->y;
    mos6502_write8(cpu, address, cpu->a);
    return 6;
}

void mos6502_register_sta(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x85, sta_zeropage);
    mos6502_register_opcode(cpu, 0x95, sta_zeropage_x);
    mos6502_register_opcode(cpu, 0x8D, sta_absolute);
    mos6502_register_opcode(cpu, 0x9D, sta_absolute_x);
    mos6502_register_opcode(cpu, 0x99, sta_absolute_y);
    mos6502_register_opcode(cpu, 0x81, sta_indirect_x);
    mos6502_register_opcode(cpu, 0x91, sta_indirect_y);
}

#ifdef _TEST

static int test_sta_zeropage(mos6502_t *cpu)
{
    cpu->a=0x49;
    mos6502_write8(cpu, 0x8000, 0x85);
    mos6502_write8(cpu, 0x8001, 0x44);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x0044);
    return ticks == 3 && cpu->a == 0x49 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_zeropage_x(mos6502_t *cpu)
{
    cpu->a = 0x49;
    cpu->x = 0x10;    
    mos6502_write8(cpu, 0x8000, 0x95);
    mos6502_write8(cpu, 0x8001, 0x40);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x0050);
    return ticks == 4 && cpu->a == 0x49 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_zeropage_x_overflow(mos6502_t *cpu)
{
    cpu->a = 0x49;
    cpu->x = 0xFF;   
    mos6502_write8(cpu, 0x8000, 0x95);
    mos6502_write8(cpu, 0x8001, 0x50);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x004F);
    return ticks == 4 && cpu->a == 0x49 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_absolute(mos6502_t *cpu)
{
    cpu->a = 0x49;
    mos6502_write8(cpu, 0x8000, 0x8D);
    mos6502_write16(cpu, 0x8001, 0x4400);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x4400);
    return ticks == 4 && cpu->a == 0x49 && cpu->pc == 0x8003 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_absolute_x(mos6502_t *cpu)
{
    cpu->a = 0x49;
    cpu->x = 0x10;
    mos6502_write8(cpu, 0x8000, 0x9D);
    mos6502_write16(cpu, 0x8001, 0x4400);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x4410);
    return ticks == 5 && cpu->a == 0x49 && cpu->pc == 0x8003 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_absolute_x_overflow(mos6502_t *cpu)
{
    cpu->a = 0x49;
    cpu->x = 0xF0;
    mos6502_write8(cpu, 0x8000, 0x9D);
    mos6502_write16(cpu, 0x8001, 0xFFFF);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x00EF);
    return ticks == 5 && cpu->a == 0x49 && cpu->pc == 0x8003 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_absolute_y(mos6502_t *cpu)
{
    cpu->a = 0x49;
    cpu->y = 0X10;
    mos6502_write8(cpu, 0x8000, 0x99);
    mos6502_write16(cpu, 0x8001, 0x4400);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x4410);
    return ticks == 5 && cpu->a == 0x49 && cpu->pc == 0x8003 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_absolute_y_overflow(mos6502_t *cpu)
{
    cpu->a = 0x49;
    cpu->y = 0xF0;
    mos6502_write8(cpu, 0x8000, 0x99);
    mos6502_write16(cpu, 0x8001, 0xFFFF);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x00EF);
    return ticks == 5 && cpu->a == 0x49 && cpu->pc == 0x8003 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_indirect_x(mos6502_t *cpu)
{
    cpu->a = 0x49;
    cpu->x = 0x10;
    mos6502_write8(cpu, 0x8000, 0x81);
    mos6502_write8(cpu, 0x8001, 0x35);
    mos6502_write16(cpu, 0x0045, 0x9000);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x9000);
    return ticks == 6 && cpu->a == 0x49 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_indirect_x_overflow(mos6502_t *cpu)
{
    cpu->a = 0x49;
    cpu->x = 0x02;
    mos6502_write8(cpu, 0x8000, 0x81);
    mos6502_write8(cpu, 0x8001, 0xFF);
    mos6502_write16(cpu, 0x0001, 0x9000);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x9000);
    return ticks == 6 && cpu->a == 0x49 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_indirect_y(mos6502_t *cpu)
{
    cpu->a = 0x49;
    cpu->y = 0x50;
    mos6502_write8(cpu, 0x8000, 0x91);
    mos6502_write8(cpu, 0x8001, 0x44);
    mos6502_write16(cpu, 0x0044, 0x9000);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x9050);
    return ticks == 6 && cpu->a == 0x49 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x49;
}

static int test_sta_indirect_y_overflow(mos6502_t *cpu)
{
    cpu->a = 0x49;
    cpu->y = 0x05;
    mos6502_write8(cpu, 0x8000, 0x91);
    mos6502_write8(cpu, 0x8001, 0x44);
    mos6502_write16(cpu, 0x0044, 0xFFFF);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x0004);
    return ticks == 6 && cpu->a == 0x49 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x49;
}

void test_mos6502_sta()
{
    RUN_TEST(test_sta_zeropage);
    RUN_TEST(test_sta_zeropage_x);
    RUN_TEST(test_sta_zeropage_x_overflow);
    RUN_TEST(test_sta_absolute);
    RUN_TEST(test_sta_absolute_x);
    RUN_TEST(test_sta_absolute_x_overflow);
    RUN_TEST(test_sta_absolute_y);
    RUN_TEST(test_sta_absolute_y_overflow);
    RUN_TEST(test_sta_indirect_x);
    RUN_TEST(test_sta_indirect_x_overflow);
    RUN_TEST(test_sta_indirect_y);
    RUN_TEST(test_sta_indirect_y_overflow);
}
#endif