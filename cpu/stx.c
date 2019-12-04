#include "mos6502.h"


static int stx_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
  
    mos6502_write8(cpu, (uint16_t)zp_address, (uint8_t)cpu->x);
    return 3;
}

static int stx_zeropage_y(mos6502_t *cpu)
{
    uint8_t zp_address = (mos6502_read8(cpu, cpu->pc++)) + cpu->y;
    mos6502_write8(cpu, (uint16_t)zp_address, cpu->x);
    return 4;
}


static int stx_absolute(mos6502_t *cpu)
{
    uint16_t address = mos6502_read16(cpu, cpu->pc);
    cpu->pc+=2;
    cpu->x = mos6502_read16(cpu, address);

    return 4;
}



void mos6502_register_stx(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x86, stx_zero_page);
    mos6502_register_opcode(cpu, 0x96, stx_zeropage_y);
    mos6502_register_opcode(cpu, 0x8E, stx_absolute);
}

#ifdef _TEST

static int test_stx_zeropage(mos6502_t *cpu)
{
    cpu->x=0x49;
    mos6502_write8(cpu, 0x8000, 0x86);
    mos6502_write8(cpu, 0x8001, 0x44);
    mos6502_write8(cpu, 0x0044, 0xA);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x44);
    return ticks == 3 && cpu->x == 0x49 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x49;
}

static int test_stx_zeropage_y(mos6502_t *cpu)
{
    cpu->x = 0x49;
    cpu->y = 0x10;    
    mos6502_write8(cpu, 0x8000, 0x96);
    mos6502_write8(cpu, 0x8001, 0x40);
    int ticks = mos6502_tick(cpu);
    uint8_t new_value = mos6502_read8(cpu,0x0050);
    return ticks == 4 && cpu->x == 0x49 && cpu->pc == 0x8002 && cpu->flags == 0 && new_value == 0x49;
}

static int test_stx_absolute(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x8E);
    mos6502_write8(cpu, 0x8001, 0x00);
    mos6502_write8(cpu, 0x8002, 0x44);
    mos6502_write8(cpu, 0x4400, 0xA);
    int ticks = mos6502_tick(cpu);

    return ticks == 4 && cpu->x == 0xA && cpu->pc == 0x8003 && cpu->flags == 0;
}





void test_mos6502_stx()
{
    
    RUN_TEST(test_stx_zeropage);
    RUN_TEST(test_stx_zeropage_y);
    RUN_TEST(test_stx_absolute);
   
}

#endif