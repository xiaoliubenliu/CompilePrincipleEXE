
#ifndef TOKENTYPE
#define TOKENTYPE
static enum ETokenType {
	//��ֵ�͡�
	_INT,
	_FLOAT,
	_STRING,
	_BOOL,
	//��֧ѭ��
	_COMP, //�Ƚ�
	_IF,
	_ELSE,
	_WHILE,
	_BREAK,

	//�Ӽ��˳�����
	_PLUS,
	_MINUS,
	_MUL,
	_DIV,
	_POW,
	_AND,
	_OR,
	_NOT,
	_MODULE,

	_FN, //����
	_LPAREN, //������
	_RPAREN, //������
	_LCB, //�������
	_RCB,  //�Ҵ�����
	_LSB,  //��������
	_RSB,   //��������
	_KW,   //�ؼ���
	_DL,
	_ID,	//����id
	_RTVALUE,

	_VOID,
	_RETURN,
	_CONTINUE,
	_ASSIGN, //��ֵ
	_PRINT,  //���ô�ӡ���
	_READINT,   //��ȡint
	_READFLOAT,   //��ȡfloat
	_READSTRING,   //��ȡstring
	_READBOOL,   //��ȡbool
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