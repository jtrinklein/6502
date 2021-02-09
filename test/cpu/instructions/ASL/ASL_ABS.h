#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class ASL_Absolute_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x0E;
    static constexpr Byte op_size = 3;
    static constexpr Byte op_cycles = 6;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
        cpu->PC = 0x0000;
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithZero_NoCarrySet( void ) {
        const Byte test_val  = 0b00000000;
        const Byte expect_val = 0b00000000;
        const Word addr = 0x5432;
        const Byte d[] = {opcode, 0,0, 0xFF};
        mem->LoadFromData(d, sizeof(d));

        mem->WriteWord(1, addr);
        mem->WriteWord(addr, test_val);

        cpu->A = 0x11;
        cpu->Carry = 0;
        cpu->Zero = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(addr), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }
    void test_WithZero_CarrySet( void ) {
        const Byte test_val  = 0b00000000;
        const Byte expect_val = 0b00000000;
        const Word addr = 0x04FB;
        const Byte d[] = {opcode, 0,0, 0xFF};
        mem->LoadFromData(d, sizeof(d));

        mem->WriteWord(1, addr);
        mem->WriteWord(addr, test_val);
        cpu->A = 0x11;
        cpu->Carry = 1;
        cpu->Zero = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(addr), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }
    void test_WithNonZero_NoCarrySet_NoCarry( void ) {
        const Byte test_val  = 0b00110111;
        const Byte expect_val = 0b01101110;
        const Word addr = 0xDCBA;
        const Byte d[] = {opcode, 0,0, 0xFF};
        mem->LoadFromData(d, sizeof(d));

        mem->WriteWord(1, addr);
        mem->WriteWord(addr, test_val);


        cpu->A = 0x00;
        cpu->Carry = 0;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(addr), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }
    void test_WithNonZero_CarrySet_NoCarry( void ) {
        const Byte test_val  = 0b00110111;
        const Byte expect_val = 0b01101110;
        const Word addr = 0x4444;
        const Byte d[] = {opcode, 0,0, 0xFF};
        mem->LoadFromData(d, sizeof(d));

        mem->WriteWord(1, addr);
        mem->WriteWord(addr, test_val);

        cpu->A = 0x00;
        cpu->Carry = 1;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(addr), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }
    void test_WithNonZero_NoCarrySet_WithCarry( void ) {
        const Byte test_val  = 0b10110111;
        const Byte expect_val = 0b01101110;
        const Word addr = 0x123F;
        const Byte d[] = {opcode, 0,0, 0xFF};
        mem->LoadFromData(d, sizeof(d));

        mem->WriteWord(1, addr);
        mem->WriteWord(addr, test_val);

        cpu->A = 0x00;
        cpu->Carry = 0;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(addr), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 1);
    }
    void test_WithNonZero_CarrySet_WithCarry( void ) {
        const Byte test_val  = 0b10110111;
        const Byte expect_val = 0b01101110;
        const Word addr = 0xCC11;
        const Byte d[] = {opcode, 0,0, 0xFF};
        mem->LoadFromData(d, sizeof(d));

        mem->WriteWord(1, addr);
        mem->WriteWord(addr, test_val);

        cpu->A = 0x00;
        cpu->Carry = 1;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(addr), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 1);
    }
};