#include <stage0.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <ctime>

using namespace std;
//Constructor
//-----------------------------------------------------------------------------------------------------------------------------------
Compiler::Compiler(char **argv) // constructor
{
	//open sourceFile using argv[1]
	//open listingFile using argv[2]
	//open objectFile using argv[3]
	
	sourceFile.open(argv[1]);
	if(!sourceFile)
	{
		exit(EXIT_FAILURE);
	}
	listingFile.open(argv[2]);
	if(!listingFile)
	{
		exit(EXIT_FAILURE);
	}
	objectFile.open(argv[3]);
	if(!objectFile)
	{
		exit(EXIT_FAILURE);
	}
}


//Deconstructor
//-----------------------------------------------------------------------------------------------------------------------------------
Compiler::~Compiler() // destructor
{
	//close all open files
	
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

//CreateListingHeader
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::createListingHeader()
{
	//print "STAGE0:", name(s), DATE, TIME OF DAY
	//print "LINE NO:", "SOURCE STATEMENT"
	//line numbers and source statements should be aligned under the headings
	cout << "STAGE0:	LOGAN WONGBANGCHUAD		";
	///cout << getTime() << endl;
	cout << "LINE NO.              SOURCE STATEMENT" << endl;
	
}

//Parser
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::parser()
{
	ch = nextChar();
	//ch must be initialized to the first character of the source file
	if (nextToken() != "program")
	processError("program");
	//a call to nextToken() has two effects
	// (1) the variable, token, is assigned the value of the next token
	// (2) the next token is read from the source file in order to make
	// the assignment. The value returned by nextToken() is also
	// the next token.
	prog();
	//parser implements the grammar rules, calling first rule
}

//createListingTrailer
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::createListingTrailer()
{
	cout << "COMPILATION TERMINATED		"<< errorCount << " ERRORS ENCOUNTERED";
}

//processError
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::processError(string err)
{
	listingFile << err;
	//Output err to listingFile
	exit(EXIT_FAILURE);
}

//nextChar
//-----------------------------------------------------------------------------------------------------------------------------------
char Compiler::nextChar()
{
	sourceFile.get(ch);
	static char prevChar = '\n';
	
	if(sourceFile.eof())
	{
		ch = END_OF_FILE;
	}
	else
	{
		if(prevChar == '\n')
		{
			++lineNo;
			listingFile << setw(5) << lineNo << '|';
		}
		listingFile << ch;
	}
	prevChar = ch;
	return ch;
}
//nextToken
//-----------------------------------------------------------------------------------------------------------------------------------
string Compiler::nextToken()
{
	token = "";
	while (token == "")
	{
		char ch = nextChar();
		if(ch == '{')
		{
			char temp = nextChar();
			while(temp != END_OF_FILE || temp != '}')
			{
				temp = nextChar();
				 //keep looping
			}
			if(ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
			else
			{
				nextChar();
			}
		}
		else if(ch == '}')
		{
			processError("'}' cannot begin token");
		}
		else if(isspace(ch))
		{
			nextChar();
		}
		else if(isSpecialSymbol(ch))
		{
			token = ch;
			nextChar();
		}
		else if(islower(ch))
		{
			token = ch;
			ch = nextChar();
			while((isalpha(ch) || isdigit(ch) || ch == '_') && (ch != END_OF_FILE))
			{
				token+=ch;
				ch = nextChar();
			}
			if(ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
		}
		else if(isdigit(ch))
		{
			token = ch;
			ch = nextChar();
			while(isdigit(ch) && ch != END_OF_FILE)
			{
				token+=ch;
				ch = nextChar();
			}
			if(ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
		}
		else if(ch == END_OF_FILE)
		{
			token = ch;
		}
		else
		{
			processError("illegal symbol");
		}
	}
 return token;
}

//isKeyword
//-----------------------------------------------------------------------------------------------------------------------------------
bool Compiler::isKeyword(string s) const  // determines if s is a keyword
{
	if(s == "program" || s == "const" || s == "var" || s == "integer" || s == "boolean" || s == "begin" || s == "end" || s == "true" || s == "false" || s == "not")
	   {
		   return true;
	   }
	return false;
}

//isSpecialSymbol
//-----------------------------------------------------------------------------------------------------------------------------------
bool Compiler::isSpecialSymbol(char c) const // determines if c is a special symbol
{
	if(c == '=' || c == ':' || c == ',' || c == ';' || c == '.' || c == '+' || c == '-')
	   {
		   return true;
	   }
	return false;
}

//isNonKeyId
//-----------------------------------------------------------------------------------------------------------------------------------
bool Compiler::isNonKeyId(string s) const // determines if s is a non_key_id
{

    if (isalpha(s[0])) 
	{
        return true;
    }
	
	if(isInteger(s))
	{
		return true;
	}
	if(s[0] == '_')
	{
		return true;
	}
    return false;
}

//isInteger
//-----------------------------------------------------------------------------------------------------------------------------------
bool Compiler::isInteger(string s) const  // determines if s is an integer
{
	
	if(isdigit(stoi(s)))
	{
		return true;
	}
	return false;
}

//isBoolean
//-----------------------------------------------------------------------------------------------------------------------------------
bool Compiler::isBoolean(string s) const  // determines if s is a boolean
{
	
	if(s == "true" || s == "false")
	{
		return true;
	}
	return false;
}

//isLiteral
//-----------------------------------------------------------------------------------------------------------------------------------
bool Compiler::isLiteral(string s) const  // determines if s is a literal
{
	
	if(isInteger(s) || isBoolean(s) || s == "not" || s == "+" || s == "-")
	{
		return true;
	}
	return false;
}

//genInternalName
//-----------------------------------------------------------------------------------------------------------------------------------
string Compiler::genInternalName(storeTypes stype) const
{
	return "";
}

// Action routines
//Insert
//--------------------------------------------------------------------------------------------------------------------------
void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
/*
	string name
	while (name broken from list of external names and put into name != "")
	{
		if (symbolTable[name] is defined)
			processError(multiple name definition)
		else if (name is a keyword)
			processError(illegal use of keyword)
		else //create table entry
		{
		if (name begins with uppercase)
			symbolTable[name]=(name,inType,inMode,inValue,inAlloc,inUnits)
		else
			symbolTable[name]=(genInternalName(inType),inType,inMode,inValue,inAlloc,inUnits)
	}
*/
}

//whichType
//--------------------------------------------------------------------------------------------------------------------------

storeTypes Compiler::whichType(string name) // tells which data type a name has
{
	/*
	string dataType;
	if (isLiteral(name))
		if (isLiteral(name) && isBoolean(name))
			dataType = BOOLEAN;
		else
			dataType = INTEGER;
	else //name is an identifier and hopefully a constant
		if (symbolTable[name] is defined)
			dataType = type of symbolTable[name];
		else
			processError("reference to undefined constant");
	return dataType;
	*/
}

//whichValue
//--------------------------------------------------------------------------------------------------------------------------

string Compiler::whichValue(string name) // tells which value a name has
{
	/*
	string value;
	if (isLiteral(name))
		value = name;
	else //name is an identifier and hopefully a constant
		if (symbolTable[name] is defined and has a value)
			value = value of symbolTable[name];
		else
			processError("reference to undefined constant");
	return value;
	*/
}


//code
//--------------------------------------------------------------------------------------------------------------------------

void Compiler::code(string op, string operand1 = "", string operand2 = "")
{
	/*
	if (op == "program")
		emitPrologue(operand1)
	else if (op == "end")
		emitEpilogue()
	else
		processError("compiler error since function code should not be called with illegal arguments")
	*/
}
//-----------------------------------------------------------------------------------------------------------------------------------

// Emit Functions
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::emit(string label = "", string instruction = "", string operands = "", string comment = "")
{    
/*
	Turn on left justification in objectFile
	Output label in a field of width 8
	Output instruction in a field of width 8
	Output the operands in a field of width 24
	Output the comment
*/
}

void Compiler::emitPrologue(string progName, string = "")
{   
/*
	Output identifying comments at beginning of objectFile
	Output the %INCLUDE directives
	emit("SECTION", ".text")
	emit("global", "_start", "", "; program " + progName)
	emit("_start:")
 */
}

void Compiler::emitEpilogue(string = "", string = "")
{
	/*
	emit("","Exit", "{0}")
	emitStorage();
	*/
}

void Compiler::emitStorage()
{
	/*
	emit("SECTION", ".data")
	for those entries in the symbolTable that have
	an allocation of YES and a storage mode of CONSTANT
	{ call emit to output a line to objectFile }
	emit("SECTION", ".bss")
	for those entries in the symbolTable that have
	an allocation of YES and a storage mode of VARIABLE
	{ call emit to output a line to objectFile }
	*/
}

//-----------------------------------------------------------------------------------------------------------------------------------
//Production 1
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::prog() //token should be "program"
{
	string token = nextToken();
	if (token != "program")
		processError("keyword \"program\" expected");
	progStmt();
	if (token == "const")
		consts();
	if (token == "var")
		vars();
	if (token != "begin")
		processError("keyword \"begin\" expected");
	beginEndStmt();
	if (token != "$")
		processError("no text may follow \"end\"");
}

//Production 2
//------------------------------------------------------------------------------------------------------------------
void Compiler::progStmt() //token should be "program"
{
	string x;
	if (nextToken() != "program")
		processError("keyword \"program\" expected");
	x = nextToken();
	if (!isNonKeyId(x))
		processError("program name expected");
	if (nextToken() != ";")
		processError("semicolon expected");
	nextToken();
	code("program", x);
	insert(x,PROG_NAME,CONSTANT,x,NO,0);
}

//Production 3
//------------------------------------------------------------------------------------------------------------------
void Compiler::consts() //token should be "const"
{
	if (nextToken() != "const")
		processError("keyword \"const\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"const\"");
	constStmts();
}

//Production 4
//------------------------------------------------------------------------------------------------------------------
void Compiler::vars() //token should be "var"
{
	if (token != "var")
		processError("keyword \"var\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"var\"");
	varStmts();
}


//Production 5
//------------------------------------------------------------------------------------------------------------------
void Compiler::beginEndStmt() //token should be "begin"
{
	if (token != "begin")
		processError("keyword \"begin\" expected");
	if (nextToken() != "end")
		processError("keyword \"end\" expected");
	if (nextToken() != ".")
		processError("period expected");
	nextToken();
	code("end", ".");
}

//Production 6
//------------------------------------------------------------------------------------------------------------------
void Compiler::constStmts() //token should be NON_KEY_ID
{
	string x,y;
	token = nextToken();
	if (!isNonKeyId(token()))
		processError("non-keyword identifier expected");
	x = token;
	if (nextToken() != "=")
		processError("\"=\" expected");
	y = nextToken();
	if (y != "+" || y != "-" || y != "not" || !isNonKeyId(y) || y != "true" || y != "false" || !isInteger(y))
		processError("token to right of \"=\" illegal");
	if (y == "+" || y == "-")
	{
		if (!isInteger(nextToken()))
		processError("integer expected after sign");
		y = y + token;
	}
	if (y == "not")
	{
		if (!isBoolean(nextToken()))
			processError("boolean expected after \"not\"");
		if (token == "true")
			y = "false";
		else
			y = "true";
	}
	if (nextToken() != ";")
		processError("semicolon expected");
	if (whichType(y) != "INTEGER" || whichType(y) != "BOOLEAN" )
		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");
	insert(x,whichType(y),CONSTANT,whichValue(y),YES,1);
	x = nextToken();
	if (x != "begin" || x != "var" || !isNonKeyId(x))
		processError("non-keyword identifier, \"begin\", or \"var\" expected");
	if (isNonKeyId(x))
		constStmts();
}


//Production 7
//------------------------------------------------------------------------------------------------------------------

void Compiler::varStmts() //token should be NON_KEY_ID
{
	string x,y;
	string token = nextToken();
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected");
	x = ids();
	if (token != ":")
		processError("\":\" expected");
	string tok = nextToken();
	if (tok != "integer" || tok != "boolean")
		processError("illegal type follows \":\"");
	y = token;
	if (nextToken() != ";")
		processError("semicolon expected");
	insert(x,y,VARIABLE,"",YES,1);
	string tok2 = nextToken();
	if (tok2 != "begin" || !isNonKeyId(tok2))
		processError("non-keyword identifier or \"begin\" expected");
	if (isNonKeyId(tok2))
		varStmts();
}


//Production 8
//------------------------------------------------------------------------------------------------------------------

string Compiler::ids() //token should be NON_KEY_ID
{
	string temp,tempString;
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected");
	tempString = token;
	temp = token;
	if (nextToken() == ",")
	{
		if (!isNonKeyId(nextToken()))
			processError("non-keyword identifier expected");
		tempString = temp + "," + ids();
	}
	return tempString;
} 