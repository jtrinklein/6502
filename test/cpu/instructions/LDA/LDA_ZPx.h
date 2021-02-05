#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class LDA_ZeroPageX_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0xB5;
    static constexpr Byte op_cycles = 4;
    static constexpr Byte op_size = 2;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithZero( void ) {
        const Byte test_val = 0x00;
        const Byte zp_addr = 0x03;
        const Byte offset = 0x01;
        const Byte d[] = {0xFF, 0x01, opcode, zp_addr, test_val, 0x05};
        mem->LoadFromData(d, sizeof(d));

        const Word pc_start = 0x0002;
        cpu->PC = pc_start;
        cpu->X = offset;
        cpu->A = 0x11;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->X, offset);
    }

    void test_WithZeroAndAddressWrap( void ) {
        const Byte test_val = 0x00;
        const Byte zp_addr = 0xFF;
        const Byte offset = 0x06;
        const Byte d[] = {0xFF, 0x01, opcode, zp_addr, 0xFF, test_val, 0xF5};
        mem->LoadFromData(d, sizeof(d));

        const Word wrapped_addr = 0x0000 + offset + zp_addr;
        mem->WriteByte(wrapped_addr, 0xFF); // 0xFF + 0x05 = 0x104, make sure 0x104 is non-zero

        const Word pc_start = 0x0002;
        cpu->PC = pc_start;
        cpu->X = offset;
        cpu->A = 0x11;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->X+zp_addr, wrapped_addr);
        TS_ASSERT_EQUALS((cpu->X+zp_addr) & 0xFF, 0x05);
    }

    void test_WithNonZero( void ) {
        const Byte test_val = 0x67;
        const Byte zp_addr = 0x01;
        const Byte offset = 0x01;
        const Byte d[] = {0x00, 0x00, test_val, 0x0, opcode, zp_addr, 0xff};

        mem->LoadFromData(d, sizeof(d));

        const Word pc_start = 0x0004;
        cpu->PC = pc_start;
        cpu->X = offset;
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->X, offset);
        TS_ASSERT_EQUALS(cpu->X+zp_addr, 0x02);
    }

    void test_WithNegative( void ) {
        const Byte test_val = 0xE7;
        const Byte zp_addr = 0x03;
        const Byte offset = 0x02;
        const Byte d[] = { 0x7F, 0x1, opcode, zp_addr,0x00, test_val};

        mem->LoadFromData(d, sizeof(d));

        const Word pc_start = 0x0002;
        cpu->PC = pc_start;
        cpu->X = offset;
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
        TS_ASSERT_EQUALS(cpu->X+zp_addr, 0x05);
    }
    
};