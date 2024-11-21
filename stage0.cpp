#include <stage0.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <cctype>

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

	time_t now = time (NULL);

	//print "STAGE0:", name(s), DATE, TIME OF DAY
	//print "LINE NO:", "SOURCE STATEMENT"
	//line numbers and source statements should be aligned under the headings
	listingFile << "STAGE0:  LOGAN WONGBANGCHUAD	";
	listingFile << ctime(&now) << endl;
	listingFile << "LINE NO.              SOURCE STATEMENT" << endl << endl;

}

//Parser
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::parser()
{

	nextChar();
	//ch must be initialized to the first character of the source file
	if (nextToken() != "program")
		processError("keyword \"program\" expected");
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
	listingFile << endl;
	if(errorCount == 0)
		listingFile << "COMPILATION TERMINATED      "<< errorCount << " ERRORS ENCOUNTERED" << endl;
	else
		listingFile << "COMPILATION TERMINATED      "<< errorCount << " ERROR ENCOUNTERED" << endl;

}

//processError
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::processError(string err)
{
	//cout << err << endl;
	listingFile << endl;
	listingFile << "Error: Line " << lineNo << ": ";
	listingFile << err << endl;
	errorCount++;
	//Output err to listingFile
	createListingTrailer();
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
	//cout << ch << endl << endl;
	return ch;
}
//nextToken
//-----------------------------------------------------------------------------------------------------------------------------------
string Compiler::nextToken()
{
	//cout << "Enter nextToken" << endl;
	token = "";
	while (token == "")
	{
		if(ch == '{')
		{
			char temp = nextChar();
			while(temp != END_OF_FILE && temp != '}')
			{
				//cout << "Enter nextToken comment loop" << endl;
				temp = nextChar();
				//keep looping
			}
			//cout << "Exit nextToken comment loop" << endl;
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
				//cout << "Enter nextToken isDigit loop" << endl;
				token+=ch;
				ch = nextChar();
			}
			//cout << "Exit nextToken isDigit loop" << endl;
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
	
	//cout << "				Token: " << token << endl;
	//cout << "Exit nextToken" << endl;
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
    // A non-key ID must not be empty
    if (s.empty())
        return false;

    // A non-key ID cannot start or end with an underscore
    if (s[0] == '_' || s[s.length() - 1] == '_')
        return false;

    // A non-key ID cannot be a keyword
    if (isKeyword(s))
        return false;

    // A non-key ID must consist only of lowercase letters, digits, or underscores
    // Additionally, it cannot contain consecutive underscores
    for (uint i = 0; i < s.length(); ++i)
    {
        char ch = s[i];

        if (!(islower(ch) || isdigit(ch) || ch == '_')) // Ensure valid characters
            return false;

        //if (ch == '_' && i > 0 && s[i - 1] == '_') // Check for consecutive underscores
		if(i > 0 || i < s.length() - 1)
			if (s[i]== '_' && s[i + 1] == '_') // Check for consecutive underscores
				return false;
    }

    return true; // Passed all checks
}

	/*
	//cout << "Enter isNonKeyId" << endl;
    if (isalpha(s[0])) 
	{
		//cout << "true isNonKeyId" << endl;
        return true;
    }
	
	if(isInteger(s))
	{
		//cout << "true isNonKeyId" << endl;
		return true;
	}
	if(s[0] == '_')
	{
		//cout << "true isNonKeyId" << endl;
		return true;
	}
	//cout << "false isNonKeyId" << endl;
    return false;
	*/

//isInteger
//-----------------------------------------------------------------------------------------------------------------------------------
bool Compiler::isInteger(string s) const  // determines if s is an integer
{
	if (s.empty())
		return false;
	
	if(s[0] == '-' || s[0] == '+')
	{
			for (uint i = 1; i < s.length(); ++i) 
			{
			//cout << "is integer false: " << s[i] << endl;
				if (!isdigit(s[i])) 
					return false;
			}
	}
	else
	{
		for (uint i = 0; i < s.length(); ++i) 
		{
			//cout << "is integer false: " << s[i] << endl;
			if (!isdigit(s[i])) 
				return false;
		}
	}
	
	return true;
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
	string notString = "";
	notString += s[0];
	notString += s[1];
	notString += s[2];
	if(isInteger(s) || isBoolean(s) || (notString == "not" && isBoolean(s)) || (s[0] == '+' && isInteger(s))|| (s[0] == '-' && isInteger(s)) || (s == "integer") || (s == "boolean"))
	{
		//cout << "is literal true" << endl;
		return true;
	}
	//cout << "is literal false: " << s << endl;
	return false;
}

//genInternalName
//-----------------------------------------------------------------------------------------------------------------------------------
string Compiler::genInternalName(storeTypes stype) const
{
	static uint progCt = 0, intCt = 0, boolCt = 0;
	string s = "";
	if(stype == INTEGER)
	{
		s = "I" + to_string(intCt);
		intCt++;
	}
	else if(stype == BOOLEAN)
	{
		s = "B" + to_string(boolCt);
		boolCt++;
	}
	else if(stype == PROG_NAME)
	{
		s = "P" + to_string(progCt);
		progCt++;
	}
	
	return s;
}

// Action routines
//Insert
//--------------------------------------------------------------------------------------------------------------------------
void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	//cout << "Enter insert" << endl;
	if(inValue == "true")
	{
		inValue = "-1";
	}
	else if(inValue == "false")
	{
		inValue = "0";
	}
	//string fifteenChars = externalName.substr(0,15);
	SymbolTableEntry symbolTableEntry(externalName, inType, inMode, inValue, inAlloc, inUnits);
	//SymbolTableEntry elseTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits);


	//Build the first separated by a comma
	//Use .find to find index
	//substring method in string substr
	
	
	//Idea from Dominic Ramirez
	istringstream names(externalName);
	string name;
	
	while (getline(names, name, ','))
	{
		name = name.substr(0, 15);
		if (symbolTable.find(name) != symbolTable.end())
			processError("symbol x is multiply defined");
		else if (isKeyword(name))
			processError("illegal use of keyword");
		else //create table entry
		{
			if (isupper(name[0]))
				//use insert here
				//symbolTable.insert({name, symbolTableEntry})
				symbolTable.insert({name, symbolTableEntry});
				//symbolTable[name]=(name,inType,inMode,inValue,inAlloc,inUnits)
			else
			{
				SymbolTableEntry elseTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits);
				symbolTable.insert({name, elseTableEntry});
				//symbolTable[name]=(genInternalName(inType),inType,inMode,inValue,inAlloc,inUnits)
			}
		}
	}
		//cout << "Exit insert" << endl;

}

//whichType
//--------------------------------------------------------------------------------------------------------------------------

storeTypes Compiler::whichType(string name) // tells which data type a name has
{
	//cout << "Enter whichType" << endl;
	//cout << "name: " << name << endl;
	storeTypes dataType;
	if (isLiteral(name))
	{
		if (isBoolean(name))
		{
			//cout << BOOLEAN << endl;
			return BOOLEAN;
		}
		else
		{
			//cout << INTEGER << endl;
			return INTEGER;
		}
	}
	else //name is an identifier and hopefully a constant
	{
		if (symbolTable.find(name) != symbolTable.end())
			dataType = symbolTable.find(name)->second.getDataType();
		else
			processError("reference to undefined constant");
		return dataType;
	}
	//cout << dataType << endl;
	return dataType;
}

//whichValue
//--------------------------------------------------------------------------------------------------------------------------

string Compiler::whichValue(string name) // tells which value a name has
{
	string value;
	if (isLiteral(name))
		value = name;
	else //name is an identifier and hopefully a constant
		if (symbolTable.find(name) != symbolTable.end() && symbolTable.find(name)->second.getValue().length() != 0 )
			value = symbolTable.find(name)->second.getValue();
		else
		{
			/*
			if(symbolTable.find(name) != symbolTable.end())
			{
				value = symbolTable.find(name)->second.getValue();
			}
			else
			{
				*/
				processError("reference to undefined constant");
			//}
		}
	return value;
	
}


//code
//--------------------------------------------------------------------------------------------------------------------------

void Compiler::code(string op, string operand1, string operand2)
{
	if (op == "program")
		emitPrologue(operand1);
	else if (op == "end")
		emitEpilogue();
	else
		processError("compiler error since function code should not be called with illegal arguments");
}
//-----------------------------------------------------------------------------------------------------------------------------------

// Emit Functions
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::emit(string label, string instruction, string operands, string comment)
{    
	//Turn on left justification in objectFile
	objectFile << left << setw(8) << label << setw(8) << instruction << setw(24) << operands << comment;
	/*
	Output label in a field of width 8
	Output instruction in a field of width 8
	Output the operands in a field of width 24
	Output the comment
*/
}

void Compiler::emitPrologue(string progName, string)
{   
	time_t now = time (NULL);
	//%INCLUDE "Along32.inc"
	//%INCLUDE "Macros_Along.inc"
	//Output identifying comments at beginning of objectFile
	objectFile << "; Logan Wongbangchuad		"  << ctime(&now);
	
	objectFile << "%INCLUDE \"Along32.inc\"" << endl;
	objectFile << "%INCLUDE \"Macros_Along.inc\"" << endl << endl;

	//Output the %INCLUDE directives
	emit("SECTION", ".text");
	objectFile << endl;
	emit("global", "_start", "", "; program " + progName.substr(0,15));
	objectFile << endl << endl;
	emit("_start:");
	objectFile << endl;
}

void Compiler::emitEpilogue(string, string)
{
	emit("","Exit", "{0}");
	objectFile << endl << endl;
	emitStorage();
}

void Compiler::emitStorage()
{
	//cout << "Enter emitStorage" << endl;
	emit("SECTION", ".data");
	objectFile << endl;
	
	map<string, SymbolTableEntry>::iterator itr;
	
	for(itr=symbolTable.begin(); itr != symbolTable.end(); ++itr)
	{
		if(itr->second.getAlloc() == YES && itr->second.getMode() == CONSTANT)
		{
			emit(itr->second.getInternalName(), "dd", itr->second.getValue(),"; " + itr->first);
			objectFile << endl;
		}
	}
	
	objectFile << "\n";
	
	emit("SECTION", ".bss");
	objectFile << endl;
		for(itr=symbolTable.begin(); itr != symbolTable.end(); ++itr)
		{
			if(itr->second.getAlloc() == YES && itr->second.getMode() == VARIABLE)
			{
				emit(itr->second.getInternalName(), "resd", "1", "; " + itr->first);
				objectFile << endl;
			}
		}
	//cout << "Exit emitStorage" << endl;
}

//-----------------------------------------------------------------------------------------------------------------------------------
//Production 1
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::prog() //token should be "program"
{
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
	if (token != "program")
		processError("keyword \"program\" expected");
	x = nextToken();
	if (!isNonKeyId(token))
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
	if (token != "const")
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
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected");
	x = token;
	if (nextToken() != "=")
		processError("\"=\" expected");
	y = nextToken();
	if (y != "+" && y != "-" && y != "not" && !isNonKeyId(y) && y != "true" && y != "false" && !isInteger(y))
		processError("token to right of \"=\" illegal");
	if (y == "+" || y == "-")
	{
		if (!isInteger(nextToken()))
			processError("integer expected after sign");
		y = y + token;
	}
	if (y == "not")
	{
		string value = nextToken();
		if (!isBoolean(token))
		{
			if(isNonKeyId(token))
			{
				if(symbolTable.find(token)->second.getDataType() != BOOLEAN)
				{
					//cout << "here" << endl;
					processError("boolean expected after \"not\"");
				}
				else
				{
					value = symbolTable.find(token)->second.getValue();
				}
			}
		}
		if (value == "true")
			y = "false";
		else
			y = "true";
	}
	if (nextToken() != ";")
		processError("semicolon expected");
	
	//cout << y << endl;
	if (whichType(y) != INTEGER && whichType(y) != BOOLEAN )
		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");
	insert(x,whichType(y),CONSTANT,whichValue(y),YES,1);
	x = nextToken();
	if (x != "begin" && x != "var" && !isNonKeyId(x))
		processError("non-keyword identifier, \"begin\", or \"var\" expected");
	if (isNonKeyId(x))
	{
		//cout << "Recursive call to constStmts" << endl;
		constStmts();
	}
}


//Production 7
//------------------------------------------------------------------------------------------------------------------

void Compiler::varStmts() //token should be NON_KEY_ID
{
	string x,y;
	if (!isNonKeyId(token))
		processError("non-keyword identifier expected");
	x = ids();
	if (token != ":")
		processError("\":\" expected");
	string tok = nextToken();
	//cout << tok << endl;
	if (tok != "integer" && tok != "boolean")
		processError("illegal type follows \":\"");
	y = token;
	storeTypes type;
	type = y == "integer" ? INTEGER : BOOLEAN;
	if (nextToken() != ";")
		processError("semicolon expected");
	insert(x,type,VARIABLE,"",YES,1);
	string tok2 = nextToken();
	if (tok2 != "begin" && !isNonKeyId(tok2))
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
