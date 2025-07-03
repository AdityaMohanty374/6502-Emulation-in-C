#include "main6502.h"

int main(){
    CPU cpu;
    Mem memory;
    memory.init();
    memory.Data[0xFFFC] = 0x00;     
    memory.Data[0xFFFD] = 0x80; //PC at 0x8000
    cpu.reset(memory);
    cpu.A = 0xBD;
    cpu.Y = 0xF0;
    memory.Data[0x8000] = CPU::INS_TXA;
    memory.Data[0x8001] = 0x56;
    //memory.Data[0x8002] = 0xAD;
    memory.Data[0x0056] = 0x56;
    memory.Data[0x0057] = 0x60;
    memory.Data[0x6146] = 0x96;
    cpu.execute(2, memory);
    printf("0x%04X\n", cpu.A);
    return 0;
}