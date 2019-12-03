#include "mos6502.h"

static int ora_immediate(mos6502_t *cpu)
{
    uint8_t immediate = cpu->read(cpu, cpu->pc++);
    cpu->a = cpu->a | immediate;
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 2;
}


static int ora_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    cpu->a = cpu->a | mos6502_read8(cpu, (uint16_t)zp_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 3;
}


static int ora_zero_page_x(mos6502_t *cpu)
{
    uint8_t zp = cpu->read(cpu, cpu->pc++);
    uint8_t x = cpu->x;
    uint8_t address = zp + x;
    cpu->a = cpu->a | mos6502_read8(cpu, (uint16_t)address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 4;

}


static int ora_absolute(mos6502_t *cpu)
{
    uint16_t absolute = mos6502_read16(cpu, cpu->pc++);
    cpu->pc++;
    cpu->a = cpu->a | mos6502_read16(cpu, absolute);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 4;

}


static int ora_absolute_x(mos6502_t *cpu)
{

    uint16_t absolute = mos6502_read16(cpu, cpu->pc++);
    cpu->pc++;
    absolute = absolute + cpu->x;
    cpu->a = cpu->a | mos6502_read16(cpu, absolute);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);




    if (cpu->pc >> 8 != absolute >> 8)
    {
        return 5;
    }
    
    return 4;
}

static int ora_absolute_y(mos6502_t *cpu)
{

    uint16_t absolute = mos6502_read16(cpu, cpu->pc++);
    cpu->pc++;
    absolute = absolute + cpu->y;
    cpu->a = cpu->a | mos6502_read16(cpu, absolute);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 4;

    if (cpu->pc >> 8 != absolute >> 8)
    {
        return 5;
    }

}

static int ora_indirect_x(mos6502_t *cpu)
{
    uint16_t firstAddressByte = mos6502_read8(cpu, cpu->pc++);
    uint16_t  address = mos6502_read16(cpu, firstAddressByte + cpu->x);
    cpu->a = cpu->a | cpu->read(cpu, address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 6;

}


static int ora_indirect_y(mos6502_t *cpu)
{

    uint16_t firstAddressByte = mos6502_read8(cpu, cpu->pc++);
    uint16_t address = mos6502_read16(cpu, firstAddressByte) + cpu->y;
    cpu->a = cpu->a | cpu->read(cpu, address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);



    if (cpu->pc >> 8 != address >> 8)
    {
        return 6;
    }

    return 5;

}

void mos6502_register_ora(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x09, ora_immediate);
    mos6502_register_opcode(cpu, 0x05, ora_zero_page);
    mos6502_register_opcode(cpu, 0x15, ora_zero_page_x);
    mos6502_register_opcode(cpu, 0x0D, ora_absolute);
    mos6502_register_opcode(cpu, 0x1D, ora_absolute_x);
    mos6502_register_opcode(cpu, 0x19, ora_absolute_y);
    mos6502_register_opcode(cpu, 0x01, ora_indirect_x);
    mos6502_register_opcode(cpu, 0x11, ora_indirect_y);
}


#ifdef _TEST

static int test_ora_immediate(mos6502_t *cpu)
{
    cpu->a = 0x15;
    mos6502_write8(cpu, 0x8000, 0x09);
    mos6502_write8(cpu, 0x8001, 0x10);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0x15 && cpu->pc == 0x8002 && cpu->flags == 0;
}


static int test_ora_zeropage(mos6502_t *cpu)
{
    cpu->a = 0x10;
    mos6502_write8(cpu, 0x0b, 0x08);
    mos6502_write8(cpu, 0x8000, 0x05);
    mos6502_write8(cpu, 0x8001, 0x0b);
    int ticks = mos6502_tick(cpu);


    return ticks == 3 && cpu->a == 0x18 && cpu->pc == 0x8002 && cpu->flags == 0;
}


static int test_ora_zeropage_x(mos6502_t *cpu)
{
    cpu->a = 0x10;
    cpu->x = 0x01;

    mos6502_write8(cpu, 0x0c, 0x08);
    mos6502_write8(cpu, 0x8000, 0x15);
    mos6502_write8(cpu, 0x8001, 0x0b);
    int ticks = mos6502_tick(cpu);


    return ticks == 4 && cpu->a == 0x18 && cpu->pc == 0x8002 && cpu->flags == 0;
}


static int test_ora_absolute(mos6502_t *cpu)
{
    cpu->a = 0x10;

    mos6502_write8(cpu, 0x0100, 0x08);
    mos6502_write8(cpu, 0x8000, 0x0d);
    mos6502_write8(cpu, 0x8001, 0x00);
    mos6502_write8(cpu, 0x8002, 0x01);

    int ticks = mos6502_tick(cpu);

    return ticks == 4 && cpu->a == 0x18 && cpu->pc == 0x8003 && cpu->flags == 0;
}


static int test_ora_absolute_x(mos6502_t *cpu)
{
    cpu->a = 0x10;
    cpu->x = 0x01;

    mos6502_write8(cpu, 0x0101, 0x08);
    mos6502_write8(cpu, 0x8000, 0x1d);
    mos6502_write8(cpu, 0x8001, 0x00);
    mos6502_write8(cpu, 0x8002, 0x01);
    int ticks = mos6502_tick(cpu);


    return ticks == 5 && cpu->a == 0x18 && cpu->pc == 0x8003 && cpu->flags == 0;
}


static int test_ora_absolute_y(mos6502_t *cpu)
{
    cpu->a = 0x10;
    cpu->y = 0x01;

    mos6502_write8(cpu, 0x0101, 0x08);
    mos6502_write8(cpu, 0x8000, 0x19);
    mos6502_write8(cpu, 0x8001, 0x00);
    mos6502_write8(cpu, 0x8002, 0x01);
    int ticks = mos6502_tick(cpu);


    return ticks == 4 && cpu->a == 0x18 && cpu->pc == 0x8003 && cpu->flags == 0;
}

static int test_ora_indirect_x(mos6502_t *cpu)
{
    cpu->a = 0x10;
    cpu->x = 0x01;

    mos6502_write8(cpu, 0x03, 0x04);
    mos6502_write8(cpu, 0x04, 0x05);
    mos6502_write8(cpu, 0x0504, 0x0a);


    mos6502_write8(cpu, 0x8000, 0x01);
    mos6502_write8(cpu, 0x8001, 0x02);
    int ticks = mos6502_tick(cpu);

    return ticks == 6 && cpu->a == 0x1a && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_ora_indirect_y(mos6502_t *cpu)
{
    cpu->a = 0x10;
    cpu->y = 0x01;

    mos6502_write8(cpu, 0x02, 0x04);
    mos6502_write8(cpu, 0x03, 0x05);
    mos6502_write8(cpu, 0x0505, 0x0a);


    mos6502_write8(cpu, 0x8000, 0x11);
    mos6502_write8(cpu, 0x8001, 0x02);
    int ticks = mos6502_tick(cpu);

    return ticks == 6 && cpu->a == 0x1a && cpu->pc == 0x8002 && cpu->flags == 0;
}




void test_mos6502_ora()
{
    RUN_TEST(test_ora_immediate);
    RUN_TEST(test_ora_zeropage);
    RUN_TEST(test_ora_zeropage_x);
    RUN_TEST(test_ora_absolute);
    RUN_TEST(test_ora_absolute_x);
    RUN_TEST(test_ora_absolute_y);
    RUN_TEST(test_ora_indirect_x);
    RUN_TEST(test_ora_indirect_y);
}


#endif
