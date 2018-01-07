#include "../Squirrel/squirrel.h"
#include "SQForwardDeclares.h"
#include "../Core/Input.h"

namespace
{
	SQInteger SQPressed(HSQUIRRELVM v)
	{
		SQInteger n;
		sq_getinteger(v,2,&n);
		if(0 <= n && n < 8)
			sq_pushbool(v, InputIsPressed(InputType::IT(n)));
		else
			sq_pushbool(v, SQFalse);
		return 1;
	}

	SQInteger SQHeld(HSQUIRRELVM v)
	{
		SQInteger n;
		sq_getinteger(v,2,&n);
		if(0 <= n && n < 8)
			sq_pushbool(v, InputIsHeld(InputType::IT(n)));
		else
			sq_pushbool(v, SQFalse);
		return 1;
	}

	SQInteger SQLeftClick(HSQUIRRELVM v)
	{
		sq_pushbool(v, InputIsLeftClick());
		return 1;
	}

	SQInteger SQLeftHeld(HSQUIRRELVM v)
	{
		sq_pushbool(v, InputIsLeftHeld());
		return 1;
	}

	SQInteger SQRightClick(HSQUIRRELVM v)
	{
		sq_pushbool(v, InputIsRightClick());
		return 1;
	}

	SQInteger SQRightHeld(HSQUIRRELVM v)
	{
		sq_pushbool(v, InputIsRightHeld());
		return 1;
	}

	SQInteger SQMouseX(HSQUIRRELVM v)
	{
		sq_pushinteger(v, InputGetMouseX());
		return 1;
	}

	SQInteger SQMouseY(HSQUIRRELVM v)
	{
		sq_pushinteger(v, InputGetMouseY());
		return 1;
	}

	SQInteger SQRebind(HSQUIRRELVM v)
	{
		SQInteger n;
		sq_getinteger(v,2,&n);
		if(0 <= n && n < 8)
			InputReBind(InputType::IT(n));
		return 0;
	}
}

#define BINDC(__NAME, __VAL) sq_pushstring(vm, _SC(__NAME), -1); sq_pushinteger(vm, __VAL); sq_newslot(vm, -3, SQTrue);
#define BINDF(__NAME, __FUNC, __PNUM, __PTYPE) sq_pushstring(vm, _SC(__NAME), -1); sq_newclosure(vm, __FUNC, 0); sq_setparamscheck(vm, __PNUM, __PTYPE); sq_setnativeclosurename(vm, -1, _SC(__NAME)); sq_newslot(vm, -3, SQFalse);

void AddInput(HSQUIRRELVM vm)
{
	SQInteger top = sq_gettop(vm); //saves the stack size before the call
	// create a table and put funcs and consts in there
	sq_pushstring(vm, _SC("Input"), -1);
	sq_newclass(vm, SQFalse);

	// constants
	BINDC("A", InputType::A);
	BINDC("B", InputType::B);
	BINDC("Start", InputType::Start);
	BINDC("Select", InputType::Select);
	BINDC("Up", InputType::Up);
	BINDC("Down", InputType::Down);
	BINDC("Left", InputType::Left);
	BINDC("Right", InputType::Right);

	// functions
	BINDF("isPressed", SQPressed, 2, ".i");
	BINDF("isHeld", SQHeld, 2, ".i");
	BINDF("isLeftClick", SQLeftClick, 1, nullptr);
	BINDF("isLeftHeld", SQLeftHeld, 1, nullptr);
	BINDF("isRightClick", SQRightClick, 1, nullptr);
	BINDF("isRightHeld", SQRightHeld, 1, nullptr);
	BINDF("GetMouseX", SQMouseX, 1, nullptr);
	BINDF("GetMouseY", SQMouseY, 1, nullptr);
	BINDF("SetRebind", SQRebind, 2, ".i");
	
	sq_newslot(vm, -3, SQFalse);
	sq_settop(vm,top); //restores the original stack size
}
