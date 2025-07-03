 [*6502 Emulation in C*]

A minimal, highly portable emulator for the MOS Technology 6502 CPU written in C.

## 🔧 Features
- ✅ Functional emulation of all legal NMOS 6502 instructions  
- 🔄 Instruction‑level cycle accuracy  
- 🧩 Clean, well‑documented code — easy to integrate and understand  
- ⚙️ Built‑in power/reset/IRQ/NMI control  
- 🧪 Tested with standardized 6502 functional test suites

## 🧠 Why This Project?
Inspired by clean-room C implementations like [bzotto's MCS6502](https://github.com/bzotto/MCS6502) and YouTube videos like [Dave Poo's](https://youtu.be/qJgsuQoy9bc?si=gLgrE8h7iMOjA0L_)
Designed for educational use, retro hardware emulation, and as a foundation for NES‑style systems.

## 📦 Getting Started

### 1. Download / Clone
```bash
git clone https://github.com/AdityaMohanty374/6502-Emulation-in-C.git
cd 6502-Emulation-in-C
```
##### 🧪 Example Usage

For now every code is to be hard coded into the memory like the following. Below shows a simple example AND instruction with indirect addressing mode being emulated. 
Code initializes a cpu and a memory, followed using multiple built-in functions in the header file "main6502.h". Then assigning various values at multiple addresses 
necessary for successful execution of AND instruction.

```cpp
#include "main6502.h"

int main(){
    CPU cpu;
    Mem memory;
    memory.init();
    memory.Data[0xFFFC] = 0x00;     
    memory.Data[0xFFFD] = 0x80; //PC at 0x8000
    cpu.reset(memory);
    cpu.A = 0xBD;
    cpu.X = 0x10;
    memory.Data[0x8000] = CPU::INS_AND_INDX;
    memory.Data[0x8001] = 0x56;
    memory.Data[0x0066] = 0x3F;
    memory.Data[0x0067] = 0xA6;
    memory.Data[0xA63F] = 0x96;
    cpu.execute(6, memory);
    printf("0xBD & 0x96 = 0x%04X\n", cpu.A);
    printf("Zero Flag = 0x%04X\nNegative Flag = 0x%04X\n", cpu.Z, cpu.N);
    return 0;
}
```
[RESULT]
0xBD & 0x96 = 0x0094
Zero Flag = 0x0000
Negative Flag = 0x0001

📜 License
MIT — free for educational and commercial use with attribution.
