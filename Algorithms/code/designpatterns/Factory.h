//
//  Factory.h
//
#pragma once

/*
===============================
Handheld
===============================
*/
class Handheld {
public:
	enum Handheld_t {
		NINTENDO,
		SEGA
	};

	virtual const char * GetName() = 0;
};

/*
===============================
Nintendo
===============================
*/
class Nintendo : public Handheld {
public:
	const char * GetName () override { return "Nintendo"; }
};

/*
===============================
Sega
===============================
*/
class Sega : public Handheld {
public:
	const char * GetName() override { return "Sega"; }
};

/*
===============================
Factory
===============================
*/
class Factory {
public:
	Handheld * GetPerson( const Handheld::Handheld_t & type ) {
		switch ( type ) {
			default:
			case Handheld::Handheld_t::NINTENDO: { return new Nintendo; } break;
			case Handheld::Handheld_t::SEGA: { return new Sega; } break;
		}
	}
};
