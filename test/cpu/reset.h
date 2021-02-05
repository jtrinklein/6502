#include "../../lib/cxxtest-4.4/cxxtest/TestSuite.h"
#include "../../cpu.h"

class ResetTests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    
    void setUp() {
        // runs for each test
        cpu = new CPU(nullptr);
    }
    
    void tearDown() {
        //runs for each test
        delete cpu;
    }

    void test_should_set_InterruptDisable_flag( void ) {
        cpu->InterruptDisable = 0;
        cpu->Reset();
        TS_ASSERT_EQUALS( cpu->InterruptDisable, 1 );
    }
};
