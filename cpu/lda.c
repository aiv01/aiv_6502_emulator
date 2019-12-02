#include "mos6502.h"

static int lda_immediate(mos6502_t *cpu)
{
    uint8_t immediate = cpu->read(cpu, cpu->pc++);
    cpu->a = immediate;
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 2;
}

static int lda_zero_page(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    cpu->a = mos6502_read8(cpu, (uint16_t)zp_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 3;
}

static int lda_zero_page_x(mos6502_t *cpu)
{
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    cpu->a = mos6502_read8(cpu, (uint16_t)zp_address+ cpu->x);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);
    return 4;
}

static int lda_absolute(mos6502_t *cpu)
{
    uint16_t zp_address = mos6502_read16(cpu,cpu->pc++);
    cpu->a = mos6502_read16(cpu, zp_address);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);

    return 4;
}

static int lda_absolute_x(mos6502_t *cpu)
{
    uint16_t zp_address = mos6502_read16(cpu,cpu->pc++);
    uint16_t finalAdress=zp_address+cpu->x;
    cpu->a = mos6502_read16(cpu, zp_address+cpu->x);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);

     uint8_t lowAdr = (uint8_t)(zp_address & 0xFF);
     uint8_t highAdr = (uint8_t)((zp_address >> 8) & 0xFF);
     uint8_t lowFinAdr = (uint8_t)(finalAdress & 0xFF);
     uint8_t highFinAdr = (uint8_t)((finalAdress >> 8) & 0xFF);

      if(highFinAdr == highAdr )
         return 4;
      else
         return 5;   
}

static int lda_absolute_y(mos6502_t *cpu)
{
    uint16_t zp_address = mos6502_read16(cpu,cpu->pc++);
    uint16_t finalAdress=zp_address+cpu->y;
    cpu->a = mos6502_read16(cpu, zp_address+cpu->y);
    mos6502_set_flag(cpu, NEGATIVE, cpu->a & NEGATIVE);
    mos6502_set_flag(cpu, ZERO, cpu->a == 0);

     uint8_t lowAdr = (uint8_t)(zp_address & 0xFF);
     uint8_t highAdr = (uint8_t)((zp_address >> 8) & 0xFF);
     uint8_t lowFinAdr = (uint8_t)(finalAdress & 0xFF);
     uint8_t highFinAdr = (uint8_t)((finalAdress >> 8) & 0xFF);

    if(highFinAdr == highAdr )
       return 4;
    else
       return 5;
}

static int lda_indirect_x(mos6502_t *cpu)
{
    uint8_t zp_address = mos6502_read8(cpu,cpu->pc++)+cpu->x;
    uint8_t lowAdr = mos6502_read8(cpu,zp_address);
    uint8_t highByte= mos6502_read8(cpu,zp_address+1);
    uint16_t finalAdress=(highByte << 8) | lowAdr;
    cpu->a = mos6502_read16(cpu, finalAdress);

       return 6;
}

static int lda_indirect_y(mos6502_t *cpu)
{
    uint8_t zp_address = mos6502_read8(cpu,cpu->pc++);
    uint8_t lowAdr = mos6502_read8(cpu,zp_address);
    uint8_t highByte= mos6502_read8(cpu,zp_address+1);
    uint16_t finalAdress=(highByte << 8) | lowAdr;
    cpu->a = mos6502_read16(cpu, finalAdress+cpu->y);
 if(highByte == (uint8_t)(cpu->pc >> 8))
       return 5;
    else
       return 6;
}
void mos6502_register_lda(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xA9, lda_immediate);
    mos6502_register_opcode(cpu, 0xA5, lda_zero_page);
    mos6502_register_opcode(cpu, 0xB5, lda_zero_page_x);
    mos6502_register_opcode(cpu, 0xAD, lda_absolute);
    mos6502_register_opcode(cpu, 0xBD, lda_absolute_x);   
    mos6502_register_opcode(cpu, 0xB9, lda_absolute_y);   
    mos6502_register_opcode(cpu, 0xA1, lda_indirect_x);        
    mos6502_register_opcode(cpu, 0xB1, lda_indirect_y);        

}


#ifdef _TEST

static int test_lda_immediate(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA9);
    mos6502_write8(cpu, 0x8001, 0x55);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0x55 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_lda_immediate_negative(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA9);
    mos6502_write8(cpu, 0x8001, 0xFF);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0xFF && cpu->pc == 0x8002 && cpu->flags == NEGATIVE;
}

static int test_lda_immediate_zero(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xA9);
    mos6502_write8(cpu, 0x8001, 0x00);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->a == 0x00 && cpu->pc == 0x8002 && cpu->flags == ZERO;
}

static int test_lda_zero_page(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x0b,0x25);
    mos6502_write8(cpu, 0x8000, 0xA5);
    mos6502_write8(cpu, 0x8001, 0x0b);
    int ticks = mos6502_tick(cpu);
    return  cpu->a == 0x25 && ticks == 3 && cpu->flags == 0 && cpu->pc == 0x8002;
}

static int test_lda_zero_page_x(mos6502_t *cpu)
{
    cpu->x=6;
    mos6502_write8(cpu, 0x0a,0x45);
    mos6502_write8(cpu, 0x8000, 0xB5);
    mos6502_write8(cpu, 0x8001, 0x04);
    int ticks = mos6502_tick(cpu);
    return  cpu->a == 0x45 && ticks == 4 && cpu->flags == 0 && cpu->pc == 0x8002;
}

static int test_lda_absolute(mos6502_t *cpu)
{
   mos6502_write8(cpu, 0x02ff,0x25);
   mos6502_write8(cpu, 0x8000, 0xAD);
   mos6502_write16(cpu, 0x8001,0x02ff);
   int ticks = mos6502_tick(cpu); 

   return  cpu->a == 0x25 && ticks == 4 && cpu->pc == 0x8002 && cpu->flags==0;
}

static int test_lda_absolute_x(mos6502_t *cpu)
{
   cpu->x=5;
   mos6502_write8(cpu, 0x02f8,0x55);
   mos6502_write8(cpu, 0x8000, 0xBD);
   mos6502_write16(cpu, 0x8001,0x02f3);
   int ticks = mos6502_tick(cpu); 

   return  cpu->a == 0x55 && ticks == 4 && cpu->pc == 0x8002;
}

static int test_lda_absolute_y(mos6502_t *cpu)
{
   cpu->y=4;
   mos6502_write8(cpu, 0x02f8,0x22);
   mos6502_write8(cpu, 0x8000, 0xB9);
   mos6502_write16(cpu, 0x8001,0x02f4);
   int ticks = mos6502_tick(cpu); 

   return  cpu->a == 0x22 && ticks == 4 && cpu->pc == 0x8002;
}

static int test_lda_indirect_x(mos6502_t *cpu)
{
   mos6502_write8(cpu, 0x0001,0x04);
   mos6502_write8(cpu, 0x0002,0x05);
   mos6502_write8(cpu, 0x0504,0x85);
   cpu->x=1;  
   mos6502_write8(cpu, 0x8000, 0xA1);
   mos6502_write8(cpu, 0x8001,0x00);
   int ticks = mos6502_tick(cpu); 

   return  cpu->a == 0x85 && ticks == 6 && cpu->pc == 0x8002;
}

static int test_lda_indirect_y(mos6502_t *cpu)
{
   mos6502_write8(cpu, 0x0001,0x04);
   mos6502_write8(cpu, 0x0002,0x05);
   mos6502_write8(cpu, 0x0505,0x85);
   cpu->y=1;  
   mos6502_write8(cpu, 0x8000, 0xB1);
   mos6502_write8(cpu, 0x8001,0x01);

   int ticks = mos6502_tick(cpu); 
   return  cpu->a == 0x85 && ticks == 6 && cpu->pc == 0x8002;
}



void test_mos6502_lda()
{
    RUN_TEST(test_lda_immediate);
    RUN_TEST(test_lda_immediate_negative);
    RUN_TEST(test_lda_immediate_zero);
    RUN_TEST(test_lda_zero_page);
    RUN_TEST(test_lda_zero_page_x);
    RUN_TEST(test_lda_absolute);
    RUN_TEST(test_lda_absolute_x);
    RUN_TEST(test_lda_absolute_y);
    RUN_TEST(test_lda_indirect_x);
    RUN_TEST(test_lda_indirect_y);
}
#endif
