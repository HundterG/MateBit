#include "../Squirrel/squirrel.h"
#include "SQForwardDeclares.h"
#include "../Core/Globals.h"
#include <random>

namespace
{
	SQInteger SQExitGame(HSQUIRRELVM v)
	{
		Globals::quit = true;
		return 0;
	}

	SQInteger SQSetFullScreen(HSQUIRRELVM v)
	{
		SQBool b;
		sq_getbool(v, 2, &b);
		Globals::fullScreen = bool(b);
		return 0;
	}

	SQInteger SQGetFullScreen(HSQUIRRELVM v)
	{
		sq_pushbool(v, Globals::fullScreen);
		return 1;
	}

	SQInteger SQSetAspect(HSQUIRRELVM v)
	{
		SQInteger n;
		sq_getinteger(v,2,&n);
		if((n == Globals::Aspect::x11 || n == Globals::Aspect::x34 || n == Globals::Aspect::xany))
			Globals::aspectRatio = Globals::Aspect::Ratio(n);
		return 0;
	}

	SQInteger SQGetAspect(HSQUIRRELVM v)
	{
		sq_pushinteger(v, Globals::aspectRatio);
		return 1;
	}

	SQInteger SQRandInt(HSQUIRRELVM v)
	{
		SQInteger low = 0, high = 0;
		sq_getinteger(v, 2, &low);
		sq_getinteger(v, 3, &high);

		static std::default_random_engine gen;
		std::uniform_int_distribution<SQInteger> dist(low, high);
		sq_pushinteger(v, dist(gen));
		return 1;
	}

	SQInteger SQSetVolume(HSQUIRRELVM v)
	{
		SQFloat nv = 100.0f;
		if(sq_gettype(v, 1) == SQObjectType::OT_FLOAT)
			sq_getfloat(v, 2, &nv);
		else
		{
			SQInteger i = 100;
			sq_getinteger(v, 2, &i);
			nv = i;
		}
		if(nv < 0.0f) nv = 0.0f;
		if(100.0f < nv) nv = 100.0f;
		Globals::volume = nv;
		return 0;
	}

	SQInteger SQGetVolume(HSQUIRRELVM v)
	{
		sq_pushfloat(v, Globals::volume);
		return 1;
	}
}

#define BINDC(__NAME, __VAL) sq_pushstring(vm, _SC(__NAME), -1); sq_pushinteger(vm, __VAL); sq_newslot(vm, -3, SQTrue);
#define BINDF(__NAME, __FUNC, __PNUM, __PTYPE) sq_pushstring(vm, _SC(__NAME), -1); sq_newclosure(vm, __FUNC, 0); sq_setparamscheck(vm, __PNUM, __PTYPE); sq_setnativeclosurename(vm, -1, _SC(__NAME)); sq_newslot(vm, -3, SQFalse);

void AddGlobals(HSQUIRRELVM vm)
{
	SQInteger top = sq_gettop(vm); //saves the stack size before the call
	// create a table and put funcs and consts in there
	sq_pushstring(vm, _SC("Globals"), -1);
	sq_newclass(vm, SQFalse);

	BINDC("x11", Globals::Aspect::Ratio::x11);
	BINDC("x34", Globals::Aspect::Ratio::x34);
	BINDC("xany", Globals::Aspect::Ratio::xany);

	BINDF("Exit", SQExitGame, 1, nullptr);
	BINDF("SetFullScreen", SQSetFullScreen, 2, ".b");
	BINDF("GetFullScreen", SQGetFullScreen, 1, nullptr);
	BINDF("SetAspectRatio", SQSetAspect, 2, ".i");
	BINDF("GetAspectRatio", SQGetAspect, 1, nullptr);
	BINDF("SetVolume", SQSetVolume, 2, ".f|i");
	BINDF("GetVolume", SQGetVolume, 1, nullptr);

	sq_newslot(vm, -3, SQFalse);

	BINDF("randint", SQRandInt, 3, ".ii");
	sq_settop(vm,top); //restores the original stack size
}
