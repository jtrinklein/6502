#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class LDA_Indirect_Y_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0xB1;
    static constexpr Byte op_size = 2;
    static constexpr Byte op_cycles = 5;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithZero( void ) {
        const Byte testing_val = 0x00;
        const Byte zp_addr = 0x84;
        const Byte offset = 0x03;
        const Word addr = 0x21CF;
        const Byte d[] = {
            opcode, zp_addr, 0xFF
        };
        mem->LoadFromData(d, sizeof(d));

        // write origin addr to zp address
        mem->WriteWord(0x0000 + zp_addr, addr);

        // write target value to addr + offset, as well as guard values
        const Byte bad_val = 0xFF;
        mem->WriteByte(addr + offset, testing_val);
        mem->WriteByte(addr, bad_val);
        mem->WriteByte(addr + offset - 1, bad_val);
        mem->WriteByte(addr + offset + 1, bad_val);

        cpu->PC = 0x0000;
        cpu->Y = offset; // offset lives in Y
        cpu->A = 0x11;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, testing_val);
        TS_ASSERT_EQUALS(cpu->Y, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithZeroAndPageCrossed( void ) {
        const Byte testing_val = 0x00;
        const Byte zp_addr = 0x1F;
        const Byte offset = 0x44;
        const Word addr = 0x66DF;
        const Byte d[] = {
            opcode, zp_addr, 0xFF
        };
        mem->LoadFromData(d, sizeof(d));

        // write origin addr to zp address
        mem->WriteWord(0x0000 + zp_addr, addr);

        // write target value to addr + offset, as well as guard values
        const Byte bad_val = 0xFF;
        mem->WriteByte(addr + offset, testing_val);
        mem->WriteByte(addr, bad_val);
        mem->WriteByte(addr + offset - 1, bad_val);
        mem->WriteByte(addr + offset + 1, bad_val);

        cpu->PC = 0x0000;
        cpu->Y = offset; // offset lives in Y
        cpu->A = 0x11;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles + 1);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, testing_val);
        TS_ASSERT_EQUALS(cpu->Y, offset);

        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithZeroAndZeroPageCrossed( void ) {
        // this tests reading the address from zp with wrap:
        // reading 2 bytes from zp addr 0xFF should read 0x00FF and 0x0000, NOT 0x00FF and 0x0100
        // the MSB should be the opcode at 0x0000 and the LSB should be at 0x00FF
        const Byte testing_val = 0x00;
        const Byte zp_addr = 0x1F;
        const Byte offset = 0x44;
        const Word addr = (Word(opcode) << 8) + 0x0011;
        const Byte d[] = {
            opcode, zp_addr, 0xFF
        };
        const Byte fake_msb = 0x11;
        mem->LoadFromData(d, sizeof(d));

        // write origin addr to zp address
        mem->WriteByte(0x0000 + zp_addr, addr & 0xFF );

        // write fake msb to 0x0100
        mem->WriteByte(0x0100, fake_msb);

        // write target value to addr + offset, as well as guard values
        const Byte bad_val = 0xFF;
        mem->WriteByte((Word(fake_msb) << 8) + (addr&0xFF), bad_val);
        mem->WriteByte((Word(fake_msb) << 8) + (addr&0xFF) + offset, bad_val);
        mem->WriteByte(addr + offset, testing_val);
        mem->WriteByte(addr, bad_val);
        mem->WriteByte(addr + offset - 1, bad_val);
        mem->WriteByte(addr + offset + 1, bad_val);

        cpu->PC = 0x0000;
        cpu->Y = offset; // offset lives in Y
        cpu->A = 0x11;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, testing_val);
        TS_ASSERT_EQUALS(cpu->Y, offset);

        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNonZero( void ) {
        const Byte testing_val = 0x59;
        const Byte zp_addr = 0xDD;
        const Byte offset = 0x01;
        const Word addr = 0x8910;
        const Byte d[] = {
            opcode, zp_addr, 0xFF
        };
        mem->LoadFromData(d, sizeof(d));

        // write origin addr to zp address
        mem->WriteWord(0x0000 + zp_addr, addr);

        // write target value to addr + offset, as well as guard values
        const Byte bad_val = 0x00;
        mem->WriteByte(addr + offset, testing_val);
        mem->WriteByte(addr, bad_val);
        mem->WriteByte(addr + offset - 1, bad_val);
        mem->WriteByte(addr + offset + 1, bad_val);

        cpu->PC = 0x0000;
        cpu->Y = offset; // offset lives in Y
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, testing_val);
        TS_ASSERT_EQUALS(cpu->Y, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNegative( void ) {
        const Byte testing_val = 0xF1;
        const Byte zp_addr = 0x0D;
        const Byte offset = 0x01;
        const Word addr = 0x0210;
        const Byte d[] = {
            opcode, zp_addr, 0xFF
        };
        mem->LoadFromData(d, sizeof(d));

        // write origin addr to zp address
        mem->WriteWord(0x0000 + zp_addr, addr);

        // write target value to addr + offset, as well as guard values
        const Byte bad_val = 0x00;
        mem->WriteByte(addr + offset, testing_val);
        mem->WriteByte(addr, bad_val);
        mem->WriteByte(addr + offset - 1, bad_val);
        mem->WriteByte(addr + offset + 1, bad_val);

        cpu->PC = 0x0000;
        cpu->Y = offset; // offset lives in Y
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, testing_val);
        TS_ASSERT_EQUALS(cpu->Y, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
        TS_ASSERT_EQUALS(cpu->PC, 2);
    }
    
};