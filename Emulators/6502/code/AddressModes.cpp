//
//	AddressModes.cpp
//
#include "cpu.h"
#include "AddressModes.h"
#include <assert.h>

/*
====================================================
AddressModes::ACC
accumulator
====================================================
*/
uint16_t AddressModes::ACC( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	// The accumulator address mode doesn't actually read from memory, it just uses the value in the A register.
	// Returning the end of addressable memory because this shouldn't happen in normal execution since the 6502 only had 64kb of memory.
	return 0xFFFF;
}

/*
====================================================
AddressModes::IMP
implied
====================================================
*/
uint16_t AddressModes::IMP( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	// The implied address mode doesn't actually read from memory.  It's only used for decrements/increments and pushing/popping the stack, etc.
	// Returning the end of addressable memory because this shouldn't happen in normal execution since the 6502 only had 64kb of memory.
	return 0xFFFF;
}

/*
====================================================
AddressModes::IMM
immediate
====================================================
*/
uint16_t AddressModes::IMM( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	return cpu->PC++;	
}

/*
====================================================
AddressModes::REL
relative (used for branching only)
====================================================
*/
uint16_t AddressModes::REL( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	uint16_t relAddress = ReadMemory( memory, cpu->PC );
	cpu->PC++;

	// If the relative address is negative put it on the last page.
	// This allows us to "subtract" by taking advantage of wrap around.
	if ( 0 != ( relAddress & ( 1 << 7 ) ) ) {
		relAddress |= 0xFF00;
	}
	return relAddress;
}

/*
========================================================================================================

Zero Page Addressing
Reads memory from the zero page only (first 256 bytes of memory)

========================================================================================================
*/

/*
====================================================
AddressModes::ZP
zero page
====================================================
*/
uint16_t AddressModes::ZP( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	uint8_t address = ReadMemory( memory, cpu->PC );	
	cpu->PC++;
	return address;
}

/*
====================================================
AddressModes::ZPX
zero page X
====================================================
*/
uint16_t AddressModes::ZPX( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	uint8_t address = ReadMemory( memory, cpu->PC ) + cpu->X;
	cpu->PC++;
	return address;
}

/*
====================================================
AddressModes::ZPY
zero page Y
====================================================
*/
uint16_t AddressModes::ZPY( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	uint8_t address = ReadMemory( memory, cpu->PC ) + cpu->Y;
	cpu->PC++;
	return address;
}

/*
========================================================================================================

Absolute Addressing
Similar to zero page, but can read from all pages

========================================================================================================
*/

/*
====================================================
ReadAbsolutAddress
====================================================
*/
static uint16_t ReadAbsolutAddress( CPU_t * cpu, Memory * memory ) {
	// read the low byte
	uint8_t addrLo = ReadMemory( memory, cpu->PC );
	cpu->PC++;

	// read the high byte
	uint8_t addrHi = ReadMemory( memory, cpu->PC );
	cpu->PC++;

	// combine the bytes
	uint16_t address = ( addrHi << 8 ) | addrLo;
	return address;
}

/*
====================================================
AddressModes::ABS
absolute
====================================================
*/
uint16_t AddressModes::ABS( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	return ReadAbsolutAddress( cpu, memory );
}

/*
====================================================
AddressModes::ABSX
absolute X
====================================================
*/
uint16_t AddressModes::ABSX( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	uint16_t absAddress = ReadAbsolutAddress( cpu, memory );
	uint16_t address = absAddress + cpu->X;

	// Check if the address has rolled into the next page
	didCrossPage = ( ( address >> 8 ) != ( absAddress >> 8 ) );

	return address;
}

/*
====================================================
AddressModes::ABSY
aboslute Y
====================================================
*/
uint16_t AddressModes::ABSY( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	uint16_t absAddress = ReadAbsolutAddress( cpu, memory );
	uint16_t address = absAddress + cpu->Y;

	// Check if the address has rolled into the next page
	didCrossPage = ( ( address >> 8 ) != ( absAddress >> 8 ) );

	return address;
}


/*
========================================================================================================

Indirect Addressing
Effectively it's how 6502 handles pointers

========================================================================================================
*/

/*
====================================================
AddressModes::IND
indirect
====================================================
*/
uint16_t AddressModes::IND( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	uint16_t ptr = ReadAbsolutAddress( cpu, memory );

	uint8_t addrLo = ReadMemory( memory, ptr + 0 );
	uint8_t addrHi = ReadMemory( memory, ptr + 1 );

	uint16_t address = ( addrHi << 8 ) | addrLo;

	// 6502 had a weird hardware bug, simulate it
	if ( 0x00FF == ( ptr >> 8 ) ) {
		addrHi = ReadMemory( memory, ptr & 0xFF00 );
		address = ( addrHi << 8 ) | addrLo;
	}
	
	return address;
}

/*
====================================================
AddressModes::INDX
indexed indirect
====================================================
*/
uint16_t AddressModes::INDX( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	uint8_t zp = ReadMemory( memory, cpu->PC ) + cpu->X;
	cpu->PC++;

	uint8_t addrLo = ReadMemory( memory, zp + 0 );
	uint8_t addrHi = ReadMemory( memory, zp + 1 );

	uint16_t address = ( addrHi << 8 ) | addrLo;	
	return address;
}

/*
====================================================
AddressModes::INDY
indirect indexed
====================================================
*/
uint16_t AddressModes::INDY( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	uint8_t zp = ReadMemory( memory, cpu->PC );
	cpu->PC++;

	uint8_t addrLo = ReadMemory( memory, zp + 0 );
	uint8_t addrHi = ReadMemory( memory, zp + 1 );

	uint16_t address = ( addrHi << 8 ) | addrLo;	
	address += cpu->Y;
	
	// Check if the address has rolled into the next page
	didCrossPage = ( ( address >> 8 ) != addrHi );
	
	return address;
}


/*
========================================================================================================

Invalid

========================================================================================================
*/

/*
====================================================
AddressModes::XXX
====================================================
*/
uint16_t AddressModes::XXX( CPU_t * cpu, Memory * memory, bool & didCrossPage ) {
	printf( "This function should never get called!\n" );
	assert( false );
	return 0xFFFF;
}