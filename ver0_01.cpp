#include "main6502.h"

int main(){
    CPU cpu;
    Mem memory;
    memory.init();
    cpu.reset(memory);
    cpu.A = 0xBC;
    cpu.Y = 0xF0;
    memory.Data[0xFFFC] = CPU::INS_AND_INDY;     
    memory.Data[0xFFFD] = 0x05;
    //memory.Data[0xFFFE] = 0x70;
    memory.Data[0x0005] = 0x18; 
    memory.Data[0x0006] = 0xAD; 
    memory.Data[0xAE08] = 0x35;
    // memory.Data[0x8043] = 0x60;
    // memory.Data[0x6045] = CPU::INS_LDA_IM;
    // memory.Data[0x6046] = 0x70;
    cpu.execute(6, memory);
    printf("0x%02X\n",cpu.A);
    return 0;
}