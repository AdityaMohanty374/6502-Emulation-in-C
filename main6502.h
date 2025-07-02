#include<stdio.h>
#include<stdlib.h>
#include<iostream>

using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;

struct Mem{
    static constexpr u32 MAX_MEM = 1024*64;
    Byte Data[MAX_MEM];
    void init(){
        for(u32 i = 0; i<MAX_MEM; i++){
            Data[i] = 0;
        }
    }
    void WriteByte(Byte Value, Word Address, u32 &cycles){
        Data[Address] = Value;
        cycles--;
    }
    void WriteWord(Word Value, Word Address, u32 &cycles){
        Data[Address] = Value & 0xFF;
        Data[Address+1] = (Value>>8);
        cycles-=2;
    }
};

struct CPU{
    Word PC;        //program counter
    Byte SP;        //stack pointer

    Byte SR;        //Status Register
    Byte A, X, Y;   //registers

    //Status Register Org
    /*
      |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  | 
      |  N  |  V  |     |  B  |  D  |  I  |  Z  |  C  | 
    */
    Byte C : 1;     //status flag
    Byte I : 1;
    Byte D : 1;
    Byte B : 1;
    Byte Z : 1;
    Byte V : 1;
    Byte N : 1;

    Byte UserBit:1;  //multipurpose

    static constexpr Byte INS_LDA_IM = 0xA9,            //Immediate Addressing LDA
                          INS_LDA_ZP = 0xA5,            //ZeroPage Addressing LDA
                          INS_LDA_ZPX = 0xB5,           //ZeroPageX Addressing LDA
                          INS_LDA_ABS = 0xAD,           //Absolute Addressing LDA
                          INS_LDA_ABSX = 0xBD,          //AbsoluteX Addressing LDA
                          INS_LDA_ABSY = 0xB9,          //AbsoluteY Addressing LDA
                          INS_LDA_INDX = 0xA1,          //(IndirectX) Addressing LDA
                          INS_LDA_INDY = 0xB1,          //(Indirect)Y Addressing LDA
                          
                          INS_LDX_IM = 0xA2,            //Immediate Addressing LDX
                          INS_LDX_ZP = 0xA6,            //ZeroPage Addressing LDX
                          INS_LDX_ZPY = 0xB6,           //ZeroPageY Addressing LDX
                          INS_LDX_ABS = 0xAE,           //Absolute Addressing LDX
                          INS_LDX_ABSY = 0xBE,          //AbsoluteY Addressing LDx

                          INS_LDY_IM = 0xA0,            //Immediate Addressing LDY
                          INS_LDY_ZP = 0xA4,            //ZeroPage Addressing LDY
                          INS_LDY_ZPX = 0xB4,           //ZeroPageX Addressing LDY
                          INS_LDY_ABS = 0xAC,           //Absolute Addressing LDY
                          INS_LDY_ABSX = 0xBC,          //AbsoluteX Addressing LDY
                          
                          INS_STA_ZP = 0x85,
                          INS_STA_ZPX = 0x95,
                          INS_STA_ABS = 0x8D,
                          INS_STA_ABSX = 0x9D,
                          INS_STA_ABSY = 0x99,
                          INS_STA_INDX = 0x81,
                          INS_STA_INDY = 0x91,

                          INS_TAX = 0xAA,
                          INS_TAY = 0xA8,
                          INS_TSX = 0xBA,
                          INS_TXA = 0x8A,

                          INS_STX_ZP = 0x86,
                          INS_STX_ZPY = 0x96,
                          INS_STX_ABS = 0x8E,

                          INS_STY_ZP = 0x84,
                          INS_STY_ZPX = 0x94,
                          INS_STY_ABS = 0x8C,

                          INS_ASL_ACC = 0X0A,
                          INS_ASL_ZP = 0x06,
                          INS_ASL_ZPX = 0x16,
                          INS_ASL_ABS = 0x0E,
                          INS_ASL_ABSX = 0x1E,

                          INS_LSR_ACC = 0x4A,
                          INS_LSR_ZP = 0x46,
                          INS_LSR_ZPX = 0x56,
                          INS_LSR_ABS = 0x4E,
                          INS_LSR_ABSX = 0x5E,

                          INS_ROL_ACC = 0x2A,
                          INS_ROL_ZP = 0x26,
                          INS_ROL_ZPX = 0x36,
                          INS_ROL_ABS = 0x2E,
                          INS_ROL_ABSX = 0x3E,

                          INS_ROR_ACC = 0x6A,
                          INS_ROR_ZP = 0x66,
                          INS_ROR_ZPX = 0x76,
                          INS_ROR_ABS = 0x6E,
                          INS_ROR_ABSX = 0x7E,

                          INS_AND_IM = 0x29,
                          INS_AND_ZP = 0x25,
                          INS_AND_ZPX = 0x35,
                          INS_AND_ABS = 0x2D,
                          INS_AND_ABSX = 0x3D,
                          INS_AND_ABSY = 0x39,
                          INS_AND_INDX = 0x21,
                          INS_AND_INDY = 0x31,

                          INS_ADC_IM = 0x69,
                          INS_ADC_ZP = 0x65,
                          INS_ADC_ZPX = 0x75,
                          INS_ADC_ABS = 0x6D,
                          INS_ADC_ABSX = 0x7D,
                          INS_ADC_ABSY = 0x79,
                          INS_ADC_INDX = 0x61,
                          INS_ADC_INDY = 0x71, 

                          INS_SBC_IM = 0xE9,
                          INS_SBC_ZP = 0xE5,
                          INS_SBC_ZPX = 0xF5,
                          INS_SBC_ABS = 0xED,
                          INS_SBC_ABSX = 0xFD,
                          INS_SBC_ABSY = 0xF9,
                          INS_SBC_INDX = 0xE1,
                          INS_SBC_INDY = 0xF1,

                          INS_BCC = 0x90,
                          INS_BCS = 0xB0,
                          INS_BEQ = 0xF0,
                          INS_BMI = 0x30,
                          INS_BNE = 0xD0,
                          INS_BPL = 0x10,
                          INS_BVC = 0x50,
                          INS_BVS = 0x70,

                          INS_BIT_ZP = 0x24,
                          INS_BIT_ABS = 0x2C,

                          INS_BRK = 0x00,

                          INS_CLC = 0x18,
                          INS_CLD = 0xD8,
                          INS_CLI = 0x58,
                          INS_CLV = 0xB8,

                          INS_CMP_IM = 0xC9,
                          INS_CMP_ZP = 0xC5,
                          INS_CMP_ZPX = 0xD5,
                          INS_CMP_ABS = 0xCD,
                          INS_CMP_ABSX = 0xDD,
                          INS_CMP_ABSY = 0xD9,
                          INS_CMP_INDX = 0xC1,
                          INS_CMP_INDY = 0xD1,

                          INS_CPX_IM = 0xE0,
                          INS_CPX_ZP = 0xE4,
                          INS_CPX_ABS = 0xEC,

                          INS_CPY_IM = 0xC0,
                          INS_CPY_ZP = 0xC4,
                          INS_CPY_ABS = 0xCC,

                          INS_DEC_ZP = 0xC6,
                          INS_DEC_ZPX = 0xD6, 
                          INS_DEC_ABS = 0xCE,
                          INS_DEC_ABSX = 0xDE,

                          INS_DEX = 0xCA,
                          INS_DEY = 0x88,

                          INS_EOR_IM = 0x49,
                          INS_EOR_ZP = 0x45,
                          INS_EOR_ZPX = 0x55,
                          INS_EOR_ABS = 0x4D,
                          INS_EOR_ABSX = 0x5D,
                          INS_EOR_ABSY = 0x59,
                          INS_EOR_INDX = 0x41,
                          INS_EOR_INDY = 0x51,

                          INS_ORA_IM = 0x09,
                          INS_ORA_ZP = 0x05,
                          INS_ORA_ZPX = 0x15,
                          INS_ORA_ABS = 0x0D,
                          INS_ORA_ABSX = 0x1D,
                          INS_ORA_ABSY = 0x19,
                          INS_ORA_INDX = 0x01,
                          INS_ORA_INDY = 0x11,
                          
                          INS_PHA = 0x48,
                          INS_PHP = 0x08,
                          INS_PLA = 0x68,
                          INS_PLP = 0x28,

                          INS_RTI = 0x40,
                          INS_RTS = 0x60, 

                          INS_SEC = 0x38,
                          INS_SED = 0xF8,
                          INS_SEI = 0x78,

                          INS_INC_ZP = 0xE6,
                          INS_INC_ZPX = 0xF6,
                          INS_INC_ABS = 0xEE,
                          INS_INC_ABSX = 0xFE,

                          INS_INX = 0xE8,
                          INS_INY = 0xC8,
                          
                          INS_JSR = 0x20,               //Jump Subroutine
                          INS_JMP_ABS = 0x4C,           //Jump Absolute Addressing
                          INS_JMP_IND = 0x6C,           //Jump Indirect Addressing 

                          INS_NOP = 0XEA;

    void reset(Mem &memory){
        Byte low  = memory.Data[0xFFFC];
        Byte high = memory.Data[0xFFFD];
        PC = (high << 8) | low;
        SP = 0xFD;
        SR = 0x24;  //bit 5 always set with interrupt disable ie bit 2
        C = Z = I = B = V = N = D = 0;
        A = X = Y = 0;
        UserBit = 0;
        memory.init();
    }
    void LDASetStatus(){
        Z = (A==0);
        N = (A & 0b10000000)>0;
        SR &= ~(0x02 | 0x80);
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
    }
    void LDXSetStatus(){
        Z = (X==0);
        N = (X & 0b10000000)>0;
        SR &= ~(0x02 | 0x80);
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
    }
    void LDYSetStatus(){
        Z = (Y==0);
        N = (Y & 0b10000000)>0;
        SR &= ~(0x02 | 0x80);
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
    }
    void ASLSetStatus(Byte result){
        Z = (A == 0);
        N = (result & 0b10000000)>0;
        SR &= ~(0x02 | 0x80);
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
    }
    void BITSetStatus(Byte operand){
        N = (operand>>6)&1;
        V = (operand>>5)&1;
        Z = (A&operand==0);
        SR &= ~(0x40 | 0x02 | 0x80);
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
        if (V) SR |= 0x40;
    }
    void SBCSetStatus(Word Res, Byte Val){
        C = (Res<0x100);
        V = ((A^Val)&0x80)&((A^Res)&0x80); //if A and Val have opp signs AND If A and Res have different signs --> Overflow
        Z = (Res == 0);
        N = ((Res & 0x80)) != 0;
        if(C) SR|=0x01;
        if(V) SR|=0x40;
        if(Z) SR|=0x02;
        if(N) SR|=0x80;
    }
    void CMPSetStatus(Byte Value, Byte Acc) {
        Byte result = Acc - Value;
        C = (Acc >= Value);
        Z = (Acc == Value);
        N = (result & 0x80) != 0;
        SR &= ~(0x01 | 0x02 | 0x80);
        if (C) SR |= 0x01;
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
    }
    void DECSetStatus(Byte Val){
        Byte Res = Val-1;
        N = (Res & 0x80)!=0;
        Z = (Res==0);
        SR &= ~(0x02 | 0x80);
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
    }
    void INCSetStatus(Byte Val){
        Byte Res = Val+1;
        N = (Res & 0x80)!=0;
        Z = (Res==0);
        SR &= ~(0x02 | 0x80);
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
    }
    void EORSetStatus(Byte Val, Byte Acc){
        Byte Res = Acc^Val;
        Z = (Acc == Val);
        N = (Res & 0x80) != 0;
        SR &= ~(0x02 | 0x80);
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
    }
    void ORASetStatus(Byte Val, Byte Acc){
        Byte Res = Acc|Val;
        Z = (Res == 0);
        N = (Res & 0x80) != 0;
        SR &= ~(0x02 | 0x80);
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
    }
    void PLASetStatus(){
        Z = (A == 0);
        N = (A & 0x80) != 0;
        SR &= ~(0x02 | 0x80);
        if (Z) SR |= 0x02;
        if (N) SR |= 0x80;
    }
    void PLPSetStatus(){
        C = (SR&0x01) != 0;
        Z = (SR&0x02) != 0;
        I = (SR&0x04) != 0;
        D = (SR&0x08) != 0;
        V = (SR&0x40) != 0;
        N = (SR&0x80) != 0;
    }
    bool pageChange(Word value, Byte B){
        Byte LL = value & 0x00FF;
        return ((LL+B)>0xFF);
    }
    Byte FetchByte(u32 &cycles,Mem &memory){
        Byte Data = memory.Data[PC];
        PC++;
        cycles--;
        return Data;
    }
    Word FetchWord(u32 &cycles, Mem &memory){
        //6502 is little endian
        Word Data = memory.Data[PC];
        PC++;
        Data |= (memory.Data[PC]<<8); 
        cycles-=2;
        //to handle big endianness swap bytes here
        return Data;
    }
    
    Byte ReadByte(u32 &cycles,Mem &memory, Byte ZPAddress){
        Byte Data = memory.Data[ZPAddress];
        cycles--;
        return Data;
    }
    Word ReadWord(u32 &cycles, Mem &memory, Word ZPAddress){
        Word Data = 0x0000;
        Data |= memory.Data[ZPAddress];
        cycles--;
        ZPAddress++;
        Data |= (memory.Data[ZPAddress]<<8);
        cycles--;
        return Data;
    }
    Byte ReadByte_ABS(u32 &cycles, Mem &memory, Word ABSaddr){
        Byte Data = memory.Data[ABSaddr];
        cycles--;
        return Data;
    }
    
    void execute(u32 cycles, Mem &memory){
        while(cycles>0){
            Byte Ins = FetchByte(cycles, memory);
            switch(Ins){
                case INS_LDA_IM:
                {
                    Byte Value = FetchByte(cycles, memory);
                    A = Value;
                    LDASetStatus();
                }
                break;
                case INS_LDA_ZP:
                {
                    Byte ZeroPageAddress = FetchByte(cycles, memory);
                    A = ReadByte(cycles, memory, ZeroPageAddress);
                    LDASetStatus();
                }
                break;
                case INS_LDA_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF; 
                    cycles--;
                    A = ReadByte(cycles, memory, ZPAddr);
                    LDASetStatus();
                }
                case INS_LDA_ABS:
                {
                    Word SubAddr = FetchWord(cycles, memory); 
                    A = ReadByte_ABS(cycles, memory, SubAddr);
                    LDASetStatus();
                }
                break;
                case INS_LDA_ABSX:
                {
                    Word SubAddr = FetchWord(cycles, memory); 
                    if(pageChange(SubAddr, X)) cycles--;
                    SubAddr+=X;
                    A = ReadByte_ABS(cycles, memory, SubAddr);
                    LDASetStatus();
                }
                break;
                case INS_LDA_ABSY:
                {
                    Word SubAddr = FetchWord(cycles, memory); 
                    if(pageChange(SubAddr, Y)) cycles--;
                    SubAddr+=Y;
                    A = ReadByte_ABS(cycles, memory, SubAddr);
                    LDASetStatus();
                }
                break;
                case INS_LDA_INDX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    cycles--;
                    Word LoadAddress = ReadWord(cycles, memory, ZPAddr);
                    A = memory.Data[LoadAddress];
                    cycles--;
                    LDASetStatus();
                }
                break;
                case INS_LDA_INDY:
                {
                    Byte ZPAddress = FetchByte(cycles, memory);
                    Word LoadAddress = ReadWord(cycles, memory, ZPAddress);
                    if(pageChange(LoadAddress, Y)) cycles--;
                    LoadAddress+=Y;
                    A = memory.Data[LoadAddress];
                    cycles--;
                    LDASetStatus();
                }
                break;
                //LDX
                case INS_LDX_IM:
                {
                    Byte Value = FetchByte(cycles, memory);
                    X = Value;
                    LDXSetStatus();
                }
                break;
                case INS_LDX_ZP:
                {
                    Byte ZeroPAddress = FetchByte(cycles, memory);
                    X = ReadByte(cycles, memory, ZeroPAddress);
                    LDXSetStatus();
                }
                break;
                case INS_LDX_ZPY:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF; //Not handled: IF ADDRESS OVERFLOWS
                    cycles--;
                    X = ReadByte(cycles, memory, ZPAddr);
                    LDXSetStatus();
                }
                break;
                case INS_LDX_ABS:
                {
                    Word SubAddr = FetchWord(cycles, memory); 
                    X = ReadByte_ABS(cycles, memory, SubAddr);
                    LDXSetStatus();
                }
                break;
                //Test Case: Failed (consumes a cycle when page isnt crossed)
                case INS_LDX_ABSY:
                {
                    Word SubAddr = FetchWord(cycles, memory); 
                    if(pageChange(SubAddr, Y)) cycles--;
                    SubAddr+=Y;
                    X = ReadByte_ABS(cycles, memory, SubAddr);
                    LDXSetStatus();
                }
                //LDY
                case INS_LDY_IM:
                {
                    Byte Value = FetchByte(cycles, memory);
                    Y = Value;
                    LDYSetStatus();
                }
                break;
                case INS_LDY_ZP:
                {
                    Byte ZeroPageAddress = FetchByte(cycles, memory);
                    Y = ReadByte(cycles, memory, ZeroPageAddress);
                    LDYSetStatus();
                }
                break;
                case INS_LDY_ZPX:
                {
                    Byte ZeroPageAddress = FetchByte(cycles, memory);
                    ZeroPageAddress+=X; //Not handled: IF ADDRESS OVERFLOWS
                    cycles--;
                    Y = ReadByte(cycles, memory, ZeroPageAddress);
                    LDYSetStatus();
                }
                break;
                case INS_LDY_ABS:
                {
                    Word SubAddr = FetchWord(cycles, memory); 
                    Y = ReadByte_ABS(cycles, memory, SubAddr);
                    LDYSetStatus();
                }
                break;
                case INS_LDY_ABSX:
                {
                    Word SubAddr = FetchWord(cycles, memory); 
                    if(pageChange(SubAddr, X)) cycles--;
                    SubAddr+=X;
                    Y = ReadByte_ABS(cycles, memory, SubAddr);
                    LDYSetStatus();
                }
                break;
                //STA
                case INS_STA_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    memory.WriteByte(A, ZPAddr, cycles);
                }
                break;
                case INS_STA_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr+=X;
                    cycles--;
                    memory.WriteByte(A, ZPAddr, cycles);
                }
                break;
                case INS_STA_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    memory.WriteByte(A, AbsAddr, cycles);
                }
                break;
                case INS_STA_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    AbsAddr+=X;
                    cycles--;
                    memory.WriteByte(A, AbsAddr, cycles);
                }
                break;
                case INS_STA_ABSY:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    AbsAddr+=Y;
                    cycles--;
                    memory.WriteByte(A, AbsAddr, cycles);
                }
                break;
                case INS_STA_INDX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr+X)&0xFF;//memory overflow not handled
                    cycles--;
                    Word LoadAddr = ReadWord(cycles, memory, ZPAddr);
                    memory.WriteByte(A, LoadAddr, cycles);
                }
                break;
                case INS_STA_INDY:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Word LoadAddr = ReadWord(cycles, memory, ZPAddr);
                    LoadAddr+=Y;
                    cycles--;
                    memory.WriteByte(A, LoadAddr, cycles);
                }
                break;
                //STX
                case INS_STX_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    memory.WriteByte(X, ZPAddr, cycles);
                }
                break;
                case INS_STX_ZPY:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr+=Y;
                    cycles--;
                    memory.WriteByte(X, ZPAddr, cycles);
                }
                break;
                case INS_STX_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    memory.WriteByte(X, AbsAddr, cycles);
                }
                break;
                //STY
                case INS_STY_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    memory.WriteByte(Y, ZPAddr, cycles);
                }
                break;
                case INS_STY_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr+X)&0xFF;
                    cycles--;
                    memory.WriteByte(Y, ZPAddr, cycles);
                }
                break;
                case INS_STY_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    memory.WriteByte(Y, AbsAddr, cycles);
                }
                break;
                //TAX
                case INS_TAX:
                {
                    X = A&0xFF;
                    cycles--;
                    Z = (X==0);
                    N = (X&0x80)!=0;
                    SR &= ~(0x82);
                    if (Z) SR |= 0x02;
                    if (N) SR |= 0x80;
                }
                break;
                //TAY
                case INS_TAY:
                {
                    Y = A&0xFF;
                    cycles--;
                    Z = (Y==0);
                    N = (Y&0x80)!=0;
                    SR &= ~(0x82);
                    if (Z) SR |= 0x02;
                    if (N) SR |= 0x80;
                }
                break;
                //TSX
                case INS_TSX:
                {
                    X = SP;
                    Z = (X == 0);
                    N = (X & 0x80) != 0;
                    SR &= ~(0x82);
                    if (Z) SR |= 0x02;
                    if (N) SR |= 0x80;
                    cycles--;
                }
                break;
                case INS_TXA:
                {
                    A = X&0xFF;
                    cycles--;
                    Z = (A==0);
                    N = (A&0x80)!=0;
                    SR &= ~(0x82);
                    if (Z) SR |= 0x02;
                    if (N) SR |= 0x80;
                }
                break;
                //ASL
                case INS_ASL_ACC:
                {
                    C = (A >> 7) & 1;
                    A <<= 1;
                    cycles--;
                    ASLSetStatus(A);
                }
                break;
                case INS_ASL_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte result = memory.Data[ZPAddr];
                    cycles--;
                    C = (result>> 7) & 1;
                    result <<= 1;
                    cycles--;
                    memory.Data[ZPAddr] = result;
                    cycles--; 
                    ASLSetStatus(result);
                }
                case INS_ASL_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    cycles--;
                    Byte result = memory.Data[ZPAddr];
                    cycles--;
                    C = (result>> 7) & 1;
                    result <<= 1;
                    cycles--;
                    memory.Data[ZPAddr] = result;
                    cycles--;
                    ASLSetStatus(result);
                }
                break;
                case INS_ASL_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte result = memory.Data[AbsAddr];
                    cycles--;
                    C = (result>> 7) & 1;
                    result <<= 1;
                    cycles--;
                    memory.Data[AbsAddr] = result;
                    cycles--; 
                    ASLSetStatus(result);
                }
                break;
                case INS_ASL_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    AbsAddr+=X;
                    cycles--;
                    Byte result = memory.Data[AbsAddr];
                    cycles--;
                    C = (result>> 7) & 1;
                    result <<= 1;
                    cycles--;
                    memory.Data[AbsAddr] = result;
                    cycles--; 
                    ASLSetStatus(result);
                }
                break;
                //LSR
                case INS_LSR_ACC:
                {
                    C = A & 1;
                    A >>= 1;
                    cycles--;
                    ASLSetStatus(A);
                }
                break;
                case INS_LSR_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte result = memory.Data[ZPAddr];
                    cycles--;
                    C = (result) & 1;
                    result >>= 1;
                    cycles--;
                    memory.Data[ZPAddr] = result;
                    cycles--; 
                    ASLSetStatus(result);
                }
                case INS_LSR_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    cycles--;
                    Byte result = memory.Data[ZPAddr];
                    cycles--;
                    C = (result) & 1;
                    result >>= 1;
                    cycles--;
                    memory.Data[ZPAddr] = result;
                    cycles--;
                    ASLSetStatus(result);
                }
                break;
                case INS_LSR_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte result = memory.Data[AbsAddr];
                    cycles--;
                    C = (result) & 1;
                    result >>= 1;
                    cycles--;
                    memory.Data[AbsAddr] = result;
                    cycles--; 
                    ASLSetStatus(result);
                }
                break;
                case INS_LSR_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    AbsAddr+=X;
                    cycles--;
                    Byte result = memory.Data[AbsAddr];
                    cycles--;
                    C = (result) & 1;
                    result >>= 1;
                    cycles--;
                    memory.Data[AbsAddr] = result;
                    cycles--; 
                    ASLSetStatus(result);
                }
                break;
                //ROL
                case INS_ROL_ACC:
                {
                    UserBit = (A>>7)&1;
                    A <<= 1;
                    cycles--;
                    A |= C;
                    C = UserBit;
                    ASLSetStatus(A);
                }
                break;
                case INS_ROL_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte result = memory.Data[ZPAddr];
                    cycles--;
                    UserBit = (result>>7)&1;
                    result <<= 1;
                    cycles--;
                    result |= C;
                    C = UserBit;
                    memory.Data[ZPAddr] = result;
                    cycles--;
                    ASLSetStatus(result);
                }
                break;
                case INS_ROL_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    cycles--;
                    Byte result = memory.Data[ZPAddr];
                    cycles--;
                    UserBit = (result>>7)&1;
                    result <<= 1;
                    cycles--;
                    result |= C;
                    C = UserBit;
                    memory.Data[ZPAddr] = result;
                    cycles--;
                    ASLSetStatus(result);
                }
                break;
                case INS_ROL_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte result = memory.Data[AbsAddr];
                    cycles--;
                    UserBit = (result>>7)&1;
                    result <<= 1;
                    cycles--;
                    result |= C;
                    C = UserBit;
                    memory.Data[AbsAddr] = result;
                    cycles--;
                    ASLSetStatus(result);
                }
                break;
                case INS_ROL_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    AbsAddr+=X;
                    cycles--;
                    Byte result = memory.Data[AbsAddr];
                    cycles--;
                    UserBit = (result>>7)&1;
                    result <<= 1;
                    cycles--;
                    result |= C;
                    C = UserBit;
                    memory.Data[AbsAddr] = result;
                    cycles--;
                    ASLSetStatus(result);
                }
                break;
                //ROR
                case INS_ROR_ACC:
                {
                    UserBit = (A)&1;
                    A >>= 1;
                    cycles--;
                    A |= (C<<7);
                    C = UserBit;
                    ASLSetStatus(A);
                }
                break;
                case INS_ROR_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte result = memory.Data[ZPAddr];
                    cycles--;
                    UserBit = (result)&1;
                    result >>= 1;
                    cycles--;
                    result |= (C<<7);
                    C = UserBit;
                    memory.Data[ZPAddr] = result;
                    cycles--;
                    ASLSetStatus(result);
                }
                break;
                case INS_ROR_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    cycles--;
                    Byte result = memory.Data[ZPAddr];
                    cycles--;
                    UserBit = (result)&1;
                    result >>= 1;
                    cycles--;
                    result |= (C<<7);
                    C = UserBit;
                    memory.Data[ZPAddr] = result;
                    cycles--;
                    ASLSetStatus(result);
                }
                break;
                case INS_ROR_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte result = memory.Data[AbsAddr];
                    cycles--;
                    UserBit = (result)&1;
                    result >>= 1;
                    cycles--;
                    result |= (C<<7);
                    C = UserBit;
                    memory.Data[AbsAddr] = result;
                    cycles--;
                    ASLSetStatus(result);
                }
                break;
                case INS_ROR_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    AbsAddr+=X;
                    cycles--;
                    Byte result = memory.Data[AbsAddr];
                    cycles--;
                    UserBit = (result)&1;
                    result >>= 1;
                    cycles--;
                    result |= (C<<7);
                    C = UserBit;
                    memory.Data[AbsAddr] = result;
                    cycles--;
                    ASLSetStatus(result);
                }
                break;
                //AND
                case INS_AND_IM:
                {
                    Byte Value = FetchByte(cycles, memory);
                    A &= Value;
                    LDASetStatus();
                }
                break;
                case INS_AND_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    A &= ReadByte(cycles, memory, ZPAddr);
                    LDASetStatus();
                }
                break;
                case INS_AND_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    cycles--;
                    A &= ReadByte(cycles, memory, ZPAddr);
                    LDASetStatus();
                }
                break;
                case INS_AND_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    A &= ReadByte_ABS(cycles, memory, AbsAddr);
                    LDASetStatus();
                }
                break;
                case INS_AND_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, X)) cycles--;
                    AbsAddr+=X;
                    A &= ReadByte_ABS(cycles, memory, AbsAddr);
                    LDASetStatus();
                }
                break;
                case INS_AND_ABSY:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, Y)) cycles--;
                    AbsAddr+=Y;
                    A &= ReadByte_ABS(cycles, memory, AbsAddr);
                    LDASetStatus();
                }
                break;
                case INS_AND_INDX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    cycles--;
                    Word ValAddr = ReadWord(cycles, memory, ZPAddr);
                    Byte Value = ReadByte_ABS(cycles, memory, ValAddr);
                    A &= Value;
                    LDASetStatus();
                }
                break;
                case INS_AND_INDY:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Word ValAddr = ReadWord(cycles, memory, ZPAddr);
                    if(pageChange(ValAddr, Y)) cycles--;
                    ValAddr+=Y;
                    Byte Value = ReadByte_ABS(cycles, memory, ValAddr);
                    A&=Value;
                    LDASetStatus();
                }
                break;
                //ADC
                case INS_ADC_IM:
                {
                    Byte operand = FetchByte(cycles, memory);
                    Byte result = A+operand+C;
                    if((A+operand+C)>0xFF){
                        C=1;
                        A = result&0xFF;
                    }
                    else A = result;
                    LDASetStatus();
                }
                break;
                case INS_ADC_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte operand = ReadByte(cycles, memory, ZPAddr);
                    Byte result = A+operand+C;
                    if((A+operand+C)>0xFF){
                        C=1;
                        A = result&0xFF;
                    }
                    else A = result;
                    LDASetStatus();
                }
                break;
                case INS_ADC_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    Byte operand = ReadByte(cycles, memory, ZPAddr);
                    Byte result = A+operand+C;
                    if((A+operand+C)>0xFF){
                        C=1;
                        A = result&0xFF;
                    }
                    else A = result;
                    LDASetStatus();
                }
                break;
                case INS_ADC_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte operand = ReadByte_ABS(cycles, memory, AbsAddr);
                    Byte result = A+operand+C;
                    if((A+operand+C)>0xFF){
                        C=1;
                        A = result%0xFF;
                    }
                    else A = result;
                    LDASetStatus();
                }
                break;
                case INS_ADC_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, X)) cycles--;
                    AbsAddr+=X;
                    Byte operand = ReadByte_ABS(cycles, memory, AbsAddr);
                    Byte result = A+operand+C;
                    if((A+operand+C)>0xFF){
                        C=1;
                        A = result%0xFF;
                    }
                    else A = result;
                    LDASetStatus();
                }
                break;
                case INS_ADC_ABSY:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, Y)) cycles--;
                    AbsAddr+=Y;
                    Byte operand = ReadByte_ABS(cycles, memory, AbsAddr);
                    Byte result = A+operand+C;
                    if((A+operand+C)>0xFF){
                        C=1;
                        A = result%0xFF;
                    }
                    else A = result;
                    LDASetStatus();
                }
                break;
                case INS_ADC_INDX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    cycles--;
                    Word LoadAddr = ReadWord(cycles, memory, ZPAddr);
                    Byte operand = ReadByte_ABS(cycles, memory, LoadAddr);
                    Byte result = A+operand+C;
                    if((A+operand+C)>0xFF){
                        C=1;
                        A = result%0xFF;
                    }
                    else A = result;
                    LDASetStatus();
                }
                break;
                case INS_ADC_INDY:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Word LoadAddr = ReadWord(cycles, memory, ZPAddr);
                    if(pageChange(LoadAddr, Y)) cycles--;
                    LoadAddr+=Y;
                    Byte operand = ReadByte_ABS(cycles, memory, LoadAddr);
                    Byte result = A+operand+C;
                    if((A+operand+C)>0xFF){
                        C=1;
                        A = result%0xFF;
                    }
                    else A = result;
                    LDASetStatus();
                }
                break;
                //SBC
                case INS_SBC_IM:
                {
                    Byte Val = FetchByte(cycles, memory);
                    Word Res = A - Val - (1 - C);
                    SBCSetStatus(Res ,Val); 
                    A = Res&0xFF;
                }
                break;
                case INS_SBC_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    Word Res = A - Val - (1 - C);
                    SBCSetStatus(Res ,Val); 
                    A = Res&0xFF;
                }
                break;
                case INS_SBC_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr+=X;
                    cycles--;
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    Word Res = A - Val - (1 - C);
                    SBCSetStatus(Res ,Val); 
                    A = Res&0xFF;
                }
                break;
                case INS_SBC_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    Word Res = A - Val - (1 - C);
                    SBCSetStatus(Res ,Val); 
                    A = Res&0xFF;
                }
                break;
                case INS_SBC_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, X)) cycles--;
                    AbsAddr+=X;
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    Word Res = A - Val - (1 - C);
                    SBCSetStatus(Res ,Val); 
                    A = Res&0xFF;
                }
                break;
                case INS_SBC_ABSY:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, Y)) cycles--;
                    AbsAddr+=Y;
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    Word Res = A - Val - (1 - C);
                    SBCSetStatus(Res ,Val); 
                    A = Res&0xFF;
                }
                break;
                case INS_SBC_INDX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr+=X;
                    cycles--;
                    Word LoadAddr = FetchWord(cycles, memory);
                    Byte Val = ReadByte_ABS(cycles, memory, LoadAddr);
                    Word Res = A - Val - (1 - C);
                    SBCSetStatus(Res ,Val); 
                    A = Res&0xFF;
                }
                break;
                case INS_SBC_INDY:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Word LoadAddr = FetchWord(cycles, memory);
                    if(pageChange(LoadAddr, Y)) cycles--;
                    LoadAddr+=Y;
                    Byte Val = ReadByte_ABS(cycles, memory, LoadAddr);
                    Word Res = A - Val - (1 - C);
                    SBCSetStatus(Res ,Val);
                    A = Res&0xFF;
                }
                break;
                //BCC
                case INS_BCC:
                {
                    Byte offset = FetchByte(cycles, memory);
                    signed char signed_offset = (signed char)offset;
                    if(C==0){
                        Word target = PC+2+signed_offset;
                        cycles--;
                        if ((PC + 2) & 0xFF00 != target & 0xFF00) cycles++;
                        PC = target;
                    }
                    else PC+=2;
                }
                break;
                //BCS
                case INS_BCS:
                {
                    Byte offset = FetchByte(cycles, memory);
                    signed char signed_offset = (signed char)offset;
                    if(C==1){
                        Word target = PC+2+signed_offset;
                        cycles--;
                        if ((PC + 2) & 0xFF00 != (PC + 2 + signed_offset) & 0xFF00) cycles++; //cant use PageChange() as signed value is present
                        PC = target;
                    }
                    else PC+=2;
                }
                break;
                //BEQ
                case INS_BEQ:
                {
                    Byte offset = FetchByte(cycles, memory);
                    signed char signed_offset = (signed char)offset;
                    if(Z==1){
                        Word target = PC+2+signed_offset;
                        cycles--;
                        if ((PC + 2) & 0xFF00 != (PC + 2 + signed_offset) & 0xFF00) cycles++; //cant use PageChange() as signed value is present
                        PC = target;
                    }
                    else PC+=2;
                }
                break;
                //BMI
                case INS_BMI:
                {
                    Byte offset = FetchByte(cycles, memory);
                    signed char signed_offset = (signed char)offset;
                    if(N==1){
                        Word target = PC+2+signed_offset;
                        cycles--;
                        if ((PC + 2) & 0xFF00 != (PC + 2 + signed_offset) & 0xFF00) cycles++; //cant use PageChange() as signed value is present
                        PC = target;
                    }
                    else PC+=2;
                }
                break;
                //BNE
                case INS_BNE:
                {
                    Byte offset = FetchByte(cycles, memory);
                    signed char signed_offset = (signed char)offset;
                    if(Z==0){
                        Word target = PC+2+signed_offset;
                        cycles--;
                        if ((PC + 2) & 0xFF00 != (PC + 2 + signed_offset) & 0xFF00) cycles++; //cant use PageChange() as signed value is present
                        PC = target;
                    }
                    else PC+=2;
                }
                break;
                //BPL
                case INS_BPL:
                {
                    Byte offset = FetchByte(cycles, memory);
                    signed char signed_offset = (signed char)offset;
                    if(N==0){
                        Word target = PC+2+signed_offset;
                        cycles--;
                        if ((PC + 2) & 0xFF00 != (PC + 2 + signed_offset) & 0xFF00) cycles++; //cant use PageChange() as signed value is present
                        PC = target;
                    }
                    else PC+=2;
                }
                break;
                //BVC
                case INS_BVC:
                {
                    Byte offset = FetchByte(cycles, memory);
                    signed char signed_offset = (signed char)offset;
                    if(V==0){
                        Word target = PC+2+signed_offset;
                        cycles--;
                        if ((PC + 2) & 0xFF00 != (PC + 2 + signed_offset) & 0xFF00) cycles++; //cant use PageChange() as signed value is present
                        PC = target;
                    }
                    else PC+=2;
                }
                break;
                //BVS
                case INS_BVS:
                {
                    Byte offset = FetchByte(cycles, memory);
                    signed char signed_offset = (signed char)offset;
                    if(V==1){
                        Word target = PC+2+signed_offset;
                        cycles--;
                        if ((PC + 2) & 0xFF00 != (PC + 2 + signed_offset) & 0xFF00) cycles++; //cant use PageChange() as signed value is present
                        PC = target;
                    }
                    else PC+=2;
                }
                break;
                //BIT
                case INS_BIT_ZP:
                {
                    Byte ZPAddress = FetchByte(cycles, memory);
                    Byte operand = ReadByte(cycles, memory, ZPAddress);
                    BITSetStatus(operand);
                }
                break;
                case INS_BIT_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte operand = ReadByte_ABS(cycles, memory, AbsAddr);
                    BITSetStatus(operand);
                }
                break;
                //CMP
                case INS_CMP_IM:
                {
                    Byte Val = FetchByte(cycles, memory);
                    CMPSetStatus(Val, A);
                }
                break;
                case INS_CMP_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    CMPSetStatus(Val, A);
                }
                break;
                case INS_CMP_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    cycles--;
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    CMPSetStatus(Val, A);
                }
                break;
                case INS_CMP_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    CMPSetStatus(Val, A);
                }
                break;
                case INS_CMP_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, X)) cycles--;
                    AbsAddr+=X;
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    CMPSetStatus(Val, A);
                }
                break;
                case INS_CMP_ABSY:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, Y)) cycles--;
                    AbsAddr+=Y;
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    CMPSetStatus(Val, A);
                }
                break;
                case INS_CMP_INDX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr = (ZPAddr + X) & 0xFF;
                    cycles--;
                    Word ValAddr = ReadWord(cycles, memory, ZPAddr);
                    Byte Val = ReadByte_ABS(cycles, memory, ValAddr);
                    CMPSetStatus(Val, A);
                }
                break;
                case INS_CMP_INDY:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Word ValAddr = ReadWord(cycles, memory, ZPAddr);
                    if(pageChange(ValAddr, Y)) cycles--;
                    ValAddr+=Y;
                    Byte Val = ReadByte_ABS(cycles, memory, ValAddr);
                    CMPSetStatus(Val, A);
                }
                break;
                //CPX
                case INS_CPX_IM:
                {
                    Byte Val = FetchByte(cycles, memory);
                    CMPSetStatus(Val, X);
                }
                break;
                case INS_CPX_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    CMPSetStatus(Val, X);
                }
                break;
                case INS_CPX_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    CMPSetStatus(Val, X);
                }
                break;
                //CPY
                case INS_CPY_IM:
                {
                    Byte Val = FetchByte(cycles, memory);
                    CMPSetStatus(Val, Y);
                }
                break;
                case INS_CPY_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    CMPSetStatus(Val, Y);
                }
                break;
                case INS_CPY_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    CMPSetStatus(Val, Y);
                }
                break;
                //DEC
                case INS_DEC_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    DECSetStatus(Val);
                    Val-=1;
                    cycles--;
                    memory.WriteByte(Val, ZPAddr, cycles);
                }
                break;
                case INS_DEC_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr+=X;
                    cycles--;
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    DECSetStatus(Val);
                    Val-=1;
                    cycles--;
                    memory.WriteByte(Val, ZPAddr, cycles);
                }
                break;
                case INS_DEC_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    DECSetStatus(Val);
                    Val -= 1;
                    cycles--;
                    memory.WriteByte(Val, AbsAddr, cycles);
                }
                break;
                case INS_DEC_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    AbsAddr+=X;
                    cycles--;
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    DECSetStatus(Val);
                    Val-=1;
                    cycles--;
                    memory.WriteByte(Val, AbsAddr, cycles);
                }
                break;
                //DECX
                case INS_DEX:
                {
                    Byte Res = X-1;
                    N = (Res & 0x80)!=0;
                    Z = (Res==0);
                    SR &= ~(0x02 | 0x80);
                    if (Z) SR |= 0x02;
                    if (N) SR |= 0x80;
                    X = Res;
                    cycles--;
                } 
                break;
                //DEY
                case INS_DEY:
                {
                    Byte Res = Y-1;
                    N = (Res & 0x80)!=0;
                    Z = (Res==0);
                    SR &= ~(0x02 | 0x80);
                    if (Z) SR |= 0x02;
                    if (N) SR |= 0x80;
                    Y = Res;
                    cycles--;
                } 
                break;
                //EOR 
                case INS_EOR_IM:
                {
                    Byte Val = FetchByte(cycles, memory);
                    EORSetStatus(Val, A);
                    A = A^Val;
                }
                break;
                case INS_EOR_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    EORSetStatus(Val, A);
                    A = A^Val;
                }
                break;
                case INS_EOR_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr+=X;
                    cycles--;
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    EORSetStatus(Val, A);
                    A = A^Val;
                }
                break;
                case INS_EOR_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    EORSetStatus(Val, A);
                    A = A^Val;
                }
                break;
                case INS_EOR_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, X)) cycles--;
                    AbsAddr+=X;
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    EORSetStatus(Val, A);
                    A = A^Val;
                }
                break;
                case INS_EOR_ABSY:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, Y)) cycles--;
                    AbsAddr+=Y;
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    EORSetStatus(Val, A);
                    A = A^Val;
                }
                break;
                case INS_EOR_INDX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr+=X;
                    cycles--;
                    Word LoadAddr = ReadWord(cycles, memory, ZPAddr);
                    Byte Val = ReadByte_ABS(cycles, memory, LoadAddr);
                    EORSetStatus(Val, A);
                    A = A^Val;
                }
                break;
                case INS_EOR_INDY:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Word LoadAddr = ReadWord(cycles, memory, ZPAddr);
                    if(pageChange(LoadAddr, Y)) cycles--;
                    LoadAddr+=Y;
                    Byte Val = ReadByte_ABS(cycles, memory, LoadAddr);
                    EORSetStatus(Val, A);
                    A = A^Val;
                }
                break;
                //ORA
                case INS_ORA_IM:
                {
                    Byte Val = FetchByte(cycles, memory);
                    ORASetStatus(Val, A);
                    A = A|Val;
                }
                break;
                case INS_ORA_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    ORASetStatus(Val, A);
                    A = A|Val;
                }
                break;
                case INS_ORA_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr+=X;
                    cycles--;
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    ORASetStatus(Val, A);
                    A = A|Val;
                }
                break;
                case INS_ORA_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    EORSetStatus(Val, A);
                    A = A|Val;
                }
                break;
                case INS_ORA_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, X)) cycles--;
                    AbsAddr+=X;
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    ORASetStatus(Val, A);
                    A = A|Val;
                }
                break;
                case INS_ORA_ABSY:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    if(pageChange(AbsAddr, Y)) cycles--;
                    AbsAddr+=Y;
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    EORSetStatus(Val, A);
                    A = A|Val;
                }
                break;
                case INS_ORA_INDX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr+=X;
                    cycles--;
                    Word LoadAddr = ReadWord(cycles, memory, ZPAddr);
                    Byte Val = ReadByte_ABS(cycles, memory, LoadAddr);
                    EORSetStatus(Val, A);
                    A = A|Val;
                }
                break;
                case INS_ORA_INDY:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Word LoadAddr = ReadWord(cycles, memory, ZPAddr);
                    if(pageChange(LoadAddr, Y)) cycles--;
                    LoadAddr+=Y;
                    Byte Val = ReadByte_ABS(cycles, memory, LoadAddr);
                    EORSetStatus(Val, A);
                    A = A|Val;
                }
                break;
                //PHA
                case INS_PHA:
                {
                    memory.Data[0x0100+SP] = A;
                    cycles--;
                    SP--;
                    cycles--;
                }
                break;
                //PHP
                case INS_PHP:
                {
                    memory.Data[0x0100+SP] = SR;
                    cycles--;
                    SP--;
                    cycles--;
                }
                break;
                //PLA
                case INS_PLA:
                {
                    SP++;
                    cycles--;
                    A = memory.Data[0x0100+SP];
                    cycles--;
                    PLASetStatus();
                    cycles--;
                }
                break;
                //PLP
                case INS_PLP:
                {
                    SP++;
                    cycles--;
                    SR = memory.Data[0x0100+SP]|0x20;
                    cycles--;
                    PLPSetStatus();
                    cycles--;
                }
                break;
                //RTI
                case INS_RTI:
                {
                    cycles--; //dummy, consumed internally reading op
                    SP++;
                    cycles--;
                    SR = memory.Data[0x0100+SP]|0x20;
                    PLPSetStatus();
                    cycles--;
                    SP++;
                    cycles--;
                    PC = memory.Data[0x0100+SP]; //low byte
                    SP++;
                    PC = (memory.Data[0x0100+SP]<<8)|PC; //high and low byte
                    cycles--;
                }
                break;
                case INS_RTS:
                {
                    SP++;
                    cycles--;
                    PC = memory.Data[0x0100+SP];//low byte
                    cycles--;
                    SP++;
                    cycles--;
                    PC = (memory.Data[0x0100+SP]<<8)|PC;//high with low byte
                    cycles--;
                    PC = PC+1;
                    cycles--;
                }
                break;
                //SEC
                case INS_SEC:
                {
                    SR |= 0x01;
                    C = 1;
                    cycles--;
                }
                break;
                //SED
                case INS_SED:
                {
                    SR |= 0x08;
                    D = 1;
                    cycles--;
                }
                break;
                //SEI
                case INS_SEI:
                {
                    SR |= 0x04;
                    I = 1;
                    cycles--;
                }
                break;
                //INC
                case INS_INC_ZP:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    INCSetStatus(Val);
                    Val+=1;
                    cycles--;
                    memory.WriteByte(Val, ZPAddr, cycles);
                }
                break;
                case INS_INC_ZPX:
                {
                    Byte ZPAddr = FetchByte(cycles, memory);
                    ZPAddr+=X;
                    cycles--;
                    Byte Val = ReadByte(cycles, memory, ZPAddr);
                    INCSetStatus(Val);
                    Val+=1;
                    cycles--;
                    memory.WriteByte(Val, ZPAddr, cycles);
                }
                break;
                case INS_INC_ABS:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    INCSetStatus(Val);
                    Val+=1;
                    cycles--;
                    memory.WriteByte(Val, AbsAddr, cycles);
                }
                break;
                case INS_INC_ABSX:
                {
                    Word AbsAddr = FetchWord(cycles, memory);
                    AbsAddr+=X;
                    cycles--;
                    Byte Val = ReadByte_ABS(cycles, memory, AbsAddr);
                    INCSetStatus(Val);
                    Val+=1;
                    cycles--;
                    memory.WriteByte(Val, AbsAddr, cycles);
                }
                break;
                //INX
                case INS_INX:
                {
                    Byte Res = X+1;
                    N = (Res & 0x80)!=0;
                    Z = (Res==0);
                    SR &= ~(0x02 | 0x80);
                    if (Z) SR |= 0x02;
                    if (N) SR |= 0x80;
                    X = Res;
                    cycles--;
                } 
                break;
                //INY
                case INS_INY:
                {
                    Byte Res = Y+1;
                    N = (Res & 0x80)!=0;
                    Z = (Res==0);
                    SR &= ~(0x02 | 0x80);
                    if (Z) SR |= 0x02;
                    if (N) SR |= 0x80;
                    Y = Res;
                    cycles--;
                } 
                break;
                //CLC
                case INS_CLC:
                {
                    C = 0;
                    cycles--;
                    PC++;
                }
                break;
                //CLD
                case INS_CLD:
                {
                    D = 0;
                    cycles--;
                    PC++;
                }
                break;
                //CLI
                case INS_CLI:
                {
                    I = 0;
                    cycles--;
                    PC++;
                }
                break;
                //CLV
                case INS_CLV:
                {
                    V = 0;
                    cycles--;
                    PC++;
                }
                break;
                //Jump to subroutine
                case INS_JSR:
                {
                    Word JSRAddress = FetchWord(cycles, memory);
                    memory.WriteWord(PC-1, SP, cycles);
                    PC = JSRAddress;
                    cycles--;
                }
                break;
                //Jump
                case INS_JMP_ABS:
                {
                    Word JMPAddress = FetchWord(cycles, memory);
                    PC = JMPAddress;
                }
                break;
                case INS_JMP_IND:
                {
                    Word SubAddress = FetchWord(cycles, memory);
                    Word JMPAddress = ReadWord(cycles, memory, SubAddress);
                    PC = JMPAddress;
                }
                break;
                //BRK
                case INS_BRK:
                {
                    PC += 2;
                    memory.Data[0x0100+SP] = (PC>>8)&0xFF;
                    SP--;
                    memory.Data[0x0100+SP] = PC&0xFF;
                    SP--;
                    SR |= 0x10; // B is the 5th bit
                    B = 1;
                    memory.Data[0x0100+SP] = SR;
                    SR |= 0x04; // I is the 3rd bit
                    I = 1;
                    SP--;
                    PC = (memory.Data[0xFFFF]<<8)|(memory.Data[0xFFFE]);
                    cycles-=6;
                }
                break;
                //NOP
                case INS_NOP:
                {
                    PC++;
                    cycles--;
                }
                break;
                default: 
                break;
            }
        }
    }
};