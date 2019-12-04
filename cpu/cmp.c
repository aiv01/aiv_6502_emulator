#include "mos6502.h"

//Compare sets flags as if a subtraction had been carried out. 
//If the value in the accumulator is equal or greater than the compared value, the Carry will be set. 
//The equal (Z) and negative (N) flags will be set based on equality or lack thereof and the sign (i.e. A>=$80) of the accumulator.

static int cmp_immediate(mos6502_t *cpu){
    uint8_t immediate = cpu->read(cpu, cpu->pc++);   
    int sub_value = cpu->a - immediate;
    mos6502_set_flag(cpu, CARRY, cpu->a >= immediate);
    //if CARRY == 0 then A < value
    //  ->  can BCC
    //else if CARRY == 1 then A >= value
    //  ->  can BCS

    mos6502_set_flag(cpu, NEGATIVE, sub_value < 0);
    //The N flag contains most significant bit 
    //of the of the subtraction result.

    mos6502_set_flag(cpu, ZERO, sub_value == 0);
    //if ZERO == 0 then A != value
    // -> can BNE
    //else if ZERO == 1 then A == value
    // -> can BEQ

    return 2;
}
static int cmp_zero_page(mos6502_t *cpu){
    uint8_t zp_address = cpu->read(cpu, cpu->pc++);   
    uint8_t zp_value = mos6502_read8(cpu, (uint16_t)zp_address);

    int sub_value = cpu->a - zp_value;

    mos6502_set_flag(cpu, CARRY, cpu->a >= zp_value);
    mos6502_set_flag(cpu, NEGATIVE, sub_value < 0);
    mos6502_set_flag(cpu, ZERO, sub_value == 0);
    
    return 3;
}
static int cmp_absolute(mos6502_t *cpu){
    uint8_t low = cpu->read(cpu, cpu->pc++);
    uint8_t high = cpu->read(cpu, cpu->pc++);
    uint16_t absolute = ((uint8_t)high << 8 | (uint8_t)low);
    uint16_t value = mos6502_read16(cpu, absolute);

    int sub_value = cpu->a - value;

    mos6502_set_flag(cpu, CARRY, cpu->a >= value);
    mos6502_set_flag(cpu, NEGATIVE, sub_value < 0);
    mos6502_set_flag(cpu, ZERO, sub_value == 0);

    return 4;
}
static int cmp_absolute_x(mos6502_t *cpu){
    uint8_t low = cpu->read(cpu, cpu->pc++);
    uint8_t high = cpu->read(cpu, cpu->pc++);
    uint16_t absolute = ((uint8_t)high << 8 | (uint8_t)low);
    uint16_t value = mos6502_read16(cpu, absolute);
    value += cpu->x;

    int sub_value = cpu->a - value;

    mos6502_set_flag(cpu, CARRY, cpu->a >= value);
    mos6502_set_flag(cpu, NEGATIVE, sub_value < 0);
    mos6502_set_flag(cpu, ZERO, sub_value == 0);

    return 4;
}
static int cmp_absolute_y(mos6502_t *cpu){
    uint8_t low = cpu->read(cpu, cpu->pc++);
    uint8_t high = cpu->read(cpu, cpu->pc++);
    uint16_t absolute = ((uint8_t)high << 8 | (uint8_t)low);
    uint16_t value = mos6502_read16(cpu, absolute);
    value += cpu->y;

    int sub_value = cpu->a - value;

    mos6502_set_flag(cpu, CARRY, cpu->a >= value);
    mos6502_set_flag(cpu, NEGATIVE, sub_value < 0);
    mos6502_set_flag(cpu, ZERO, sub_value == 0);

    return 4;
}

void mos6502_register_cmp(mos6502_t *cpu){
    mos6502_register_opcode(cpu, 0xC9, cmp_immediate);

    mos6502_register_opcode(cpu, 0xC5, cmp_zero_page);

    mos6502_register_opcode(cpu, 0xCD, cmp_absolute);
    mos6502_register_opcode(cpu, 0xDD, cmp_absolute_x);
    mos6502_register_opcode(cpu, 0xD9, cmp_absolute_y);
}

#ifdef _TEST
//test immediate
static int test_cmp_a_greater_immediate(mos6502_t *cpu){
    cpu->a = 8;
    mos6502_write8(cpu, 0x8000, 0xC9);
    mos6502_write8(cpu, 0x8001, 0x04);
    int ticks = mos6502_tick(cpu);
    int zero_flag = mos6502_get_flag(cpu, ZERO);
    int carry_flag = mos6502_get_flag(cpu, CARRY);
    int negative_flag = mos6502_get_flag(cpu, NEGATIVE);
    return zero_flag == 0 && carry_flag == 1 && negative_flag == 0;
}
static int test_cmp_a_less_immediate(mos6502_t *cpu){
    cpu->a = 3;
    mos6502_write8(cpu, 0x8000, 0xC9);
    mos6502_write8(cpu, 0x8001, 0x04);
    int ticks = mos6502_tick(cpu);
    int zero_flag = mos6502_get_flag(cpu, ZERO);
    int carry_flag = mos6502_get_flag(cpu, CARRY);
    int negative_flag = mos6502_get_flag(cpu, NEGATIVE);
    return zero_flag == 0 && carry_flag == 0 && negative_flag == 1;
}
static int test_cmp_a_equal_immediate(mos6502_t *cpu){
    cpu->a = 8;
    mos6502_write8(cpu, 0x8000, 0xC9);
    mos6502_write8(cpu, 0x8001, 0x08);
    int ticks = mos6502_tick(cpu);
    int zero_flag = mos6502_get_flag(cpu, ZERO);
    int carry_flag = mos6502_get_flag(cpu, CARRY);
    int negative_flag = mos6502_get_flag(cpu, NEGATIVE);
    return zero_flag == 1 && carry_flag == 1 && negative_flag == 0;
}
//test absolute, absolute_x, absolute_y
static int test_cmp_greater_absolute(mos6502_t *cpu){
    cpu->a = 8;
    mos6502_write8(cpu, 0x8000, 0xCD);
    mos6502_write8(cpu, 0x8001, 0x80);
    int ticks = mos6502_tick(cpu);
    int zero_flag = mos6502_get_flag(cpu, ZERO);
    int carry_flag = mos6502_get_flag(cpu, CARRY);
    int negative_flag = mos6502_get_flag(cpu, NEGATIVE);
    return zero_flag == 0 && carry_flag == 1 && negative_flag == 0;
}
static int test_cmp_less_absolute_x(mos6502_t *cpu){
    cpu->a = 3;
    cpu->x = 8;
    mos6502_write8(cpu, 0x8000, 0xDD);
    mos6502_write8(cpu, 0x8001, 0x80);
    int ticks = mos6502_tick(cpu);
    int zero_flag = mos6502_get_flag(cpu, ZERO);
    int carry_flag = mos6502_get_flag(cpu, CARRY);
    int negative_flag = mos6502_get_flag(cpu, NEGATIVE);
    return zero_flag == 0 && carry_flag == 0 && negative_flag == 1;
}
static int test_cmp_equal_absolute_y(mos6502_t *cpu){
    cpu->a = 8;
    cpu->y = 8;
    mos6502_write8(cpu, 0x8000, 0xD9);
    mos6502_write8(cpu, 0x8001, 0x80);
    int ticks = mos6502_tick(cpu);
    int zero_flag = mos6502_get_flag(cpu, ZERO);
    int carry_flag = mos6502_get_flag(cpu, CARRY);
    int negative_flag = mos6502_get_flag(cpu, NEGATIVE);
    return zero_flag == 1 && carry_flag == 1 && negative_flag == 0;
}

void test_mos6502_cmp(){
    //test immediate
    RUN_TEST(test_cmp_a_greater_immediate);
    RUN_TEST(test_cmp_a_less_immediate);
    RUN_TEST(test_cmp_a_equal_immediate);
    //test absolute, absolute_x, absolute_y
    RUN_TEST(test_cmp_greater_absolute);
    RUN_TEST(test_cmp_less_absolute_x);
    RUN_TEST(test_cmp_equal_absolute_y);
}
#endif