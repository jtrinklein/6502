#include <cxxtest/TestSuite.h>
#include "../../mem.h"

class MemLoad_Tests : public CxxTest::TestSuite 
{
public:

    Mem* mem;
    static constexpr Byte testData[] = {0x00, 0x12, 0x34, 0x56, 0x00, 0x78};
    static constexpr size_t testDataSize = 6;

    void setUp() {
        mem= new Mem();
    }

    void tearDown() {
        delete mem;
    }

    void test_LoadFromData( void ) {
        mem->LoadFromData(testData, testDataSize);

        for (auto i = 0; i < testDataSize; ++i) {
            TS_ASSERT_EQUALS( mem->m_data[i],  testData[i]);
        }
    }

    void test_LoadFromDataAtOffset( void ) {
        auto offset = 0x8765;
        mem->LoadFromDataAtOffset(testData, testDataSize, offset);

        for (auto i = 0; i < testDataSize; ++i) {
            TS_ASSERT_EQUALS( mem->m_data[i + offset],  testData[i]);
        }
    }
};