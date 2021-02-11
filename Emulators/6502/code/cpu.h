//
//	cpu.h
//
#pragma once

class Memory;

/*
====================================================
ProcessorStatus_t
====================================================
*/
enum ProcessorStatus_t {
	PS_CarryFlag		= ( 1 << 0 ),	// Carry Flag			- Set when the last operation results in an overflow from bit 7 or underflow from bit 0.
	PS_ZeroFlag			= ( 1 << 1 ),	// Zero Flag			- Set when the last operation results in a zero.
	PS_InterruptDisable	= ( 1 << 2 ),	// Interrupt Disable	- Set when a 'Set Interrupt Disable' is executed.  CPU ignores interrupts until a 'Clear Interrupt Disable' is executed.
	PS_DecimalMode		= ( 1 << 3 ),	// Decimal Mode			- When set, the CPU obeys the rules of Binary Codded Decimal arithmetic.
	PS_BreakCommand		= ( 1 << 4 ),	// Break Command		- Set when a BRK instruction has been executed, and an interrupt generated to process it.
	PS_OverflowFlag		= ( 1 << 5 ),	// Overflow Flag		- Set when the last operation results in an overflow.
	PS_NegativeFlag		= ( 1 << 6 ),	// Negative Flag		- Set when the last operation results in bit 7 set to one.
};

/*
====================================================
CPU_t
Represents the state of a 6502 processor
====================================================
*/
struct CPU_t {
	uint16_t PC;	// Program Counter	- 16 bit register which points to the next instruction to be executed.
	uint8_t SP;		// Stack Pointer	- 8 bit register - 256 byte stack located between $0100 and $01FF, holds the location of the next free location.  No overflow protections.
	uint8_t A;		// Accumulator		- 8 bit accumulator for arithmetic and logical operations.
	uint8_t X;		// Index Register X	- 8 bit register - commonly used for holding counters of offsets into memory.
	uint8_t Y;		// Index Register Y	- 8 bit register - commonly used for holding counters of offsets into memory.
	struct flags_t {
		bool C : 1;	// Carry Flag
		bool Z : 1;	// Zero Flag
		bool I : 1;	// Interrupt Disable
		bool D : 1;	// Decimal Mode
		bool B : 1;	// Break Command
		bool V : 1;	// Overflow Flag
		bool N : 1;	// Negative Flag
		bool pad : 1;	// not used
	};
	union {
		uint8_t PS;		// Processor Status	- 8 bit register - see the enum
		flags_t flags;	// This is only used for debugging purposes (makes it easier to see the flag values in the debugger)
	};
};

uint8_t FetchByte( const CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address );
uint8_t ReadMemory( Memory * memory, uint16_t address );
void WriteMemory( Memory * memory, uint16_t address, uint8_t data );

uint8_t PopStack( CPU_t * cpu, Memory * memory );
void PushStack( CPU_t * cpu, Memory * memory, uint8_t data );

bool GetFlag( const CPU_t * cpu, const ProcessorStatus_t flag );
void SetFlag( CPU_t * cpu, const ProcessorStatus_t flag, const bool value );
