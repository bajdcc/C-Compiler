#include "StdAfx.h"
#include "Logic.h"

//////////////////////////////////////////////////////////////////////////

namespace Logic_Global
{
	// global vaiables
	static ILogic_OutputMsg* g_IOMsg = NULL;
	static LIST_ARRAY* g_pList = NULL;
	static BOOL g_bError = FALSE;

	// C- Keywords
	static LPTSTR s_apszCKeywordList[] =
	{
		_T("class"),
		_T("public"),
		_T("private"),
		_T("protected"),
		_T("static"),
		_T("namespace"),
		_T("using"),
		_T("var"),
		_T("new"),
		_T("auto"),
		_T("break"),
		_T("case"),
		_T("continue"),
		_T("const"),
		_T("default"),
		_T("delete"),
		_T("do"),
		_T("else"),
		_T("enum"),
		_T("extern"),
		_T("for"),
		_T("goto"),
		_T("if"),
		_T("else"),
		_T("while"),
		_T("register"),
		_T("return"),
		_T("sizeof"),
		_T("struct"),
		_T("switch"),
		_T("case"),
		_T("typedef"),
		_T("union"),
		_T("volatile"),
		_T("mutable"),
		_T("true"),
		_T("false"),

		NULL
	};

	// C- Types
	static LPTSTR s_apszCTypeList[] =
	{
		_T("signed"),
		_T("unsigned"),
		_T("char"),
		_T("bool"),
		_T("short"),
		_T("int"),
		_T("long"),
		_T("float"),
		_T("double"),
		_T("void"),

		NULL
	};

	// Token analysis
	static LPTSTR s_apszCReservedKeywords[] =
	{
		// keywords
		_T("extern"), _T("static"), _T("sizeof"),
		_T("auto"), _T("const"), _T("typedef"), _T("default"),
		_T("struct"), _T("enum"), _T("class"), _T("union"),
		_T("signed"), _T("unsigned"),
		_T("void"),
		_T("char"), _T("int"), _T("double"), _T("float"), _T("long"),
		_T("volatile"), _T("mutable"), _T("register"),
		_T("if"), _T("else"), _T("for"), _T("break"), _T("while"), _T("do"), _T("continue"), _T("switch"), _T("case"), _T("goto"),
		_T("return"),

		// functions


		// operations
		_T("="), _T("+"), _T("-"), _T("*"), _T("/"), _T("%"), _T("&"), _T("|"), _T("~"), _T("!"), _T("<"), _T(">"),

		// interpunctions
		_T("("), _T(")"), _T("{"), _T("}"), _T("["), _T("]"), _T(","), _T("."), _T(";"), _T(":"),

		NULL
	};
}

//////////////////////////////////////////////////////////////////////////

void Logic_SetInterface( ILogic_OutputMsg* pInterface )
{
	ASSERT(pInterface);
	Logic_Global::g_IOMsg = pInterface;
}

ILogic_OutputMsg* Logic_GetInterface()
{
	return Logic_Global::g_IOMsg;
}

void Logic_SetDataPtr( LIST_ARRAY* pInterface )
{
	Logic_Global::g_pList = pInterface;
}

LIST_ARRAY* Logic_GetDataPtr()
{
	return Logic_Global::g_pList;
}

void Logic_OutputMsg( TCHAR* format, ... )
{
	if ( Logic_Global::g_IOMsg == NULL )
		return;

	CString output;

	va_list params;
	va_start( params, format );
	output.FormatV( format, params );
	va_end( params );

	Logic_SetErrorFlag(TRUE);

	Logic_Global::g_IOMsg->Output(output);
}

void Logic_OutputMsg( UINT id, ... )
{
	if ( Logic_Global::g_IOMsg == NULL )
		return;

	CString output;
	CString loads;

	BOOL bSuccess = loads.LoadString(id);
	ASSERT(bSuccess);

	va_list params;
	va_start( params, id );
	output.FormatV( loads.GetBuffer(), params );
	va_end( params );

	Logic_SetErrorFlag(TRUE);

	Logic_Global::g_IOMsg->Output(output);
}

void Logic_TraceMsg( TCHAR* format, ... )
{
	if ( Logic_Global::g_IOMsg == NULL )
		return;

	CString output;

	va_list params;
	va_start( params, format );
	output.FormatV( format, params );
	va_end( params );

	Logic_Global::g_IOMsg->Trace(output);
}

void Logic_TraceMsg( UINT id, ... )
{
	if ( Logic_Global::g_IOMsg == NULL )
		return;

	CString output;
	CString loads;

	BOOL bSuccess = loads.LoadString(id);
	ASSERT(bSuccess);

	va_list params;
	va_start( params, id );
	output.FormatV( loads.GetBuffer(), params );
	va_end( params );

	Logic_Global::g_IOMsg->Trace(output);
}

//////////////////////////////////////////////////////////////////////////


LPTSTR* Logic_GetKeywords()
{
	return Logic_Global::s_apszCKeywordList;
}

LPTSTR* Logic_GetTypes()
{
	return Logic_Global::s_apszCTypeList;
}

LPTSTR* Logic_GetRevKeywords()
{
	return Logic_Global::s_apszCReservedKeywords;
}

void Logic_SetErrorFlag( BOOL bErrFlag )
{
	Logic_Global::g_bError = bErrFlag;
}

BOOL Logic_GetErrorFlag()
{
	return Logic_Global::g_bError;
}

