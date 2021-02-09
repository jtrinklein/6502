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

/* LSR
Each of the bits in A or M is shift one place to the right.
The bit that was in bit 0 is shifted into the carry flag.
Bit 7 is set to zero. 

For mem operations: data is loaded into accumulator, shifted, then written back to memory
*/
static constexpr Byte INS_LSR_A    = 0x4A;
static constexpr Byte INS_LSR_ZP   = 0x46;
static constexpr Byte INS_LSR_ZPX  = 0x56;
static constexpr Byte INS_LSR_ABS  = 0x4E;
static constexpr Byte INS_LSR_ABSX = 0x5E;

/* ROR
Each of the bits in A or M is shift one place to the right.
Bit 7 is filled with the current value of the carry flag 
whilst the old bit 0 becomes the new carry flag value.

For mem operations: data is loaded into accumulator, shifted, then written back to memory
*/
static constexpr Byte INS_ROR_A    = 0x6A;
static constexpr Byte INS_ROR_ZP   = 0x66;
static constexpr Byte INS_ROR_ZPX  = 0x76;
static constexpr Byte INS_ROR_ABS  = 0x6E;
static constexpr Byte INS_ROR_ABSX = 0x7E;

/* ROL
Move each of the bits in either A or M one place to the left.
Bit 0 is filled with the current value of the carry flag whilst
the old bit 7 becomes the new carry flag value.

For mem operations: data is loaded into accumulator, shifted, then written back to memory
*/
static constexpr Byte INS_ROL_A    = 0x2A;
static constexpr Byte INS_ROL_ZP   = 0x26;
static constexpr Byte INS_ROL_ZPX  = 0x36;
static constexpr Byte INS_ROL_ABS  = 0x2E;
static constexpr Byte INS_ROL_ABSX = 0x3E;

/* ASL
This operation shifts all the bits of the accumulator or
memory contents one bit left.
Bit 0 is set to 0 and bit 7 is placed in the carry flag.
The effect of this operation is to multiply the memory
contents by 2 (ignoring 2's complement considerations),
setting the carry if the result will not fit in 8 bits.

For mem operations: data is loaded into accumulator, shifted, then written back to memory
*/
static constexpr Byte INS_ASL_A    = 0x0A;
static constexpr Byte INS_ASL_ZP   = 0x06;
static constexpr Byte INS_ASL_ZPX  = 0x16;
static constexpr Byte INS_ASL_ABS  = 0x0E;
static constexpr Byte INS_ASL_ABSX = 0x1E;

/* STA */
static constexpr Byte INS_STA_ZP   = 0x85;
static constexpr Byte INS_STA_ZPX  = 0x95;
static constexpr Byte INS_STA_ABS  = 0x8D;
static constexpr Byte INS_STA_ABSX = 0x9D;
static constexpr Byte INS_STA_ABSY = 0x99;
static constexpr Byte INS_STA_INDX = 0x81;
static constexpr Byte INS_STA_INDY = 0x91;

/* STX */
static constexpr Byte INS_STX_ZP   = 0x86;
static constexpr Byte INS_STX_ZPY  = 0x96;
static constexpr Byte INS_STX_ABS  = 0x8E;

/* STY */
static constexpr Byte INS_STY_ZP   = 0x84;
static constexpr Byte INS_STY_ZPX  = 0x94;
static constexpr Byte INS_STY_ABS  = 0x8C;

/* Transfer between regs */
static constexpr Byte INS_TAX  = 0xAA;
static constexpr Byte INS_TXA  = 0x8A;
static constexpr Byte INS_TAY  = 0xA8;
static constexpr Byte INS_TYA  = 0x98;
static constexpr Byte INS_TSX  = 0xBA;
static constexpr Byte INS_TXS  = 0x9A;

/* Set/Clear flags */
static constexpr Byte INS_SEC  = 0x38;
static constexpr Byte INS_SED  = 0xF8;
static constexpr Byte INS_SEI  = 0x78;
static constexpr Byte INS_CLC  = 0x18;
static constexpr Byte INS_CLD  = 0xD8;
static constexpr Byte INS_CLI  = 0x58;

/* BIT test 
This instructions is used to test if 
one or more bits are set in a target memory location.
The mask pattern in A is ANDed with the value in memory
to set or clear the zero flag, but the result is not kept.
Bits 7 and 6 of the value from memory are copied into the N and V flags.
*/
static constexpr Byte INS_BIT_ZP  = 0x24;
static constexpr Byte INS_BIT_ABS = 0x2C;

/* Branch Instructions */
static constexpr Byte INS_BMI  = 0x30; // branch if minus (Negative is set)
static constexpr Byte INS_BPL  = 0x10; // branch if positive (Negative is clear)
static constexpr Byte INS_BNE  = 0xD0; // branch if not equal (Zero is clear)
static constexpr Byte INS_BEQ  = 0xF0; // branch if positive (Zero is set)



#define SET_BIT_FLAGS(v) do {           \
        Zero = (A & v) == 0;            \
        Overflow = ((1 << 6) & v) != 0; \
        Negative = ((1 << 7) & v) != 0; \
    } while(false)

#define SET_LOAD_REG_FLAGS(v) do { \
    Zero = v == 0;\
    Negative = (v & 0x80) != 0;\
    } while(false)

#define SET_LSR_FLAGS(v) SET_LOAD_REG_FLAGS(v)
#define SET_ROR_FLAGS(v) SET_LOAD_REG_FLAGS(v)
#define SET_ROL_FLAGS(v) SET_LOAD_REG_FLAGS(v)
#define SET_ASL_FLAGS(v) SET_LOAD_REG_FLAGS(v)

#define DO_LSR(x) do {          \
        Byte newC = x & 0x01;   \
        x = (x >> 1);           \
        Carry = newC;           \
    }while(false)

#define DO_ROR(x) do {          \
        Byte newC = x & 0x01;   \
        x = (x >> 1);           \
        if (Carry) {            \
            x += 0x80;          \
        }                       \
        Carry = newC;           \
    }while(false)

#define DO_ROL(x) do {                      \
        Byte newC = (x & 0x80) == 0x80;     \
        x = (x << 1);                       \
        if (Carry) {                        \
            x += 0x01;                      \
        }                                   \
        Carry = newC;                       \
    }while(false)

#define DO_ASL(x) do {                      \
        Byte newC = (x & 0x80) == 0x80;     \
        x = (x << 1);                       \
        Carry = newC;                       \
    }while(false)

#define DO_RELATIVE_JUMP(r) do {    \
        if (r & 0x80) {             \
            PC -= (0x100 - r);      \
        } else {                    \
            PC += r;                \
        }                           \
    }while(false)

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
        case INS_LSR_A:
        {
            DO_LSR(A);
            
            SET_LSR_FLAGS(A);
            return 2;
        }
        case INS_LSR_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            A = mem->ReadByte(addr);
            DO_LSR(A);
            mem->WriteByte(addr, A);
            SET_LSR_FLAGS(A);
            return 5;
        }
        case INS_LSR_ZPX:
        {
            Byte offset = mem->ReadByte(PC++);
            Word addr = 0x0000 + (X + offset) & 0xFF;
            A = mem->ReadByte(addr);

            DO_LSR(A);
            mem->WriteByte(addr, A);
            SET_LSR_FLAGS(A);
            return 6;
        }
        case INS_LSR_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr);

            DO_LSR(A);
            mem->WriteByte(addr, A);
            SET_LSR_FLAGS(A);
            return 6;
        }
        case INS_LSR_ABSX:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr + X);

            DO_LSR(A);
            mem->WriteByte(addr + X, A);
            SET_LSR_FLAGS(A);
            return 7;
        }
        case INS_ROR_A:
        {
            DO_ROR(A);
            
            SET_ROR_FLAGS(A);
            return 2;
        }
        case INS_ROR_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr);

            DO_ROR(A);
            mem->WriteByte(addr, A);
            SET_ROR_FLAGS(A);
            return 6;
        }
        case INS_ROR_ABSX:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr + X);

            DO_ROR(A);
            mem->WriteByte(addr + X, A);
            SET_ROR_FLAGS(A);
            return 7;
        }
        case INS_ROR_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            A = mem->ReadByte(addr);
            DO_ROR(A);
            mem->WriteByte(addr, A);
            SET_ROR_FLAGS(A);
            return 5;
        }
        case INS_ROR_ZPX:
        {
            Byte offset = mem->ReadByte(PC++);
            Word addr = 0x0000 + (X + offset) & 0xFF;
            A = mem->ReadByte(addr);

            DO_ROR(A);
            mem->WriteByte(addr, A);
            SET_ROR_FLAGS(A);
            return 6;
        }
        case INS_ROL_A:
        {
            DO_ROL(A);
            
            SET_ROL_FLAGS(A);
            return 2;
        }
        case INS_ROL_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr);

            DO_ROL(A);
            mem->WriteByte(addr, A);
            SET_ROL_FLAGS(A);
            return 6;
        }
        case INS_ROL_ABSX:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr + X);

            DO_ROL(A);
            mem->WriteByte(addr + X, A);
            SET_ROL_FLAGS(A);
            return 7;
        }
        case INS_ROL_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            A = mem->ReadByte(addr);
            DO_ROL(A);
            mem->WriteByte(addr, A);
            SET_ROL_FLAGS(A);
            return 5;
        }
        case INS_ROL_ZPX:
        {
            Byte offset = mem->ReadByte(PC++);
            Word addr = 0x0000 + (X + offset) & 0xFF;
            A = mem->ReadByte(addr);

            DO_ROL(A);
            mem->WriteByte(addr, A);
            SET_ROL_FLAGS(A);
            return 6;
        }
        case INS_ASL_A:
        {
            DO_ASL(A);
            
            SET_ASL_FLAGS(A);
            return 2;
        }
        case INS_ASL_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr);

            DO_ASL(A);
            mem->WriteByte(addr, A);
            SET_ASL_FLAGS(A);
            return 6;
        }
        case INS_ASL_ABSX:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            A = mem->ReadByte(addr + X);

            DO_ASL(A);
            mem->WriteByte(addr + X, A);
            SET_ASL_FLAGS(A);
            return 7;
        }
        case INS_ASL_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            A = mem->ReadByte(addr);
            DO_ASL(A);
            mem->WriteByte(addr, A);
            SET_ASL_FLAGS(A);
            return 5;
        }
        case INS_ASL_ZPX:
        {
            Byte offset = mem->ReadByte(PC++);
            Word addr = 0x0000 + (X + offset) & 0xFF;
            A = mem->ReadByte(addr);

            DO_ASL(A);
            mem->WriteByte(addr, A);
            SET_ASL_FLAGS(A);
            return 6;
        }
        case INS_STA_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            mem->WriteByte(addr, A);
            return 3;
        }
        case INS_STA_ZPX:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++) + X;
            mem->WriteByte(addr, A);
            return 4;
        }
        case INS_STA_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            mem->WriteByte(addr, A);
            return 4;
        }
        case INS_STA_ABSX:
        {
            Word addr = mem->ReadWord(PC) + X;
            PC += 2;
            mem->WriteByte(addr, A);
            return 5;
        }
        case INS_STA_ABSY:
        {
            Word addr = mem->ReadWord(PC) + Y;
            PC += 2;
            mem->WriteByte(addr, A);
            return 5;
        }
        case INS_STA_INDX:
        {
            Byte offset = mem->ReadByte(PC++);
            Word addr = 0x0000 + (X + offset) & 0xFF;
            addr = mem->ReadWord(addr);
            mem->WriteByte(addr, A);

            return 6;
        }
        case INS_STA_INDY:
        {
            // get zeropage addr
            Byte offset = mem->ReadByte(PC++);
            // get the abs addr at zp
            Byte lsb = mem->ReadByte(0x0000 + offset);
            Byte msb = mem->ReadByte((0x0000 + offset + 1) & 0xFF);
            Word addr = (Word(msb) << 8) + lsb;

            // add y to abs addr
            // set a to value located at final addr
            mem->WriteByte(addr + Y, A);

            return 6;
        }
        case INS_STX_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            mem->WriteByte(addr, X);
            return 3;
        }
        case INS_STX_ZPY:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++) + Y;
            mem->WriteByte(addr, X);
            return 4;
        }
        case INS_STX_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            mem->WriteByte(addr, X);
            return 4;
        }
        case INS_STY_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            mem->WriteByte(addr, Y);
            return 3;
        }
        case INS_STY_ZPX:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++) + X;
            mem->WriteByte(addr, Y);
            return 4;
        }
        case INS_STY_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            mem->WriteByte(addr, Y);
            return 4;
        }
        case INS_TAX:
        {
            X = A;
            SET_LOAD_REG_FLAGS(X);
            return 2;
        }
        case INS_TXA:
        {
            A = X;
            SET_LOAD_REG_FLAGS(A);
            return 2;
        }
        case INS_TAY:
        {
            Y = A;
            SET_LOAD_REG_FLAGS(Y);
            return 2;
        }
        case INS_TYA:
        {
            A = Y;
            SET_LOAD_REG_FLAGS(A);
            return 2;
        }
        case INS_TSX:
        {
            X = SP;
            SET_LOAD_REG_FLAGS(X);
            return 2;
        }
        case INS_TXS:
        {
            SP = X;
            SET_LOAD_REG_FLAGS(SP);
            return 2;
        }
        case INS_SEC:
        {
            Carry = 1;
            return 2;
        }
        case INS_SED:
        {
            DecimalMode = 1;
            return 2;
        }
        case INS_SEI:
        {
            InterruptDisable = 1;
            return 2;
        }
        case INS_CLC:
        {
            Carry = 0;
            return 2;
        }
        case INS_CLD:
        {
            DecimalMode = 0;
            return 2;
        }
        case INS_CLI:
        {
            InterruptDisable = 0;
            return 2;
        }
        case INS_BIT_ZP:
        {
            Word addr = 0x0000 + mem->ReadByte(PC++);
            Byte v = mem->ReadByte(addr);

            SET_BIT_FLAGS(v);

            return 3;
        }
        case INS_BIT_ABS:
        {
            Word addr = mem->ReadWord(PC);
            PC += 2;
            Byte v = mem->ReadByte(addr);

            SET_BIT_FLAGS(v);

            return 4;
        }
        case INS_BMI:
        {
            Byte relative_jump = mem->ReadByte(PC++);
            Word old_pc(PC);

            if (Negative) {
                DO_RELATIVE_JUMP(relative_jump);

                auto page_crossed = (PC & 0x100) != (old_pc & 0x100);
                if (page_crossed) {
                    return 4;
                }
                return 3;
            }
            return 2;
        }
        case INS_BNE:
        {
            Byte relative_jump = mem->ReadByte(PC++);
            Word old_pc(PC);

            if (!Zero) {
                DO_RELATIVE_JUMP(relative_jump);

                auto page_crossed = (PC & 0x100) != (old_pc & 0x100);
                if (page_crossed) {
                    return 4;
                }
                return 3;
            }
            return 2;
        }
        case INS_BPL:
        {
            Byte relative_jump = mem->ReadByte(PC++);
            Word old_pc(PC);

            if (!Negative) {
                DO_RELATIVE_JUMP(relative_jump);

                auto page_crossed = (PC & 0x100) != (old_pc & 0x100);
                if (page_crossed) {
                    return 4;
                }
                return 3;
            }
            return 2;
        }
        case INS_BEQ:
        {
            Byte relative_jump = mem->ReadByte(PC++);
            Word old_pc(PC);

            if (Zero) {
                DO_RELATIVE_JUMP(relative_jump);

                auto page_crossed = (PC & 0x100) != (old_pc & 0x100);
                if (page_crossed) {
                    return 4;
                }
                return 3;
            }
            return 2;
        }
        default:
        {
            std::cout << "unknown opcode: 0x" << std::hex << (u32)opcode << std::endl;
        }
    }
    return 0;
}