#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class AND_Indirect_Y_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x31;
    static constexpr Byte op_size = 2;
    static constexpr Byte op_cycles = 5;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
        cpu->PC = 0x0000;
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_Should_SetZero_ClearNegative( void ) {
        const Byte A_val    = 0b10101001;
        const Byte test_val = 0b00010010;
        const Byte expect_val = A_val & test_val;
        const Byte offset = 0x1F;
        const Byte zp_addr = 0x84;
        const Word addr = 0x9870;
        const Byte d[] = {
            opcode, zp_addr, 0xFF
        };
        mem->LoadFromData(d, sizeof(d));

        // write origin addr to zp address
        mem->WriteWord(0x0000 + zp_addr, addr);

        // write target value to addr + offset, as well as guard values
        const Byte bad_val = 0xFF;
        mem->WriteByte(addr + offset, test_val);
        mem->WriteByte(addr, bad_val);
        mem->WriteByte(addr + offset - 1, bad_val);
        mem->WriteByte(addr + offset + 1, bad_val);

        cpu->Y = offset;
        cpu->A = A_val;
        cpu->Zero = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->Y, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_Should_ClearZero_ClearNegative( void ) {
        const Byte A_val    = 0b10101001;
        const Byte test_val = 0b00110010;
        const Byte expect_val = A_val & test_val;
        const Byte offset = 0x50;
        const Byte zp_addr = 0x84;
        const Word addr = 0x777F;
        const Byte d[] = {
            opcode, zp_addr, 0xFF
        };
        mem->LoadFromData(d, sizeof(d));

        // write origin addr to zp address
        mem->WriteWord(0x0000 + zp_addr, addr);

        // write target value to addr + offset, as well as guard values
        const Byte bad_val = 0xFF;
        mem->WriteByte(addr + offset, test_val);
        mem->WriteByte(addr, bad_val);
        mem->WriteByte(addr + offset - 1, bad_val);
        mem->WriteByte(addr + offset + 1, bad_val);

        cpu->Y = offset;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->Y, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_Should_ClearZero_SetNegative( void ) {
        const Byte A_val    = 0b10101001;
        const Byte test_val = 0b10010010;
        const Byte expect_val = A_val & test_val;
        const Byte offset = 0x69;
        const Byte zp_addr = 0x84;
        const Word addr = 0x815F;
        const Byte d[] = {
            opcode, zp_addr, 0xFF
        };
        mem->LoadFromData(d, sizeof(d));

        // write origin addr to zp address
        mem->WriteWord(0x0000 + zp_addr, addr);

        // write target value to addr + offset, as well as guard values
        const Byte bad_val = 0xFF;
        mem->WriteByte(addr + offset, test_val);
        mem->WriteByte(addr, bad_val);
        mem->WriteByte(addr + offset - 1, bad_val);
        mem->WriteByte(addr + offset + 1, bad_val);

        cpu->Y = offset;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->Y, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }

    void test_Should_ClearZero_SetNegative_WithPageCross( void ) {
        const Byte A_val    = 0b10101001;
        const Byte test_val = 0b10010010;
        const Byte expect_val = A_val & test_val;
        const Byte zp_addr = 0x1F;
        const Byte offset = 0x44;
        const Word addr = 0xABF7;
        const Byte d[] = {
            opcode, zp_addr, 0xFF
        };
        mem->LoadFromData(d, sizeof(d));

        // write origin addr to zp address
        mem->WriteWord(0x0000 + zp_addr, addr );

        // write target value to addr + offset, as well as guard values
        const Byte bad_val = 0xFF;
        mem->WriteByte(addr + offset, test_val);
        mem->WriteByte(addr, bad_val);
        mem->WriteByte(addr + offset - 1, bad_val);
        mem->WriteByte(addr + offset + 1, bad_val);

        cpu->Y = offset;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles+1);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->Y, offset);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }
};