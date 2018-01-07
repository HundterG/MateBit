#include "../Squirrel/squirrel.h"
#include "SQForwardDeclares.h"
#include <string>
#include "../Core/Audio.h"
#include "../Loaders/Loaders.h"

namespace
{
	SQInteger SQSetSquare1Note(HSQUIRRELVM v)
	{
		Note note;
		SQInteger read = -1;
		sq_getinteger(v, 2, &note.freq);
		sq_getinteger(v, 3, &read);
		if(Volume::vx0 <= read && read <= Volume::vx3)
			note.volume = Volume::Type(read);
		sq_getinteger(v, 4, &note.mode);
		sq_getinteger(v, 5, &note.length);
		sq_getinteger(v, 6, &read);
		note.priority = Priority::Type(read);

		AudioSetSquare1Note(note);
		return 0;
	}

	SQInteger SQSetSquare2Note(HSQUIRRELVM v)
	{
		Note note;
		SQInteger read = -1;
		sq_getinteger(v, 2, &note.freq);
		sq_getinteger(v, 3, &read);
		if(Volume::vx0 <= read && read <= Volume::vx3)
			note.volume = Volume::Type(read);
		sq_getinteger(v, 4, &note.mode);
		sq_getinteger(v, 5, &note.length);
		sq_getinteger(v, 6, &read);
		note.priority = Priority::Type(read);

		AudioSetSquare2Note(note);
		return 0;
	}

	SQInteger SQSetTriangleNote(HSQUIRRELVM v)
	{
		Note note;
		SQInteger read = -1;
		sq_getinteger(v, 2, &note.freq);
		sq_getinteger(v, 3, &note.length);
		sq_getinteger(v, 4, &read);
		note.priority = Priority::Type(read);

		AudioSetTriangleNote(note);
		return 0;
	}

	SQInteger SQSetNoiseNote(HSQUIRRELVM v)
	{
		Note note;
		SQInteger read = -1;
		sq_getinteger(v, 2, &note.freq);
		SQBool readbool = false;
		sq_getbool(v, 3, &readbool);
		note.mode = (readbool == SQTrue) ? 1 : 0;
		sq_getinteger(v, 4, &note.length);
		sq_getinteger(v, 5, &read);
		note.priority = Priority::Type(read);

		AudioSetNoiseNote(note);
		return 0;
	}

	SQInteger SQPlayBGM(HSQUIRRELVM v)
	{
		SQChar const *name = nullptr;
		SQBool fromLoop = SQFalse;

		sq_getstring(v, 2, &name);
		sq_getbool(v, 3, &fromLoop);

		PlayBGM(std::string(name), (fromLoop == SQTrue) ? true : false);
		return 0;
	}

	SQInteger SQPlaySFX(HSQUIRRELVM v)
	{
		SQChar const *name = nullptr;
		SQBool highPriority = SQFalse;

		sq_getstring(v, 2, &name);
		sq_getbool(v, 3, &highPriority);

		PlaySFX(std::string(name), (highPriority == SQTrue) ? true : false);
		return 0;
	}
}

#define BINDC(__NAME, __VAL) sq_pushstring(vm, _SC(__NAME), -1); sq_pushinteger(vm, __VAL); sq_newslot(vm, -3, SQTrue);
#define BINDF(__NAME, __FUNC, __PNUM, __PTYPE) sq_pushstring(vm, _SC(__NAME), -1); sq_newclosure(vm, __FUNC, 0); sq_setparamscheck(vm, __PNUM, __PTYPE); sq_setnativeclosurename(vm, -1, _SC(__NAME)); sq_newslot(vm, -3, SQFalse);

void AddAudio(HSQUIRRELVM vm)
{
	SQInteger top = sq_gettop(vm); //saves the stack size before the call

	sq_pushstring(vm, _SC("Audio"), -1);
	sq_newclass(vm, SQFalse);
	BINDC("Volume0", Volume::vx0);
	BINDC("Volume1", Volume::vx1);
	BINDC("Volume2", Volume::vx2);
	BINDC("Volume3", Volume::vx3);
	BINDC("Duty13", Duty::x13);
	BINDC("Duty25", Duty::x25);
	BINDC("Duty50", Duty::x50);
	BINDC("PriorityLow", Priority::Low);
	BINDC("PriorityMedium", Priority::Medium);
	BINDC("PriorityHigh", Priority::High);
	BINDF("SetSquare1Note", SQSetSquare1Note, 6, ".iiiii");
	BINDF("SetSquare2Note", SQSetSquare2Note, 6, ".iiiii");
	BINDF("SetTriangleNote", SQSetTriangleNote, 4, ".iii");
	BINDF("SetNoiseNote", SQSetNoiseNote, 5, ".ibii");
	BINDF("PlayBGM", SQPlayBGM, 3, ".sb");
	BINDF("PlaySFX", SQPlaySFX, 3, ".sb");
	sq_newslot(vm, -3, SQFalse);

	sq_settop(vm,top); //restores the original stack size
}
