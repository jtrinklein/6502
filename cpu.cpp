#include "cpu.h"
#include "mem.h"
#include <iostream>

CPU::CPU(Mem *m) :
    A(0),
    X(0),
    Y(0),
    SP(0xFD),
    PC(0x00),
    Carry(0),
    Zero(0),
    InterruptDisable(1),
    DecimalMode(0),
    BreakCommand(0),
    Overflow(0),
    Negative(0)
{
    
    /*
    Power On State:
    A, X, Y = 0
    SP = $FD
    $4017 = $00 (frame irq enabled)
    $4015 = $00 (all channels disabled)
    $4000-$400F = $00
    $4010-$4013 = $00
    All 15 bits of noise channel LFSR = $0000

    Status Flag values:
    0011 0100
    NVss DIZC
    |||| ||||
    |||| |||+- Carry
    |||| ||+-- Zero
    |||| |+--- Interrupt Disable
    |||| +---- Decimal
    ||++------ No CPU effect, see: the B flag
    |+-------- Overflow
    +--------- Negative
    */ 
     mem = m;
}

CPU::~CPU() {

}

void CPU::Reset() {
    InterruptDisable = 1;
    /*
    APU was silenced ($4015 = 0)
    APU triangle phase is reset to 0 (i.e. outputs a value of 15, the first step of its waveform)
    APU DPCM output ANDed with 1 (upper 6 bits cleared)
    */
}

/* LDA */
static constexpr Byte INS_LDA_IM = 0xA9;
static constexpr Byte INS_LDA_ZP = 0xA5;
static constexpr Byte INS_LDA_ZPX = 0xB5;
static constexpr Byte INS_LDA_ABS = 0xAD;
static constexpr Byte INS_LDA_ABSX = 0xBD;
static constexpr Byte INS_LDA_ABSY = 0xB9;
static constexpr Byte INS_LDA_INDX = 0xA1;
static constexpr Byte INS_LDA_INDY = 0xB1;
/* LDX */
static constexpr Byte INS_LDX_IM = 0xA2;
static constexpr Byte INS_LDX_ZP = 0xA6;
static constexpr Byte INS_LDX_ZPY = 0xB6;
static constexpr Byte INS_LDX_ABS = 0xAE;
static constexpr Byte INS_LDX_ABSY = 0xBE;
/* LDY */
static constexpr Byte INS_LDY_IM = 0xA0;
static constexpr Byte INS_LDY_ZP = 0xA4;
static constexpr Byte INS_LDY_ZPX = 0xB4;
static constexpr Byte INS_LDY_ABS = 0xAC;
static constexpr Byte INS_LDY_ABSX = 0xBC;

#define SET_LOAD_REG_FLAGS(v) do { \
    Zero = v == 0;\
    Negative = (v & 0x80) != 0;\
    } while(false)

u32 CPU::RunOneInstruction() {
    
    auto opcode = mem->ReadByte(PC++);
    switch (opcode) {
        case INS_LDA_IM:
        {
            A = mem->ReadByte(PC++);
            SET_LOAD_REG_FLAGS(A);
            return 2;
        }
        case INS_LDA_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            A = mem->ReadByte(addr);

            SET_LOAD_REG_FLAGS(A);
            return 3;
        }
        case INS_LDA_ZPX:
        {
            Byte offset = mem->ReadByte(PC++);
            Word addr = 0x0000 + (X + offset) & 0xFF;
            A = mem->ReadByte(addr);

            SET_LOAD_REG_FLAGS(A);
            return 4;
        }
        case INS_LDA_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr);

            SET_LOAD_REG_FLAGS(A);
            return 4;
        }
        case INS_LDA_ABSX:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr + X);

            SET_LOAD_REG_FLAGS(A);
            if ((((addr & 0xFF) + X ) & 0x100) == 0x100) {
                return 5;
            }
            return 4;
        }
        case INS_LDA_ABSY:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr + Y);

            SET_LOAD_REG_FLAGS(A);
            if ((((addr & 0xFF) + Y ) & 0x100) == 0x100) {
                return 5;
            }
            return 4;
        }
        case INS_LDA_INDX:
        {
            Byte offset = mem->ReadByte(PC++);
            Word addr = 0x0000 + (X + offset) & 0xFF;
            addr = mem->ReadWord(addr);
            A = mem->ReadByte(addr);

            SET_LOAD_REG_FLAGS(A);
            return 6;
        }
        case INS_LDA_INDY:
        {
            // get zeropage addr
            Byte offset = mem->ReadByte(PC++);
            // get the abs addr at zp
            Byte lsb = mem->ReadByte(0x0000 + offset);
            Byte msb = mem->ReadByte((0x0000 + offset + 1) & 0xFF);
            Word addr = (Word(msb) << 8) + lsb;

            // add y to abs addr
            // set a to value located at final addr
            A = mem->ReadByte(addr + Y);

            SET_LOAD_REG_FLAGS(A);
            if ((((addr & 0xFF) + Y ) & 0x100) == 0x100) {
                return 6;
            }
            return 5;
        }
        case INS_LDX_IM:
        {
            X = mem->ReadByte(PC++);
            SET_LOAD_REG_FLAGS(X);
            return 2;
        }
        case INS_LDX_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            X = mem->ReadByte(addr);

            SET_LOAD_REG_FLAGS(X);
            return 3;
        }
        case INS_LDX_ZPY:
        {
            Byte offset = mem->ReadByte(PC++);
            Word addr = 0x0000 + (Y + offset) & 0xFF;
            X = mem->ReadByte(addr);

            SET_LOAD_REG_FLAGS(X);
            return 4;
        }
        case INS_LDX_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            X = mem->ReadByte(addr);

            SET_LOAD_REG_FLAGS(X);
            return 4;
        }
        case INS_LDX_ABSY:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            X = mem->ReadByte(addr + Y);

            SET_LOAD_REG_FLAGS(X);
            if ((((addr & 0xFF) + Y ) & 0x100) == 0x100) {
                return 5;
            }
            return 4;
        }
        case INS_LDY_IM:
        {
            Y = mem->ReadByte(PC++);
            SET_LOAD_REG_FLAGS(Y);
            return 2;
        }
        case INS_LDY_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            Y = mem->ReadByte(addr);

            SET_LOAD_REG_FLAGS(Y);
            return 3;
        }
        case INS_LDY_ZPX:
        {
            Byte offset = mem->ReadByte(PC++);
            Word addr = 0x0000 + (X + offset) & 0xFF;
            Y = mem->ReadByte(addr);

            SET_LOAD_REG_FLAGS(Y);
            return 4;
        }
        case INS_LDY_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            Y = mem->ReadByte(addr);

            SET_LOAD_REG_FLAGS(Y);
            return 4;
        }
        case INS_LDY_ABSX:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            Y = mem->ReadByte(addr + X);

            SET_LOAD_REG_FLAGS(Y);
            if ((((addr & 0xFF) + X ) & 0x100) == 0x100) {
                return 5;
            }
            return 4;
        }
        default:
        {
            std::cout << "unknown opcode: 0x" << std::hex << (u32)opcode << std::endl;
        }
    }
    return 0;
}