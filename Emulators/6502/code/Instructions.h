//
//	Instructions.h
//
#pragma once
#include "AddressModes.h"

class CPU_t;
class Memory;

typedef uint8_t (instruction_functor)( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );

/*
====================================================
OpCodes
====================================================
*/
class OpCodes {
public:
	// Load/Store Operations
	static uint8_t LDA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t LDX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t LDY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t STA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t STX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t STY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );

	// Register Transfers
	static uint8_t TAX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t TAY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t TXA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t TYA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );

	// Stack Operations
	static uint8_t TSX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t TXS( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t PHA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t PHP( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t PLA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t PLP( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );

	// Logical Operations
	static uint8_t AND( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t EOR( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t ORA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t BIT( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );

	// Arithmetic Operations
	static uint8_t ADC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t SBC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t CMP( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t CPX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t CPY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );

	// Increments & Decrements
	static uint8_t INC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t INX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t INY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t DEC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t DEX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t DEY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );

	// Shifts
	static uint8_t ASL( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t LSR( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t ROL( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t ROR( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );

	// Jumps & Calls
	static uint8_t JMP( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t JSR( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t RTS( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );

	// Branching Operations
	static uint8_t BCC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t BCS( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t BEQ( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t BMI( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t BNE( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t BPL( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t BVC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t BVS( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	
	// Status Flag Operations
	static uint8_t CLC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t CLD( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t CLI( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t CLV( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t SEC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t SED( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t SEI( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );

	// System Functions
	static uint8_t BRK( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	static uint8_t NOP( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );	
	static uint8_t RTI( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
	
	// Catch all for operations that do not exist
	static uint8_t XXX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage );
};

struct Instruction_t {
 	instruction_functor * operation = nullptr;
 	addressMode_functor * addressMode = nullptr;
	uint8_t baseCost = 0;
};

extern const Instruction_t g_instructionTable[ 16 * 16 ];



uint8_t ExecuteInstruction( CPU_t * cpu, Memory * memory, uint8_t opCode ); 