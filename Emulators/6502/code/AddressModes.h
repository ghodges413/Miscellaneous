//
//	AddressModes.h
//
#pragma once

struct CPU_t;
class Memory;

typedef uint16_t (addressMode_functor)( CPU_t * cpu, Memory * memory, bool & didCrossPage );

/*
====================================================
AddressModes
====================================================
*/
class AddressModes {
public:
	static uint16_t ACC( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	static uint16_t IMP( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	static uint16_t IMM( CPU_t * cpu, Memory * memory, bool & didCrossPage );

	static uint16_t ZP( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	static uint16_t ZPX( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	static uint16_t ZPY( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	
	static uint16_t REL( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	static uint16_t ABS( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	static uint16_t ABSX( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	static uint16_t ABSY( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	
	static uint16_t IND( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	static uint16_t INDX( CPU_t * cpu, Memory * memory, bool & didCrossPage );
	static uint16_t INDY( CPU_t * cpu, Memory * memory, bool & didCrossPage );

	static uint16_t XXX( CPU_t * cpu, Memory * memory, bool & didCrossPage );
};