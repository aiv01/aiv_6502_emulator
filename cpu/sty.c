#include "mos6502.h"


static int sty_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = mos6502_read8(cpu, cpu->pc++);
    mos6502_write8(cpu, (uint16_t)zp_address, cpu->y);
    return 3;
}

static int sty_zeropage_x(mos6502_t *cpu)
{
    uint8_t zp_address = (mos6502_read8(cpu, cpu->pc++)) + cpu->x;
    mos6502_write8(cpu, (uint16_t)zp_address, cpu->y);
    return 4;
}


static int sty_absolute(mos6502_t *cpu)
{
    uint16_t address = mos6502_read16(cpu, cpu->pc);
    cpu->pc+=2;
    
    mos6502_write8(cpu, address, cpu->y);

    return 4;
}



void mos6502_register_sty(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x84, sty_zero_page);
    mos6502_register_opcode(cpu, 0x94, sty_zeropage_x);
    mos6502_register_opcode(cpu, 0x8C, sty_absolute);
}

#ifdef _TEST



static int test_sty_zero_page(mos6502_t *cpu)
{
    cpu->y=0x77;
    mos6502_write8(cpu, 0x8000, 0x84);
    mos6502_write8(cpu, 0x8001, 0x44);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x0044);
    return ticks == 3 && cpu->y == 0x77 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x77;
}

static int test_sty_zeropage_x(mos6502_t *cpu)
{
    cpu->y = 0x77;
    cpu->x = 0x30;    
    mos6502_write8(cpu, 0x8000, 0x94);
    mos6502_write8(cpu, 0x8001, 0x40);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x0070);
    return ticks == 4 && cpu->y == 0x77 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x77;
}

static int test_sty_absolute(mos6502_t *cpu)
{
    cpu->y = 0x49;
    mos6502_write8(cpu, 0x8000, 0x8C);
    mos6502_write16(cpu, 0x8001, 0x4400);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x4400);
    return ticks == 4 && cpu->y == 0x49 && cpu->pc == 0x8003 && cpu->flags == 0 && new_value == 0x49;
}


void test_mos6502_sty()
{
    RUN_TEST(test_sty_absolute);
    RUN_TEST(test_sty_zero_page);
    RUN_TEST(test_sty_zeropage_x);

}

#endif 
