#include "mos6502.h"

static int cpx_immediate(mos6502_t *cpu)
{
    uint8_t immediate = cpu->read(cpu, cpu->pc++); //takes the given value in the next point in the program counter
    uint8_t Xvalue = cpu->x;
    //if the value in the registe x is bigger than the compared one , it will light up the negative and Carry flags
    if (Xvalue > immediate)
    {
         mos6502_set_flag(cpu, NEGATIVE,1);
         mos6502_set_flag(cpu,CARRY,1);
    }
    //if the subtraction gives 0 it will set the 0 flag
    mos6502_set_flag(cpu, ZERO, (immediate - Xvalue) == 0);
    
    return 2;
}

static int cpx_zero_page(mos6502_t *cpu)
{
    //takes the zero page address
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);
    //than takes the value inside of the zero page
    uint8_t value = mos6502_read8(cpu, (uint16_t)zp_address);
    //after that controlls the varius flags to light up after the comparison
    uint8_t Xvalue = cpu->x;
    if (Xvalue > value)
    {
         mos6502_set_flag(cpu, NEGATIVE,1);
         mos6502_set_flag(cpu,CARRY,1);
    }
    mos6502_set_flag(cpu, ZERO, (value - cpu->x) == 0);
    return 3;
}

static int cpx_absolute(mos6502_t *cpu)
{
    //take the low bytes of the address
    uint8_t low_Addr = cpu->read(cpu,cpu->pc++);
    //than takes the high bytes of the addres
    uint8_t high_Addr = cpu->read(cpu,cpu->pc++);
    //combines them to get the full address
    uint16_t AbsoluteAddr = ((uint16_t)high_Addr << 8 | (uint16_t)low_Addr);
    //after that takes the value inside of the given address
    uint8_t value = mos6502_read16(cpu,AbsoluteAddr);
    //check for the flags to light up 
    uint8_t Xvalue = cpu->x;
    if (Xvalue > value)
    {
         mos6502_set_flag(cpu, NEGATIVE,1);
         mos6502_set_flag(cpu,CARRY,1);
    }
    mos6502_set_flag(cpu, ZERO, (value - cpu->x) == 0);
    return 4;
}

//register's the opcodes in the opcodes array
void mos6502_register_cpx(mos6502_t *cpu)
{                          //hex value of the opcode
    mos6502_register_opcode(cpu, 0xE0, cpx_immediate);
    mos6502_register_opcode(cpu, 0xE4, cpx_zero_page);
    mos6502_register_opcode(cpu, 0xEC, cpx_absolute);
}

#ifdef _TEST

static int test_cpx_immediate_ZeroFlag(mos6502_t *cpu)
{
    cpu->x = 0x10; 

    mos6502_write8(cpu, 0x8000, 0xE0);
    mos6502_write8(cpu, 0x8001,0x10);

    int ticks = mos6502_tick(cpu);
    
    return ticks == 2 && cpu->flags == ZERO;
}

static int test_cpx_immediate_NegativeFlag(mos6502_t *cpu)
{
    cpu->x = 0x02; 

    mos6502_write8(cpu, 0x8000, 0xE0);
    mos6502_write8(cpu, 0x8001,0x01);

    int ticks = mos6502_tick(cpu);
    int negativeFlag = mos6502_get_flag(cpu,NEGATIVE);

    return ticks == 2 && negativeFlag == 1 ;
}

static int test_cpx_immediate_CarryFlag(mos6502_t *cpu)
{
    cpu->x = 0x11; 
    mos6502_write8(cpu, 0x8000, 0xE0);
    mos6502_write8(cpu, 0x8001,0x10);

    int ticks = mos6502_tick(cpu);
    int CarryFlag = mos6502_get_flag(cpu,CARRY);

    return ticks == 2 && CarryFlag == 1;
}

static int test_cpx_ZeroPage_Zero(mos6502_t *cpu)
{
    cpu->x = 0x10; 
    
    mos6502_write8(cpu, 0x0001, 0x10);
    mos6502_write8(cpu, 0x8000, 0xE4);
    mos6502_write8(cpu, 0x8001,0x01);

    int ticks = mos6502_tick(cpu);
    int ZeroFlag = mos6502_get_flag(cpu,ZERO);

    return ticks == 3 &&  ZeroFlag == 1;
}

static int test_cpx_ZeroPage_Negative(mos6502_t *cpu)
{
    cpu->x = 0x10; 
    
    mos6502_write8(cpu, 0x0001, 0x01);
    mos6502_write8(cpu, 0x8000, 0xE4);
    mos6502_write8(cpu, 0x8001,0x01);

    int ticks = mos6502_tick(cpu);
    int negativeFlag = mos6502_get_flag(cpu,NEGATIVE);

    return ticks == 3 && negativeFlag == 1;
}

static int test_cpx_ZeroPage_Carry(mos6502_t *cpu)
{
    cpu->x = 0x10; 
    
    mos6502_write8(cpu, 0x0001, 0x01);
    mos6502_write8(cpu, 0x8000, 0xE4);
    mos6502_write8(cpu, 0x8001,0x01);

    int ticks = mos6502_tick(cpu);
    int CarryFlag = mos6502_get_flag(cpu,CARRY);

    return ticks == 3 && CarryFlag == 1;
}

static int test_cpx_Absolute_Zero(mos6502_t *cpu)
{
    cpu->x = 0x10; 
    
    mos6502_write8(cpu, 0x5001, 0x10);
    mos6502_write8(cpu, 0x8000, 0xEC);
    mos6502_write8(cpu, 0x8001,0x01);
    mos6502_write8(cpu, 0x8002,0x50);
    
    int ticks = mos6502_tick(cpu);
    int ZeroFlag = mos6502_get_flag(cpu,ZERO);

    return ticks == 4 && ZeroFlag == 1;
}

static int test_cpx_Absolute_Negative(mos6502_t *cpu)
{
    cpu->x = 0x10; 
    
    mos6502_write8(cpu, 0x0001, 0x01);
    mos6502_write8(cpu, 0x8000, 0xEC);
    mos6502_write8(cpu, 0x8001,0x01);
    mos6502_write8(cpu, 0x8002,0x50);

    int ticks = mos6502_tick(cpu);
    int NegativeFlag = mos6502_get_flag(cpu,NEGATIVE);

    return ticks == 4 && NegativeFlag == 1;
}

static int test_cpx_Absolute_Carry(mos6502_t *cpu)
{
    cpu->x = 0x10; 
    
    mos6502_write8(cpu, 0x0001, 0x01);
    mos6502_write8(cpu, 0x8000, 0xEC);
    mos6502_write8(cpu, 0x8001,0x01);
    mos6502_write8(cpu, 0x8002,0x50);

    int ticks = mos6502_tick(cpu);
    int CarryFlag = mos6502_get_flag(cpu,CARRY);

    return ticks == 4 && CarryFlag == 1;
}
void test_mos6502_cpx()
{
    RUN_TEST(test_cpx_immediate_ZeroFlag);
    RUN_TEST(test_cpx_immediate_NegativeFlag);
    RUN_TEST(test_cpx_immediate_CarryFlag);
    RUN_TEST(test_cpx_ZeroPage_Zero);
    RUN_TEST(test_cpx_ZeroPage_Negative);
    RUN_TEST(test_cpx_ZeroPage_Carry);
    RUN_TEST(test_cpx_Absolute_Zero);
    RUN_TEST(test_cpx_Absolute_Negative);
    RUN_TEST(test_cpx_Absolute_Carry);
    //TODO ,Remaining tests with zero page and absolute
}
#endif

