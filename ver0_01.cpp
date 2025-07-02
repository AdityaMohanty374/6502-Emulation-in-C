#include "main6502.h"

int main(){
    CPU cpu;
    Mem memory;
    memory.init();
    cpu.reset(memory);
    cpu.A = 0xB4;
    cpu.Y = 0x80;
    memory.Data[0xFFFC] = CPU::INS_DEC_ABS;     
    memory.Data[0xFFFD] = 0x05;
    memory.Data[0xFFFE] = 0x70;
    memory.Data[0x0015] = 0xB4; 
    // memory.Data[0x0016] = 0xAD; 
    memory.Data[0x7015] = 0xB4;
    // memory.Data[0x8043] = 0x60;
    // memory.Data[0x6045] = CPU::INS_LDA_IM;
    // memory.Data[0x6046] = 0x70;
    cpu.execute(2, memory);
    printf("0x%02X\n", cpu.Y);
    return 0;
}