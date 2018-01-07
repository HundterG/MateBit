#include "Globals.h"
#include "GSM.h"
#include "../Squirrel/squirrel.h"
#include "../Squirrel/sqstdmath.h"
#include "../Squirrel/sqstdstring.h"
#include "../Squirrel/sqstdaux.h"
#include "../Squirrel/sqstdio.h"
#include "../SQConnect/SQForwardDeclares.h"
#include <cstdarg>

#ifdef SQUNICODE
#define scvprintf vfwprintf
#else
#define scvprintf vfprintf
#endif

namespace
{
	HSQUIRRELVM sqvm;
	Screen back;
	bool funcExist = true;

	void PrintFunc(HSQUIRRELVM v,const SQChar *s,...)
	{
	    va_list vl;
	    va_start(vl, s);
	    scvprintf(stdout, s, vl);
	    va_end(vl);
	}
	
	void ErrorFunc(HSQUIRRELVM v,const SQChar *s,...)
	{
	    va_list vl;
	    va_start(vl, s);
	    scvprintf(stderr, s, vl);
	    va_end(vl);
	}

	void DoFile(std::string &file)
	{
		softcheck(SQ_FAILED(sqstd_dofile(sqvm, file.c_str(), SQFalse, SQTrue)), "There was an error compiling",);
	}
}

void GSMInit(void)
{
	back.Clear();

	sqvm = sq_open(1024);
	hardcheck(sqvm==nullptr, "Squirrel could not find it's nuts.");
	sq_setprintfunc(sqvm, PrintFunc, ErrorFunc);
	sqstd_seterrorhandlers(sqvm);
	sq_pushroottable(sqvm);
	sqstd_register_mathlib(sqvm);
	sqstd_register_stringlib(sqvm);
	AddInput(sqvm);
	AddGraphics(sqvm);
	AddGlobals(sqvm);
	AddFile(sqvm);
	AddAudio(sqvm);
	DirScan("Script", "nut", DoFile);
	sq_pop(sqvm,1);
}

void GSMUpdate(void)
{
	SQInteger top = sq_gettop(sqvm); //saves the stack size before the call
	sq_pushroottable(sqvm);
	if(funcExist)
	{
		sq_pushstring(sqvm, _SC("Screen"), -1);
		if(SQ_SUCCEEDED(sq_get(sqvm, 1)))
		{
			sq_pushstring(sqvm, _SC("tick"), -1);
			if(SQ_SUCCEEDED(sq_get(sqvm, 1)))
			{
				sq_pushroottable(sqvm);
				// push the backbuffer
				sq_createinstance(sqvm, 2);
				sq_setinstanceup(sqvm, -1, &back);

				if(SQ_FAILED(sq_call(sqvm,2,SQFalse,SQTrue)))
				{
					message("there was an error running the script.");
					funcExist = false;
				}
			}
			else
			{
				message("There is no tick function.");
				funcExist = false;
			}
		}
	}
	sq_settop(sqvm,top); //restores the original stack size
	back.Present();
}

void GSMExit(void)
{
	sq_close(sqvm);
}
