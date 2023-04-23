
#ifndef TOKENTYPE
#define TOKENTYPE
static enum ETokenType {
	//数值型。
	_INT,
	_FLOAT,
	_STRING,
	_BOOL,
	//分支循环
	_COMP, //比较
	_IF,
	_ELSE,
	_WHILE,
	_BREAK,

	//加减乘除与或非
	_PLUS,
	_MINUS,
	_MUL,
	_DIV,
	_POW,
	_AND,
	_OR,
	_NOT,
	_MODULE,

	_FN, //函数
	_LPAREN, //左括号
	_RPAREN, //右括号
	_LCB, //左大括号
	_RCB,  //右大括号
	_LSB,  //左中括号
	_RSB,   //右中括号
	_KW,   //关键字
	_DL,
	_ID,	//变量id
	_RTVALUE,

	_VOID,
	_RETURN,
	_CONTINUE,
	_ASSIGN, //赋值
	_PRINT,  //内置打印语句
	_READINT,   //读取int
	_READFLOAT,   //读取float
	_READSTRING,   //读取string
	_READBOOL,   //读取bool
	_EOF      //end of file
};
static const char* token_names[39] = {
			"int",
			"float",
			"string",
			"bool",
			"comp",

			"if",
			"else",
			"while",
			"break",
			"+",
			"-",
			"*",
			"/",
			"^",
			"&",
			"|",
			"!",
			"module",

			"fn",
			"(",
			")",
			"{",
			"}",
			"[",
			"]",

			"kw",
			";",
			"id",
			"retValue",

			"void",
			"return",
			"continue",
			"=",
			"print",
			"readInt",
			"readFloat",
			"readString",
			"readBool",
			"EOF"
};
static enum ReadNodeType
{
	READINT,
	READFLOAT,
	READSTRING,
	READBOOL
};
#endif