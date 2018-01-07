#include "../Squirrel/squirrel.h"
#include "SQForwardDeclares.h"
#include "../Core/BinaryFile.h"

namespace
{
	SQInteger SQFileDestruct(SQUserPointer data, SQInteger)
	{
		delete reinterpret_cast<BinaryFile*>(data);
		return 0;
	}

	SQInteger SQFileConstruct(HSQUIRRELVM v)
	{
		sq_setreleasehook(v, -1, SQFileDestruct);
		BinaryFile *stream = new BinaryFile();
		sq_setinstanceup(v, -2, stream);
		return 0;
	}

	SQInteger SQFileWriteInt(HSQUIRRELVM v)
	{
		BinaryFile *stream = nullptr;
		SQInteger number;

		sq_getinstanceup(v, 1, (SQUserPointer*)&stream, nullptr);
		sq_getinteger(v, 2, &number);

		stream->Write(int32_t(number));
		return 0;
	}

	SQInteger SQFileReadInt(HSQUIRRELVM v)
	{
		BinaryFile *stream = nullptr;
		int32_t number;

		sq_getinstanceup(v, 1, (SQUserPointer*)&stream, nullptr);
		stream->Read(number);

		sq_pushinteger(v, number);
		return 1;
	}

	SQInteger SQFileWriteString(HSQUIRRELVM v)
	{
		BinaryFile *stream = nullptr;
		SQChar const *string = nullptr;

		sq_getinstanceup(v, 1, (SQUserPointer*)&stream, nullptr);
		sq_getstring(v, 2, &string);

		stream->Write(std::string(string));
		return 0;
	}

	SQInteger SQFileReadString(HSQUIRRELVM v)
	{
		BinaryFile *stream = nullptr;
		std::string string;

		sq_getinstanceup(v, 1, (SQUserPointer*)&stream, nullptr);
		stream->Read(string);

		sq_pushstring(v, string.c_str(), -1);
		return 1;
	}

	SQInteger SQFileLoad(HSQUIRRELVM v)
	{
		BinaryFile *stream = nullptr;
		SQChar const *name = nullptr;

		sq_getinstanceup(v, 1, (SQUserPointer*)&stream, nullptr);
		sq_getstring(v, 2, &name);

		SQBool good = stream->Load(std::string(name)) ? SQTrue : SQFalse;

		sq_pushbool(v, good);
		return 1;
	}

	SQInteger SQFileSave(HSQUIRRELVM v)
	{
		BinaryFile *stream = nullptr;
		SQChar const *name = nullptr;

		sq_getinstanceup(v, 1, (SQUserPointer*)&stream, nullptr);
		sq_getstring(v, 2, &name);

		SQBool good = stream->Save(std::string(name)) ? SQTrue : SQFalse;

		sq_pushbool(v, good);
		return 1;
	}
}

#define BINDF(__NAME, __FUNC, __PNUM, __PTYPE) sq_pushstring(vm, _SC(__NAME), -1); sq_newclosure(vm, __FUNC, 0); sq_setparamscheck(vm, __PNUM, __PTYPE); sq_setnativeclosurename(vm, -1, _SC(__NAME)); sq_newslot(vm, -3, SQFalse);

void AddFile(HSQUIRRELVM vm)
{
	SQInteger top = sq_gettop(vm); //saves the stack size before the call

	sq_pushstring(vm, _SC("SaveFile"), -1);
	sq_newclass(vm, SQFalse);
	BINDF("constructor", SQFileConstruct, 1, nullptr);
	BINDF("WriteInt", SQFileWriteInt, 2, "xi");
	BINDF("ReadInt", SQFileReadInt, 1, "x");
	BINDF("WriteString", SQFileWriteString, 2, "xs");
	BINDF("ReadString", SQFileReadString, 1, "x");
	BINDF("Load", SQFileLoad, 2, "xs");
	BINDF("Save", SQFileSave, 2, "xs");
	sq_newslot(vm, -3, SQFalse);

	sq_settop(vm,top); //restores the original stack size
}
