//
//	Instructions.cpp
//
#include "cpu.h"
#include "Instructions.h"
#include <assert.h>

/*
====================================================
g_instructionTable
This was originally a giant switch/case statement.
However, One Lone Coder used a lookup table in his
video series on 6502 emulation.  And I thought
that was a more elegant solution.
====================================================
*/
const Instruction_t g_instructionTable[ 16 * 16 ] = {
	// Row 0
	{ OpCodes::BRK, AddressModes::IMM, 7 },
	{ OpCodes::ORA, AddressModes::INDX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::ORA, AddressModes::ZP, 3 },
	{ OpCodes::ASL, AddressModes::ZP, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::PHP, AddressModes::IMP, 3 },
	{ OpCodes::ORA, AddressModes::IMM, 2 },
	{ OpCodes::ASL, AddressModes::ACC, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::ORA, AddressModes::ABS, 4 },
	{ OpCodes::ASL, AddressModes::ABS, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row 1
	{ OpCodes::BPL, AddressModes::REL, 2 },
	{ OpCodes::ORA, AddressModes::INDY, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::ORA, AddressModes::ZPX, 4 },
	{ OpCodes::ASL, AddressModes::ZPX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::CLC, AddressModes::IMP, 2 },
	{ OpCodes::ORA, AddressModes::ABSY, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::ORA, AddressModes::ABSX, 4 },
	{ OpCodes::ASL, AddressModes::ABSX, 7 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row 2
	{ OpCodes::JSR, AddressModes::ABS, 6 },
	{ OpCodes::AND, AddressModes::INDX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::BIT, AddressModes::ZP, 3 },
	{ OpCodes::AND, AddressModes::ZP, 3 },
	{ OpCodes::ROL, AddressModes::ZP, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::PLP, AddressModes::IMP, 4 },
	{ OpCodes::AND, AddressModes::IMM, 2 },
	{ OpCodes::ROL, AddressModes::ACC, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::BIT, AddressModes::ABS, 4 },
	{ OpCodes::AND, AddressModes::ABS, 4 },
	{ OpCodes::ROL, AddressModes::ABS, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row 3
	{ OpCodes::BMI, AddressModes::REL, 2 },
	{ OpCodes::AND, AddressModes::INDY, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::AND, AddressModes::ZPX, 4 },
	{ OpCodes::ROL, AddressModes::ZPX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::SEC, AddressModes::IMP, 2 },
	{ OpCodes::AND, AddressModes::ABSY, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::AND, AddressModes::ABSX, 4 },
	{ OpCodes::ROL, AddressModes::ABSX, 7 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row 4
	{ OpCodes::RTI, AddressModes::IMP, 6 },
	{ OpCodes::EOR, AddressModes::INDX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::EOR, AddressModes::ZP, 3 },
	{ OpCodes::LSR, AddressModes::ZP, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::PHA, AddressModes::IMP, 3 },
	{ OpCodes::EOR, AddressModes::IMM, 2 },
	{ OpCodes::LSR, AddressModes::ACC, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::JMP, AddressModes::ABS, 3 },
	{ OpCodes::EOR, AddressModes::ABS, 4 },
	{ OpCodes::LSR, AddressModes::ABS, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row 5
	{ OpCodes::BVC, AddressModes::REL, 2 },
	{ OpCodes::EOR, AddressModes::INDY, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::EOR, AddressModes::ZPX, 4 },
	{ OpCodes::LSR, AddressModes::ZPX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::CLI, AddressModes::IMP, 2 },
	{ OpCodes::EOR, AddressModes::ABSY, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::EOR, AddressModes::ABSX, 4 },
	{ OpCodes::LSR, AddressModes::ABSX, 7 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row 6
	{ OpCodes::RTS, AddressModes::IMP, 6 },
	{ OpCodes::ADC, AddressModes::INDX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::ADC, AddressModes::ZP, 3 },
	{ OpCodes::ROR, AddressModes::ZP, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::PLA, AddressModes::IMP, 4 },
	{ OpCodes::ADC, AddressModes::IMM, 2 },
	{ OpCodes::ROR, AddressModes::ACC, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::JMP, AddressModes::IND, 5 },
	{ OpCodes::ADC, AddressModes::ABS, 4 },
	{ OpCodes::ROR, AddressModes::ABS, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row 7
	{ OpCodes::BVS, AddressModes::REL, 2 },
	{ OpCodes::ADC, AddressModes::INDY, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::ADC, AddressModes::ZPX, 4 },
	{ OpCodes::ROR, AddressModes::ZPX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::SEI, AddressModes::IMP, 2 },
	{ OpCodes::ADC, AddressModes::ABSY, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::ADC, AddressModes::ABSX, 4 },
	{ OpCodes::ROR, AddressModes::ABSX, 7 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row 8
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::STA, AddressModes::INDX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::STY, AddressModes::ZP, 3 },
	{ OpCodes::STA, AddressModes::ZP, 3 },
	{ OpCodes::STX, AddressModes::ZP, 3 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::DEY, AddressModes::IMP, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::TXA, AddressModes::IMP, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::STY, AddressModes::ABS, 4 },
	{ OpCodes::STA, AddressModes::ABS, 4 },
	{ OpCodes::STX, AddressModes::ABS, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row 9
	{ OpCodes::BCC, AddressModes::REL, 2 },
	{ OpCodes::STA, AddressModes::INDY, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::STY, AddressModes::ZPX, 4 },
	{ OpCodes::STA, AddressModes::ZPX, 4 },
	{ OpCodes::STX, AddressModes::ZPY, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::TYA, AddressModes::IMP, 2 },
	{ OpCodes::STA, AddressModes::ABSY, 5 },
	{ OpCodes::TXS, AddressModes::IMP, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::STA, AddressModes::ABSX, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row A
	{ OpCodes::LDY, AddressModes::IMM, 2 },
	{ OpCodes::LDA, AddressModes::INDX, 6 },
	{ OpCodes::LDX, AddressModes::IMM, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::LDY, AddressModes::ZP, 3 },
	{ OpCodes::LDA, AddressModes::ZP, 3 },
	{ OpCodes::LDX, AddressModes::ZP, 3 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::TAY, AddressModes::IMP, 2 },
	{ OpCodes::LDA, AddressModes::IMM, 2 },
	{ OpCodes::TAX, AddressModes::IMP, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::LDY, AddressModes::ABS, 4 },
	{ OpCodes::LDA, AddressModes::ABS, 4 },
	{ OpCodes::LDX, AddressModes::ABS, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row B
	{ OpCodes::BCS, AddressModes::REL, 2 },
	{ OpCodes::LDA, AddressModes::INDY, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::LDY, AddressModes::ZPX, 4 },
	{ OpCodes::LDA, AddressModes::ZPX, 4 },
	{ OpCodes::LDX, AddressModes::ZPY, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::CLV, AddressModes::IMP, 2 },
	{ OpCodes::LDA, AddressModes::ABSY, 4 },
	{ OpCodes::TSX, AddressModes::IMP, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::LDY, AddressModes::ABSX, 4 },
	{ OpCodes::LDA, AddressModes::ABSX, 4 },
	{ OpCodes::LDX, AddressModes::ABSY, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row C
	{ OpCodes::CPY, AddressModes::IMM, 2 },
	{ OpCodes::CMP, AddressModes::INDX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::CPY, AddressModes::ZP, 3 },
	{ OpCodes::CMP, AddressModes::ZP, 3 },
	{ OpCodes::DEC, AddressModes::ZP, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::INY, AddressModes::IMP, 2 },
	{ OpCodes::CMP, AddressModes::IMM, 2 },
	{ OpCodes::DEX, AddressModes::IMP, 2 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::CPY, AddressModes::ABS, 4 },
	{ OpCodes::CMP, AddressModes::ABS, 4 },
	{ OpCodes::DEC, AddressModes::ABS, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row D
	{ OpCodes::BNE, AddressModes::REL, 2 },
	{ OpCodes::CMP, AddressModes::INDY, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::CMP, AddressModes::ZPX, 4 },
	{ OpCodes::DEC, AddressModes::ZPX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::CLD, AddressModes::IMP, 2 },
	{ OpCodes::CMP, AddressModes::ABSY, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::CMP, AddressModes::ABSX, 4 },
	{ OpCodes::DEC, AddressModes::ABSX, 7 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row E
	{ OpCodes::CPX, AddressModes::IMM, 2 },
	{ OpCodes::SBC, AddressModes::INDX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::CPX, AddressModes::ZP, 3 },
	{ OpCodes::SBC, AddressModes::ZP, 3 },
	{ OpCodes::INC, AddressModes::ZP, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::INX, AddressModes::IMP, 2 },
	{ OpCodes::SBC, AddressModes::IMM, 2 },
	{ OpCodes::NOP, AddressModes::IMP, 2 },
	{ OpCodes::SBC, AddressModes::IMP, 2 },
	{ OpCodes::CPX, AddressModes::ABS, 4 },
	{ OpCodes::SBC, AddressModes::ABS, 4 },
	{ OpCodes::INC, AddressModes::ABS, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },

	// Row F
	{ OpCodes::BEQ, AddressModes::REL, 2 },
	{ OpCodes::SBC, AddressModes::INDY, 5 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::SBC, AddressModes::ZPX, 4 },
	{ OpCodes::INC, AddressModes::ZPX, 6 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::SED, AddressModes::IMP, 2 },
	{ OpCodes::SBC, AddressModes::ABSY, 4 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
	{ OpCodes::SBC, AddressModes::ABSX, 4 },
	{ OpCodes::INC, AddressModes::ABSX, 7 },
	{ OpCodes::XXX, AddressModes::XXX, 255 },
};

/*
====================================================
ExecuteInstruction
====================================================
*/
uint8_t ExecuteInstruction( CPU_t * cpu, Memory * memory, uint8_t opCode ) {
	uint8_t cycles = g_instructionTable[ opCode ].baseCost;

 	addressMode_functor * addressMode = g_instructionTable[ opCode ].addressMode;
 	instruction_functor * operation = g_instructionTable[ opCode ].operation;

	bool didCrossPage = false;
	uint16_t address = addressMode( cpu, memory, didCrossPage );
 	cycles += operation( cpu, memory, opCode, address, didCrossPage );

	return cycles;
}













/*
========================================================================================================

Load/Store Instructions

========================================================================================================
*/

/*
====================================================
OpCodes::LDA
load accumulator
====================================================
*/
uint8_t OpCodes::LDA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	cpu->A = fetchedByte;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->A ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->A & ( 1 << 7 ) ) );
	return ( didCrossPage ? 1 : 0 );
}

/*
====================================================
OpCodes::LDX
load X
====================================================
*/
uint8_t OpCodes::LDX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );
	cpu->X = fetchedByte;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->X ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->X & ( 1 << 7 ) ) );
	return ( didCrossPage ? 1 : 0 );
}

/*
====================================================
OpCodes::LDY
load Y
====================================================
*/
uint8_t OpCodes::LDY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );
	cpu->Y = fetchedByte;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->Y ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->Y & ( 1 << 7 ) ) );
	return ( didCrossPage ? 1 : 0 );
}

/*
====================================================
OpCodes::STA
store accumulator
====================================================
*/
uint8_t OpCodes::STA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	WriteMemory( memory, address, cpu->A );
	return 0;
}

/*
====================================================
OpCodes::STX
store X
====================================================
*/
uint8_t OpCodes::STX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	WriteMemory( memory, address, cpu->X );
	return 0;
}

/*
====================================================
OpCodes::STY
store Y
====================================================
*/
uint8_t OpCodes::STY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	WriteMemory( memory, address, cpu->Y );
	return 0;
}










/*
========================================================================================================

Transfer Instructions

========================================================================================================
*/

/*
====================================================
OpCodes::TAX
transfer accumulator to X
====================================================
*/
uint8_t OpCodes::TAX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->X = cpu->A;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->X ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->X & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::TAY
transfer accumulator to Y
====================================================
*/
uint8_t OpCodes::TAY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->Y = cpu->A;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->Y ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->Y & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::TSX
transfer stack pointer to X
====================================================
*/
uint8_t OpCodes::TSX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->X = cpu->SP;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->X ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->X & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::TXA
transfer X to accumulator
====================================================
*/
uint8_t OpCodes::TXA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->A = cpu->X;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->A ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->A & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::TXS
transfer X to stack pointer
====================================================
*/
uint8_t OpCodes::TXS( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->SP = cpu->X;
	return 0;
}

/*
====================================================
OpCodes::TYA
transfer Y to accumulator
====================================================
*/
uint8_t OpCodes::TYA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->A = cpu->Y;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->A ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->A & ( 1 << 7 ) ) );
	return 0;
}




















/*
========================================================================================================

Stack Instructions

========================================================================================================
*/

/*
====================================================
OpCodes::PHA
push accumulator to stack
====================================================
*/
uint8_t OpCodes::PHA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	PushStack( cpu, memory, cpu->A );
	return 0;
}

/*
====================================================
OpCodes::PHP
push status flags to stack
====================================================
*/
uint8_t OpCodes::PHP( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t statusFlags = cpu->PS | ProcessorStatus_t::PS_BreakCommand;
	SetFlag( cpu, ProcessorStatus_t::PS_BreakCommand, 0 );

	PushStack( cpu, memory, statusFlags );
	return 0;
}

/*
====================================================
OpCodes::PLA
pull/pop accumulator from stack
====================================================
*/
uint8_t OpCodes::PLA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->A = PopStack( cpu, memory );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->A ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->A & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::PLP
pull/pop status flags from stack
====================================================
*/
uint8_t OpCodes::PLP( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->PS = PopStack( cpu, memory );
	return 0;
}



















/*
========================================================================================================

Logical Operations

========================================================================================================
*/

/*
====================================================
OpCodes::AND
====================================================
*/
uint8_t OpCodes::AND( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	cpu->A = cpu->A & fetchedByte;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->A ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->A & ( 1 << 7 ) ) );

	return ( didCrossPage ? 1 : 0 );
}


/*
====================================================
OpCodes::EOR
exclusive or
====================================================
*/
uint8_t OpCodes::EOR( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	cpu->A = cpu->A ^ fetchedByte;	
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->A ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->A & ( 1 << 7 ) ) );
	return ( didCrossPage ? 1 : 0 );
}


/*
====================================================
OpCodes::ORA
bitwise or
====================================================
*/
uint8_t OpCodes::ORA( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	cpu->A = cpu->A | fetchedByte;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->A ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->A & ( 1 << 7 ) ) );
	return ( didCrossPage ? 1 : 0 );
}


/*
====================================================
OpCodes::BIT
bitwise and, sets flags on the result
====================================================
*/
uint8_t OpCodes::BIT( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	uint8_t result = cpu->A & fetchedByte;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, fetchedByte & ( 1 << 7 ) );
	SetFlag( cpu, ProcessorStatus_t::PS_OverflowFlag, fetchedByte & ( 1 << 6 ) );
	return 0;
}


















/*
========================================================================================================

Arithmetic Instructions

========================================================================================================
*/

/*
====================================================
OpCodes::ADC
Add with carry
====================================================
*/
static bool DidOverFlowAdd( uint8_t a, uint8_t b, uint8_t result ) {
	uint8_t t = ( ~( a ^ b ) & ( a ^ result ) );
	return ( t & ( 1 << 7 ) );
}

uint8_t OpCodes::ADC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );
	
	uint16_t temp = cpu->A + fetchedByte;
	if ( GetFlag( cpu, ProcessorStatus_t::PS_CarryFlag ) ) {
		temp++;
	}
	
	uint8_t result = (uint8_t)temp;
	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, temp > 255 );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_OverflowFlag, DidOverFlowAdd( cpu->A, fetchedByte, result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( result & ( 1 << 7 ) ) );
	cpu->A = result;

	return ( didCrossPage ? 1 : 0 );
}

/*
====================================================
OpCodes::SBC
Subtract with carry
====================================================
*/
static bool DidOverFlowSub( uint8_t a, uint8_t b, uint8_t result ) {
	uint8_t t = ( ( result ^ a ) & ( result ^ b ) );
	return ( t & ( 1 << 7 ) );
}

uint8_t OpCodes::SBC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );
	uint8_t value = fetchedByte ^ 0xFF;
	
	uint16_t temp = cpu->A + value;
	if ( GetFlag( cpu, ProcessorStatus_t::PS_CarryFlag ) ) {
		temp++;
	}

	uint8_t result = (uint8_t)temp;
	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, temp > 255 );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_OverflowFlag, DidOverFlowSub( cpu->A, value, result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( result & ( 1 << 7 ) ) );
	cpu->A = result;

	return ( didCrossPage ? 1 : 0 );
}

/*
====================================================
OpCodes::CMP
compare accumulator
====================================================
*/
uint8_t OpCodes::CMP( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	uint8_t result = cpu->A - fetchedByte;
	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, cpu->A >= fetchedByte );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, cpu->A == fetchedByte );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( result & ( 1 << 7 ) ) );
	return ( didCrossPage ? 1 : 0 );
}

/*
====================================================
OpCodes::CPX
compare X
====================================================
*/
uint8_t OpCodes::CPX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	uint8_t result = cpu->X - fetchedByte;
	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, cpu->X >= fetchedByte );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, cpu->X == fetchedByte );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( result & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::CPY
compare Y
====================================================
*/
uint8_t OpCodes::CPY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	uint8_t result = cpu->Y - fetchedByte;
	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, cpu->Y >= fetchedByte );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, cpu->Y == fetchedByte );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( result & ( 1 << 7 ) ) );
	return 0;
}























/*
========================================================================================================

Increment/Decrement Instructions

========================================================================================================
*/

/*
====================================================
OpCodes::INC
increment value
====================================================
*/
uint8_t OpCodes::INC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t result = FetchByte( cpu, memory, opCode, address );
	result++;

	WriteMemory( memory, address, result );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( result & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::INX
increment X
====================================================
*/
uint8_t OpCodes::INX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->X++;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->X ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->X & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::INY
increment Y
====================================================
*/
uint8_t OpCodes::INY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->Y++;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->Y ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->Y & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::DEC
decrement value
====================================================
*/
uint8_t OpCodes::DEC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t result = FetchByte( cpu, memory, opCode, address );
	result--;

	WriteMemory( memory, address, result );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( result & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::DEX
decrement X
====================================================
*/
uint8_t OpCodes::DEX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->X--;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->X ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->X & ( 1 << 7 ) ) );
	return 0;
}

/*
====================================================
OpCodes::DEY
decrement Y
====================================================
*/
uint8_t OpCodes::DEY( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->Y--;
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == cpu->Y ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( cpu->Y & ( 1 << 7 ) ) );
	return 0;
}























/*
========================================================================================================

Shifts

========================================================================================================
*/

/*
====================================================
OpCodes::ASL
arithmetic shift left
====================================================
*/
uint8_t OpCodes::ASL( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	uint8_t result = fetchedByte << 1;
	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, ( fetchedByte & ( 1 << 7 ) ) );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( result & ( 1 << 7 ) ) );

	if ( AddressModes::ACC == g_instructionTable[ opCode ].addressMode ) {
		cpu->A = result;
	} else {
		WriteMemory( memory, address, result );
	}

	return 0;
}

/*
====================================================
OpCodes::LSR
logical shift right
====================================================
*/
uint8_t OpCodes::LSR( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );
	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, fetchedByte & 0x0001 );

	uint8_t result = fetchedByte >> 1;	
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag,  ( result & ( 1 << 7 ) ) );

	if ( AddressModes::ACC == g_instructionTable[ opCode ].addressMode ) {
		cpu->A = result;
	} else {
		WriteMemory( memory, address, result );
	}

	return 0;
}

/*
====================================================
OpCodes::ROL
rotate left
====================================================
*/
uint8_t OpCodes::ROL( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	uint8_t result = fetchedByte << 1;
	if ( GetFlag( cpu, ProcessorStatus_t::PS_CarryFlag ) ) {
		result |= 1;
	}
	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, ( fetchedByte & ( 1 << 7 ) ) );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag,  ( result & ( 1 << 7 ) ) );

	if ( AddressModes::ACC == g_instructionTable[ opCode ].addressMode ) {
		cpu->A = result;
	} else {
		WriteMemory( memory, address, result );
	}
	return 0;
}

/*
====================================================
OpCodes::ROR
rotate right
====================================================
*/
uint8_t OpCodes::ROR( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	uint8_t fetchedByte = FetchByte( cpu, memory, opCode, address );

	uint8_t result = fetchedByte >> 1;
	if ( GetFlag( cpu, ProcessorStatus_t::PS_CarryFlag ) ) {
		result |= ( 1 << 7 );
	}

	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, fetchedByte & 1 );
	SetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag, ( 0 == result ) );
	SetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag, ( result & ( 1 << 7 ) ) );

	if ( AddressModes::ACC == g_instructionTable[ opCode ].addressMode ) {
		cpu->A = result;
	} else {
		WriteMemory( memory, address, result );
	}

	return 0;
}





























/*
========================================================================================================

Jumps & Calls

========================================================================================================
*/

/*
====================================================
OpCodes::JMP
====================================================
*/
uint8_t OpCodes::JMP( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->PC = address;
	return 0;
}

/*
====================================================
OpCodes::JSR
jump to sub routine
====================================================
*/
uint8_t OpCodes::JSR( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->PC--;

	PushStack( cpu, memory, ( cpu->PC >> 8 ) & 0x00FF );
	PushStack( cpu, memory, cpu->PC & 0x00FF );

	cpu->PC = address;
	return 0;
}

/*
====================================================
OpCodes::RTS
return from stack
====================================================
*/
uint8_t OpCodes::RTS( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->PC = PopStack( cpu, memory );
	cpu->PC |= ( PopStack( cpu, memory ) << 8 );
	
	cpu->PC++;
	return 0;
}
























/*
========================================================================================================

Branching Operations

========================================================================================================
*/

/*
====================================================
Branch
====================================================
*/
static uint8_t Branch( CPU_t * cpu, uint16_t relAddr ) {
	uint16_t address = cpu->PC + relAddr;

	uint8_t cycles = 1;	// A successful branch costs one clock cycle
	
	// If address jumped to next page, add a clock cycle
	if ( ( address & 0xFF00 ) != ( cpu->PC & 0xFF00 ) ) {
		cycles++;
	}
	
	cpu->PC = address;
	return cycles;
}

/*
====================================================
OpCodes::BCC
branch carry clear
====================================================
*/
uint8_t OpCodes::BCC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	if ( GetFlag( cpu, ProcessorStatus_t::PS_CarryFlag ) ) {
		return 0;
	}

	return Branch( cpu, address );
}

/*
====================================================
OpCodes::BCS
branch carry set
====================================================
*/
uint8_t OpCodes::BCS( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	if ( !GetFlag( cpu, ProcessorStatus_t::PS_CarryFlag ) ) {
		return 0;
	}

	return Branch( cpu, address );
}

/*
====================================================
OpCodes::BEQ
branch equal
====================================================
*/
uint8_t OpCodes::BEQ( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	if ( !GetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag ) )	{
		return 0;
	}

	return Branch( cpu, address );
}

/*
====================================================
OpCodes::BMI
branch if negative
====================================================
*/
uint8_t OpCodes::BMI( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	if ( !GetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag ) )	{
		return 0;
	}

	return Branch( cpu, address );
}

/*
====================================================
OpCodes::BNE
branch if not equal
====================================================
*/
uint8_t OpCodes::BNE( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	if ( GetFlag( cpu, ProcessorStatus_t::PS_ZeroFlag ) ) {
		return 0;
	}

	return Branch( cpu, address );
}

/*
====================================================
OpCodes::BPL
branch if positive
====================================================
*/
uint8_t OpCodes::BPL( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	if ( GetFlag( cpu, ProcessorStatus_t::PS_NegativeFlag ) )	{
		return 0;
	}

	return Branch( cpu, address );
}

/*
====================================================
OpCodes::BVC
branch if overflow clear
====================================================
*/
uint8_t OpCodes::BVC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	if ( GetFlag( cpu, ProcessorStatus_t::PS_OverflowFlag ) ) {
		return 0;
	}

	return Branch( cpu, address );
}

/*
====================================================
OpCodes::BVS
branch if overflow set
====================================================
*/
uint8_t OpCodes::BVS( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	if ( !GetFlag( cpu, ProcessorStatus_t::PS_OverflowFlag ) ) {
		return 0;
	}

	return Branch( cpu, address );
}




















/*
========================================================================================================

Status Flag Operations

========================================================================================================
*/

/*
====================================================
OpCodes::SEC
set carry flag
====================================================
*/
uint8_t OpCodes::SEC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, true );
	return 0;
}

/*
====================================================
OpCodes::SED
set decimal flag
====================================================
*/
uint8_t OpCodes::SED( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	SetFlag( cpu, ProcessorStatus_t::PS_DecimalMode, true );
	return 0;
}

/*
====================================================
OpCodes::SEI
set interrupt flag
====================================================
*/
uint8_t OpCodes::SEI( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	SetFlag( cpu, ProcessorStatus_t::PS_InterruptDisable, true );
	return 0;
}

/*
====================================================
OpCodes::CLC
clear carry flag
====================================================
*/
uint8_t OpCodes::CLC( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	SetFlag( cpu, ProcessorStatus_t::PS_CarryFlag, false );
	return 0;
}

/*
====================================================
OpCodes::CLD
clear decimal flag
====================================================
*/
uint8_t OpCodes::CLD( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	SetFlag( cpu, ProcessorStatus_t::PS_DecimalMode, false );
	return 0;
}

/*
====================================================
OpCodes::CLI
clear interrupt flag
====================================================
*/
uint8_t OpCodes::CLI( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	SetFlag( cpu, ProcessorStatus_t::PS_InterruptDisable, false );
	return 0;
}

/*
====================================================
OpCodes::CLV
clear overflow flag
====================================================
*/
uint8_t OpCodes::CLV( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	SetFlag( cpu, ProcessorStatus_t::PS_OverflowFlag, false );
	return 0;
}


















/*
========================================================================================================

System Functions

========================================================================================================
*/

/*
====================================================
OpCodes::BRK
break
====================================================
*/
uint8_t OpCodes::BRK( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->PC++;
	
	SetFlag( cpu, ProcessorStatus_t::PS_InterruptDisable, true );
	PushStack( cpu, memory, ( cpu->PC >> 8 ) & 0x00FF );
	PushStack( cpu, memory, cpu->PC & 0x00FF );

	SetFlag( cpu, ProcessorStatus_t::PS_BreakCommand, true );
	PushStack( cpu, memory, cpu->PS );
	SetFlag( cpu, ProcessorStatus_t::PS_BreakCommand, false );

	cpu->PC = (uint16_t)ReadMemory( memory, 0xFFFE ) | ((uint16_t)ReadMemory( memory, 0xFFFF ) << 8 );
	return 0;
}

/*
====================================================
OpCodes::NOP
====================================================
*/
uint8_t OpCodes::NOP( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	return 0;
}

/*
====================================================
OpCodes::RTI
return from interrupt
====================================================
*/
uint8_t OpCodes::RTI( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	cpu->PS = PopStack( cpu, memory );
	cpu->PS &= ~ProcessorStatus_t::PS_BreakCommand;

	cpu->PC = PopStack( cpu, memory );
	cpu->PC |= PopStack( cpu, memory ) << 8;
	return 0;
}











/*
========================================================================================================

Undefined Behavior

========================================================================================================
*/


/*
====================================================
OpCodes::XXX
Undefined behavior
====================================================
*/
uint8_t OpCodes::XXX( CPU_t * cpu, Memory * memory, uint8_t opCode, uint16_t address, bool didCrossPage ) {
	printf( "This function should never get called!\n" );
	assert( false );
	return 0;
}

