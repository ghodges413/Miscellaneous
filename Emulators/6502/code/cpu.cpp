//
//	cpu.cpp
//
#include "cpu.h"
#include "Instructions.h"
#include <assert.h>

/*
========================================================================================================

Memory

========================================================================================================
*/

/*
====================================================
Memory
====================================================
*/
class Memory {
public:
	uint8_t m_memory[ 64 * 1024 ];

	void    Write( uint16_t addr, uint8_t data ) override { m_memory[ addr ] = data; }
	uint8_t Read( uint16_t addr ) override { return m_memory[ addr ]; }
};

/*
====================================================
FetchByte
====================================================
*/
uint8_t FetchByte( const CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address ) {
	const Instruction_t & instruction = g_instructionTable[ opCode ];

	// The accumulator address mode doesn't read memory, it just uses the value in the A register
	if ( AddressModes::ACC == instruction.addressMode ) {
		return cpu->A;
	}

	// The implied mode also doesn't read memory
	if ( AddressModes::IMP == instruction.addressMode ) {
		printf( "This shouldn't get called\n" );
		assert( false );
		return 0;
	}

	return ReadMemory( memory, address );
}

/*
====================================================
ReadMemory
====================================================
*/
uint8_t ReadMemory( Memory * memory, uint16_t address ) {
	return memory->Read( address, false );
}

/*
====================================================
WriteMemory
====================================================
*/
void WriteMemory( Memory * memory, uint16_t address, uint8_t data ) {
	assert( address != 0xFFFF );
	return memory->Write( address, data );
}

/*
====================================================
PopStack
====================================================
*/
uint8_t PopStack( CPU_t * cpu, Memory * memory ) {
	cpu->SP++;
	return ReadMemory( memory, 0x0100 + cpu->SP );
}

/*
====================================================
PushStack
====================================================
*/
void PushStack( CPU_t * cpu, Memory * memory, uint8_t data ) {
	WriteMemory( memory, 0x0100 + cpu->SP, data );
	cpu->SP--;
}


/*
========================================================================================================

Flags

========================================================================================================
*/

/*
====================================================
GetFlag
====================================================
*/
bool GetFlag( const CPU_t * cpu, const ProcessorStatus_t flag ) {
	if ( ( cpu->PS & flag ) > 0 ) {
		return true;
	}

	return false;
}

/*
====================================================
SetFlag
====================================================
*/
void SetFlag( CPU_t * cpu, const ProcessorStatus_t flag, const bool value ) {
	if ( value ) {
		cpu->PS |= flag;
	} else {
		cpu->PS &= ~flag;
	}
}
