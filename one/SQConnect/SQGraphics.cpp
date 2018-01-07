#include "../Squirrel/squirrel.h"
#include "SQForwardDeclares.h"
#include "../Core/Globals.h"
#include "../Core/Graphics.h"
#include "../Loaders/Loaders.h"

namespace
{
	HSQMEMBERHANDLE colorArgs[3];
	HSQMEMBERHANDLE transformArgs[4];

	SQInteger SQScreenDestruct(SQUserPointer data, SQInteger)
	{
		delete reinterpret_cast<Screen*>(data);
		return 0;
	}

	SQInteger SQScreenConstruct(HSQUIRRELVM v)
	{
		sq_setreleasehook(v, -1, SQScreenDestruct);
		Screen *screen = new Screen();
		sq_setinstanceup(v, -2, screen);
		return 0;
	}

	SQInteger SQClear(HSQUIRRELVM v)
	{
		SQInteger top = sq_gettop(v);
		Screen *screen = nullptr;
		SQInteger color = 0;

		sq_getinstanceup(v, 1, (SQUserPointer*)&screen, nullptr);
		if(top > 1) sq_getinteger(v, 2, &color);
		
		screen->Clear((unsigned char)(color));
		return 0;
	}

	SQInteger SQWarpSet(HSQUIRRELVM v)
	{
		Screen *screen = nullptr;
		SQInteger index = -1;
		SQInteger offset = 0;

		sq_getinstanceup(v, 1, (SQUserPointer*)&screen, nullptr);
		sq_getinteger(v, 2, &index);
		sq_getinteger(v, 3, &offset);
		
		screen->SetWarp(int(index), char(offset));
		return 0;
	}

	SQInteger SQResetWarp(HSQUIRRELVM v)
	{
		Screen *screen = nullptr;

		sq_getinstanceup(v, 1, (SQUserPointer*)&screen, nullptr);
		
		screen->SetWarp(ScreenWarp());
		return 0;
	}

	struct DrawSettingsHolder
	{
		Pallet *pallet = nullptr;
		DrawSettings settings;
		bool useSettings = false;
	};

	void ParseSettings(HSQUIRRELVM v, DrawSettingsHolder &settings, int i)
	{
		if(sq_gettype(v, i) == SQObjectType::OT_INTEGER)
		{
			SQInteger trans = -1;
			sq_getinteger(v, i, &trans);
			settings.settings.transparency = trans;
		}
		else if(sq_gettype(v, i) == SQObjectType::OT_INSTANCE)
		{
			TypeID id = nullptr;
			sq_gettypetag(v, i, (SQUserPointer*)&id);
			if(id == GetTypeID<DrawSettings::Fade>())
			{
#define GETI(__VAR, __VARHAND) { SQInteger get=0; sq_getbyhandle(v, i, &__VARHAND); if(sq_gettype(v, -1) == SQObjectType::OT_INTEGER) sq_getinteger(v, -1, &get); sq_poptop(v); __VAR = DrawSettings::Fade(get); }
				GETI(settings.settings.red, colorArgs[0]);
				GETI(settings.settings.green, colorArgs[1]);
				GETI(settings.settings.blue, colorArgs[2]);
#undef GETI
				settings.useSettings = true;
			}
			else if(id == GetTypeID<DrawSettings>())
			{
#define GETB(__VAR, __VARHAND) { sq_getbyhandle(v, i, &__VARHAND); switch(sq_gettype(v, -1)) { case SQObjectType::OT_BOOL: { SQBool get=false; sq_getbool(v, -1, &get); __VAR = bool(get); break; } case SQObjectType::OT_INTEGER: {SQInteger get=0; sq_getinteger(v, -1, &get); __VAR = bool(get); break; } } sq_poptop(v); }
				GETB(settings.settings.flipX, transformArgs[0]);
				GETB(settings.settings.flipY, transformArgs[1]);
				GETB(settings.settings.rotateCW, transformArgs[2]);
				GETB(settings.settings.rotateCCW, transformArgs[3]);
#undef GETB
				settings.useSettings = true;
			}
			else if(id == GetTypeID<Pallet>())
				sq_getinstanceup(v, i, (SQUserPointer*)&settings.pallet, nullptr);
		}
	}

	SQInteger SQDrawBlock(HSQUIRRELVM v)
	{
		SQInteger top = sq_gettop(v);
		Screen *screen = nullptr;
		SQInteger x, y;
		SQChar const *name = nullptr;
		SQInteger frame;
		DrawSettingsHolder settings;

		sq_getinstanceup(v, 1, (SQUserPointer*)&screen, nullptr);
		sq_getstring(v, 2, &name);
		sq_getinteger(v, 3, &frame);
		sq_getinteger(v, 4, &x);
		sq_getinteger(v, 5, &y);

		for(int i=6 ; i<=top ; ++i)
		{
			if(sq_gettype(v, i) == SQObjectType::OT_ARRAY)
			{
				sq_pushnull(v);
				while(SQ_SUCCEEDED(sq_next(v, i)))
				{
					ParseSettings(v, settings, -1);
					sq_pop(v, 2);
				}
				sq_pop(v, 1);
			}
			else
				ParseSettings(v, settings, i);
		}

		if(settings.useSettings)
		{
			if(settings.pallet)
				screen->DrawBlock(GetBlock(std::string(name), frame), x, y, settings.settings, *settings.pallet);
			else
				screen->DrawBlock(GetBlock(std::string(name), frame), x, y, settings.settings);
		}
		else
		{
			if(settings.pallet)
				screen->DrawBlock(GetBlock(std::string(name), frame), x, y, *settings.pallet, settings.settings.transparency);
			else
				screen->DrawBlock(GetBlock(std::string(name), frame), x, y, settings.settings.transparency);
		}

		return 0;
	}

	SQInteger SQDrawSolid(HSQUIRRELVM v)
	{
		Screen *screen = nullptr;
		SQInteger x, y, w, h, color;

		sq_getinstanceup(v, 1, (SQUserPointer*)&screen, nullptr);
		sq_getinteger(v, 2, &x);
		sq_getinteger(v, 3, &y);
		sq_getinteger(v, 4, &w);
		sq_getinteger(v, 5, &h);
		sq_getinteger(v, 6, &color);

		screen->DrawSolid(int(x), int(y), int(w), int(h), unsigned char(color));
		return 0;
	}

	SQInteger SQOverlay(HSQUIRRELVM v)
	{
		SQInteger top = sq_gettop(v);
		Screen *screen = nullptr;
		Screen *other = nullptr;
		SQInteger trans = -1;

		sq_getinstanceup(v, 1, (SQUserPointer*)&screen, nullptr);
		sq_getinstanceup(v, 2, (SQUserPointer*)&other, nullptr);
		if(top > 2) sq_getinteger(v, 3, &trans);

		screen->OverLay(*other, trans);
		return 0;
	}

	SQInteger SQColorConstructor(HSQUIRRELVM v)
	{
		sq_setbyhandle(v, 1, &colorArgs[2]);
		sq_setbyhandle(v, 1, &colorArgs[1]);
		sq_setbyhandle(v, 1, &colorArgs[0]);
		return 0;
	}

	SQInteger SQTransformConstructor(HSQUIRRELVM v)
	{
		sq_setbyhandle(v, 1, &transformArgs[3]);
		sq_setbyhandle(v, 1, &transformArgs[2]);
		sq_setbyhandle(v, 1, &transformArgs[1]);
		sq_setbyhandle(v, 1, &transformArgs[0]);
		return 0;
	}

	SQInteger SQPalletDestruct(SQUserPointer data, SQInteger)
	{
		delete reinterpret_cast<Pallet*>(data);
		return 0;
	}
	
	SQInteger SQPalletConstructor(HSQUIRRELVM v)
	{
		sq_setreleasehook(v, -1, SQPalletDestruct);
		Pallet *pallet = new Pallet();
		for(int i=0 ; i<256 ; ++i)
			pallet->color[i] = char(i);
		sq_setinstanceup(v, -2, pallet);
		return 0;
	}

	SQInteger SQPalletSet(HSQUIRRELVM v)
	{
		Pallet *pallet = nullptr;
		SQInteger index = 0;
		SQInteger color = 0;

		sq_getinstanceup(v, 1, (SQUserPointer*)&pallet, nullptr);
		sq_getinteger(v, 2, &index);
		sq_getinteger(v, 3, &color);
		if(0 <= index && index < 256)
			pallet->color[index] = char(color);

		return 0;
	}

	SQInteger SQPalletReset(HSQUIRRELVM v)
	{
		Pallet *pallet = nullptr;

		sq_getinstanceup(v, 1, (SQUserPointer*)&pallet, nullptr);
		for(int i=0 ; i<256 ; ++i)
			pallet->color[i] = char(i);

		return 0;
	}
}

#define BINDV(__NAME, __VAL, __CONST) sq_pushstring(vm, _SC(__NAME), -1); sq_pushinteger(vm, __VAL); sq_newslot(vm, -3, __CONST);
#define GETHANDLE(__NAME, __VAR) sq_pushstring(vm, _SC(__NAME), -1); sq_getmemberhandle(vm, -2, &__VAR);
#define BINDF(__NAME, __FUNC, __PNUM, __PTYPE) sq_pushstring(vm, _SC(__NAME), -1); sq_newclosure(vm, __FUNC, 0); sq_setparamscheck(vm, __PNUM, __PTYPE); sq_setnativeclosurename(vm, -1, _SC(__NAME)); sq_newslot(vm, -3, SQFalse);

void AddGraphics(HSQUIRRELVM vm)
{
	SQInteger top = sq_gettop(vm); //saves the stack size before the call

	// Global Functions
	sq_pushstring(vm, _SC("Screen"), -1);
	sq_newclass(vm, SQFalse);
	BINDF("Clear", SQClear, -1, "xi");
	BINDF("constructor", SQScreenConstruct, 1, nullptr);
	BINDF("Draw", SQDrawBlock, -5, "xsiii");
	BINDF("DrawSolid", SQDrawSolid, 6, "xiiiii");
	BINDF("Overlay", SQOverlay, -2, "xxi");
	BINDF("SetWarp", SQWarpSet, 3, "xii");
	BINDF("ResetWarp", SQResetWarp, 1, "x");
	sq_settypetag(vm, -1, GetTypeID<Screen>());
	sq_newslot(vm, -3, SQFalse);

	// Draw Helpers
	sq_pushstring(vm, _SC("ColorFade"), -1);
	sq_newclass(vm, SQFalse);
	BINDV("x0", DrawSettings::x0, true);
	BINDV("x33", DrawSettings::x33, true);
	BINDV("x66", DrawSettings::x66, true);
	BINDV("x100", DrawSettings::x100, true);
	BINDV("x133", DrawSettings::x133, true);
	BINDV("x166", DrawSettings::x166, true);
	BINDV("x200", DrawSettings::x200, true);
	BINDV("red", DrawSettings::x100, false);
	BINDV("green", DrawSettings::x100, false);
	BINDV("blue", DrawSettings::x100, false);
	GETHANDLE("red", colorArgs[0]);
	GETHANDLE("green", colorArgs[1]);
	GETHANDLE("blue", colorArgs[2]);
	BINDF("constructor", SQColorConstructor, 4, ".iii");
	sq_settypetag(vm, -1, GetTypeID<DrawSettings::Fade>());
	sq_newslot(vm, -3, SQFalse);

	sq_pushstring(vm, _SC("Transform"), -1);
	sq_newclass(vm, SQFalse);
	BINDV("flipx", false, false);
	BINDV("flipy", false, false);
	BINDV("cw", false, false);
	BINDV("ccw", false, false);
	GETHANDLE("flipx", transformArgs[0]);
	GETHANDLE("flipy", transformArgs[1]);
	GETHANDLE("cw", transformArgs[2]);
	GETHANDLE("ccw", transformArgs[3]);
	BINDF("constructor", SQTransformConstructor, 5, ".bbbb");
	sq_settypetag(vm, -1, GetTypeID<DrawSettings>());
	sq_newslot(vm, -3, SQFalse);

	sq_pushstring(vm, _SC("Pallet"), -1);
	sq_newclass(vm, SQFalse);
	BINDF("Set", SQPalletSet, 3, "xii");
	BINDF("Reset", SQPalletReset, 1, "x");
	BINDF("constructor", SQPalletConstructor, 1, nullptr);
	sq_settypetag(vm, -1, GetTypeID<Pallet>());
	sq_newslot(vm, -3, SQFalse);

	sq_settop(vm,top); //restores the original stack size
}
