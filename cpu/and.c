#include "mos6502.h"

static int and_immediate(mos6502_t *cpu)
{
    uint8_t immediate = cpu->read(cpu, cpu->pc++);
    cpu->a = cpu->a & immediate;
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 2;
}


static int and_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    cpu->a = cpu->a & mos6502_read8(cpu, (uint16_t)zp_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 3;
}


static int and_zero_page_x(mos6502_t *cpu)
{
    uint8_t zeropage = cpu->read(cpu, cpu->pc++);
    uint8_t x = cpu->x;
    uint8_t address = zeropage + x;
    cpu->a = cpu->a & mos6502_read8(cpu, (uint16_t)address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 4;

}


static int and_absolute(mos6502_t *cpu)
{
    uint16_t absolute = mos6502_read16(cpu, cpu->pc++);
    cpu->pc++;
    cpu->a = cpu->a & mos6502_read16(cpu, absolute);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 4;

}


static int and_absolute_x(mos6502_t *cpu)
{

    uint16_t absolute = mos6502_read16(cpu, cpu->pc++);
    cpu->pc++;
    absolute = absolute + cpu->x;
    cpu->a = cpu->a & mos6502_read16(cpu, absolute);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);

    if (cpu->pc >> 8 != absolute >> 8)
    {
        return 5;
    }
    
    return 4;
}

static int and_absolute_y(mos6502_t *cpu)
{

    uint16_t absolute = mos6502_read16(cpu, cpu->pc++);
    cpu->pc++;
    absolute = absolute + cpu->y;
    cpu->a = cpu->a & mos6502_read16(cpu, absolute);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);

    if (cpu->pc >> 8 != absolute >> 8)
    {
        return 5;
    }
    return 4;

}

static int and_indirect_x(mos6502_t *cpu)
{
    uint16_t firstAddressByte = mos6502_read8(cpu, cpu->pc++);
    uint16_t  address = mos6502_read16(cpu, firstAddressByte + cpu->x);
    cpu->a = cpu->a & cpu->read(cpu, address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 6;

}


static int and_indirect_y(mos6502_t *cpu)
{

    uint16_t firstAddressByte = mos6502_read8(cpu, cpu->pc++);
    uint16_t address = mos6502_read16(cpu, firstAddressByte) + cpu->y;
    cpu->a = cpu->a & cpu->read(cpu, address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);

    if (cpu->pc >> 8 != address >> 8)
    {
        return 6;
    }

    return 5;

}

void mos6502_register_and(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x29, and_immediate);
    mos6502_register_opcode(cpu, 0x25, and_zero_page);
    mos6502_register_opcode(cpu, 0x35, and_zero_page_x);
    mos6502_register_opcode(cpu, 0x2D, and_absolute);
    mos6502_register_opcode(cpu, 0x3D, and_absolute_x);
    mos6502_register_opcode(cpu, 0x39, and_absolute_y);
    mos6502_register_opcode(cpu, 0x21, and_indirect_x);
    mos6502_register_opcode(cpu, 0x31, and_indirect_y);
}


#ifdef _TEST

static int test_and_immediate(mos6502_t *cpu)
{
    cpu->a = 0x15;
    mos6502_write8(cpu, 0x8000, 0x29);
    mos6502_write8(cpu, 0x8001, 0x25);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0x05 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_and_immediate_zero_flag(mos6502_t *cpu)
{
    cpu->a = 0x15;
    mos6502_write8(cpu, 0x8000, 0x29);
    mos6502_write8(cpu, 0x8001, 0x20);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0x00 && cpu->pc == 0x8002 && cpu->flags == ZERO;
}
static int test_and_immediate_negative_flag(mos6502_t *cpu)
{
    cpu->a = 0xFF;
    mos6502_write8(cpu, 0x8000, 0x29);
    mos6502_write8(cpu, 0x8001, 0xC8);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0xC8 && cpu->pc == 0x8002 && cpu->flags == NEGATIVE;
}


static int test_and_zeropage(mos6502_t *cpu)
{
    cpu->a = 0x15;
    mos6502_write8(cpu, 0x0b, 0x25);
    mos6502_write8(cpu, 0x8000, 0x25);
    mos6502_write8(cpu, 0x8001, 0x0b);
    int ticks = mos6502_tick(cpu);


    return ticks == 3 && cpu->a == 0x05 && cpu->pc == 0x8002 && cpu->flags == 0;
}


static int test_and_zeropage_x(mos6502_t *cpu)
{
    cpu->a = 0x15;
    cpu->x = 0x01;

    mos6502_write8(cpu, 0x0c, 0x25);
    mos6502_write8(cpu, 0x8000, 0x35);
    mos6502_write8(cpu, 0x8001, 0x0b);
    int ticks = mos6502_tick(cpu);


    return ticks == 4 && cpu->a == 0x05 && cpu->pc == 0x8002 && cpu->flags == 0;
}


static int test_and_absolute(mos6502_t *cpu)
{
    cpu->a = 0x15;

    mos6502_write8(cpu, 0x0100, 0x25);
    mos6502_write8(cpu, 0x8000, 0x2d);
    mos6502_write8(cpu, 0x8001, 0x00);
    mos6502_write8(cpu, 0x8002, 0x01);

    int ticks = mos6502_tick(cpu);

    return ticks == 4 && cpu->a == 0x05 && cpu->pc == 0x8003 && cpu->flags == 0;
}


static int test_and_absolute_x(mos6502_t *cpu)
{
    cpu->a = 0x15;
    cpu->x = 0x01;

    mos6502_write8(cpu, 0x0101, 0x25);
    mos6502_write8(cpu, 0x8000, 0x3d);
    mos6502_write8(cpu, 0x8001, 0x00);
    mos6502_write8(cpu, 0x8002, 0x01);
    int ticks = mos6502_tick(cpu);


    return ticks == 5 && cpu->a == 0x05 && cpu->pc == 0x8003 && cpu->flags == 0;
}


static int test_and_absolute_y(mos6502_t *cpu)
{
    cpu->a = 0x15;
    cpu->y = 0x01;

    mos6502_write8(cpu, 0x0101, 0x25);
    mos6502_write8(cpu, 0x8000, 0x39);
    mos6502_write8(cpu, 0x8001, 0x00);
    mos6502_write8(cpu, 0x8002, 0x01);
    int ticks = mos6502_tick(cpu);
    return ticks == 5 && cpu->a == 0x05 && cpu->pc == 0x8003 && cpu->flags == 0;
}

static int test_and_indirect_x(mos6502_t *cpu)
{
    cpu->a = 0x15;
    cpu->x = 0x01;

    mos6502_write8(cpu, 0x03, 0x04);
    mos6502_write8(cpu, 0x04, 0x05);
    mos6502_write8(cpu, 0x0504, 0x25);


    mos6502_write8(cpu, 0x8000, 0x21);
    mos6502_write8(cpu, 0x8001, 0x02);
    int ticks = mos6502_tick(cpu);

    return ticks == 6 && cpu->a == 0x05 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_and_indirect_y(mos6502_t *cpu)
{
    cpu->a = 0x15;
    cpu->y = 0x01;

    mos6502_write8(cpu, 0x02, 0x04);
    mos6502_write8(cpu, 0x03, 0x05);
    mos6502_write8(cpu, 0x0505, 0x25);


    mos6502_write8(cpu, 0x8000, 0x31);
    mos6502_write8(cpu, 0x8001, 0x02);
    int ticks = mos6502_tick(cpu);

    return ticks == 6 && cpu->a == 0x05 && cpu->pc == 0x8002 && cpu->flags == 0;
}




void test_mos6502_and()
{
    RUN_TEST(test_and_immediate);
    RUN_TEST(test_and_immediate_zero_flag);
    RUN_TEST(test_and_immediate_negative_flag);
    RUN_TEST(test_and_zeropage);
    RUN_TEST(test_and_zeropage_x);
    RUN_TEST(test_and_absolute);
    RUN_TEST(test_and_absolute_x);
    RUN_TEST(test_and_absolute_y);
    RUN_TEST(test_and_indirect_x);
    RUN_TEST(test_and_indirect_y);
}


#endif
