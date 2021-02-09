#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class ROL_ZeroPageX_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x36;
    static constexpr Byte op_size = 2;
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
        const Byte zp_addr = 0x02;
        const Byte offset = 0x31;
        const Byte d[] = {opcode, zp_addr};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr + offset, test_val);

        cpu->X = offset;
        cpu->A = 0x11;
        cpu->Carry = 0;
        cpu->Zero = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr + offset), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_WithZero_CarrySet( void ) {
        const Byte test_val  = 0b00000000;
        const Byte expect_val = 0b00000001;
        const Byte zp_addr = 0x02;
        const Byte offset = 0xC9;
        const Byte d[] = {opcode, zp_addr};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr + offset, test_val);

        cpu->X = offset;

        cpu->A = 0x11;
        cpu->Carry = 1;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr + offset), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }


    void test_WithNonZero_NoCarrySet_NoCarry( void ) {
        const Byte test_val  = 0b00110111;
        const Byte expect_val = 0b01101110;
        const Byte zp_addr = 0x15;
        const Byte offset = 0xFF;
        const Byte d[] = {opcode, zp_addr};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + ((zp_addr + offset)& 0xFF), test_val);

        cpu->X = offset;

        cpu->A = 0x00;
        cpu->Carry = 0;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte((zp_addr + offset) & 0xFF), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_WithNonZero_CarrySet_NoCarry( void ) {
        const Byte test_val  = 0b00110111;
        const Byte expect_val = 0b01101111;
        const Byte zp_addr = 0xf2;
        const Byte offset = 0x04;
        const Byte d[] = {opcode, zp_addr};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr + offset, test_val);

        cpu->X = offset;

        cpu->A = 0x00;
        cpu->Carry = 1;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr + offset), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_WithNonZero_NoCarrySet_WithCarry( void ) {
        const Byte test_val  = 0b11010111;
        const Byte expect_val = 0b10101110;
        const Byte zp_addr = 0x02;
        const Byte offset = 0x70;
        const Byte d[] = {opcode, zp_addr, test_val};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr + offset, test_val);

        cpu->X = offset;

        cpu->A = 0x00;
        cpu->Carry = 0;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr + offset), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
        TS_ASSERT_EQUALS(cpu->Carry, 1);
    }

    void test_WithNonZero_CarrySet_WithCarry( void ) {
        const Byte test_val  = 0b10110111;
        const Byte expect_val = 0b01101111;
        const Byte zp_addr = 0x55;
        const Byte offset = 0x01;
        const Byte d[] = {opcode, zp_addr, test_val};
        mem->LoadFromData(d, sizeof(d));
        mem->WriteByte(0x0000 + zp_addr + offset, test_val);

        cpu->X = offset;

        cpu->A = 0x00;
        cpu->Carry = 1;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr + offset), expect_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 1);
    }
};