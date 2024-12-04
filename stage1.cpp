#include <stage1.h>
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


/*
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------
INTIALIZATION FUNCTIONS---------------------------------------------------------------------------------------------

*/
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

/*
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------
OTHER ROUTINES-----------------------------------------------------------------------------------------------

*/
//processError
//-----------------------------------------------------------------------------------------------------------------------------------
void Compiler::processError(string err)
{
	cout << err << endl;
	cout << "Last Token: " << token << endl;
	listingFile << endl;
	listingFile << "Error: Line " << lineNo << ": ";
	listingFile << err << endl;
	errorCount++;
	//Output err to listingFile
	createListingTrailer();
	exit(EXIT_FAILURE);

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

void Compiler::freeTemp()
{
	currentTempNo--;
	if (currentTempNo < -1)
		processError("compiler error, currentTempNo should be ≥ –1");
}
string Compiler::getTemp()
{
	string temp;
	currentTempNo++;
	temp = "T" + to_string(currentTempNo);
	if (currentTempNo > maxTempNo)
	{
		insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
		maxTempNo++;
	}
	cout << "temp: " << temp << endl;
	return temp;
}
string Compiler::getLabel()
{
	string label;
	static int labelCount = 0;
	label = "L" + to_string(labelCount);
	++labelCount;
	return label;
}
bool Compiler::isTemporary(string s) const // determines if s represents a temporary
{
	return s[0] == 'T';
}
/*
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
LEXICAL ROUTINES-----------------------------------------------------------------------------------------------
*/
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
			if(ch == ':')
			{
				token += ch;
				nextChar();
				if(ch == '=')
				{
					token += ch;
					nextChar();
				}
			}
			else if(ch == '<')
			{
				token += ch;
				nextChar();
				if(ch == '>')
				{
					token += ch;
					nextChar();
				}
				else if(ch == '=')
				{
					token += ch;
					nextChar();
				}
			}
			else if(ch == '>')
			{
				token += ch;
				nextChar();
				if(ch == '=')
				{
					token += ch;
					nextChar();
				}
			}
			else
			{
				token = ch;
				nextChar();
			}
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
	
	cout << "								Token: " << token << endl;
	//cout << "Exit nextToken" << endl;
 return token;
}


/*
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
HELPER FUNCTIONS---------------------------------------------------------------------------------------------
*/
//isKeyword
//-----------------------------------------------------------------------------------------------------------------------------------
bool Compiler::isKeyword(string s) const  // determines if s is a keyword
{
	if(s == "program" || s == "const" || s == "var" || s == "integer" || s == "boolean" ||
	   s == "begin"   || s == "end"   || s == "true"|| s == "false"   || s == "not"     ||
	   s == "mod"	  || s == "div"	  || s == "and" || s == "or"	  || s == "read"	 || s == "write")
	   {
		   return true;
	   }
	return false;
}

//isSpecialSymbol
//-----------------------------------------------------------------------------------------------------------------------------------
bool Compiler::isSpecialSymbol(char c) const // determines if c is a special symbol
{
	if(c == '=' || c == ':' || c == ',' || c == ';' || c == '.' || c == '+' ||
	   c == '-' || c == '*' || c == '(' || c == ')' || c == '<' || c == '>')
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



/*
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------
ACTION ROUTINES-----------------------------------------------------------------------------------------------

*/

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
		{
			cout << "name: " << name << endl;
			processError("illegal use of keyword");
		}
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
	else if (op == "read")
		emitReadCode(operand1, operand2);
	else if (op == "write")
		emitWriteCode(operand1, operand2);
	else if (op == "+")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "+ error" << endl;
			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitAdditionCode(operand1, operand2);
	}
	else if (op == "-")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "- error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitSubtractionCode(operand1, operand2);
	}
	else if (op == "neg")
	{
		if (operand1 != "" && operand2 == "")
		{
			emitNegationCode(operand1);
		}
		else if (operand1 == "" && operand2 != "")
		{
			emitNegationCode(operand2);
		}
		cout << "neg error" << endl;

		processError("compiler error since function code should not be called with illegal arguments");
	}
	else if (op == "not")
	{
		if (operand1 != "" && operand2 == "")
		{
			emitNotCode(operand1);
		}
		else if (operand1 == "" && operand2 != "")
		{
			emitNotCode(operand2);
		}
		cout << "not error" << endl;

		processError("compiler error since function code should not be called with illegal arguments");
	}
	else if (op == "*")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "* error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitMultiplicationCode(operand1, operand2);
	}
	else if (op == "div")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "div error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitDivisionCode(operand1, operand2);
	}
	else if (op == "mod")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "mod error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitModuloCode(operand1, operand2);
	}
	else if (op == "and")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "and error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitAndCode(operand1, operand2);
	}
	else if (op == "or")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "or error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitOrCode(operand1, operand2);
	}
	else if (op == "=")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "= error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitEqualityCode(operand1, operand2);
	}
	else if(op == "<")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "< error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitLessThanCode(operand1, operand2);
	}
	else if(op == "<=")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "<= error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitLessThanOrEqualToCode(operand1, operand2);
	}
	else if(op == ">")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "> error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitGreaterThanCode(operand1, operand2);
	}
	else if(op == ">=")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << ">= error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitGreaterThanOrEqualToCode(operand1, operand2);
	}
	else if(op == "<>")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << "<> error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitInequalityCode(operand1, operand2);
	}
	else if (op == ":=")
	{
		if(operand1 == "" || operand2 == "")
		{
			cout << ":= error" << endl;

			processError("compiler error since function code should not be called with illegal arguments");
		}
		emitAssignCode(operand1, operand2);
	}
	else
	{
		cout << "compiler error" << endl;

		processError("compiler error since function code should not be called with illegal arguments");
	}
}
void Compiler::pushOperator(string op)
{
	cout << "				pushed operator: " << op << endl;

	operatorStk.push(op);
}
string Compiler::popOperator()
{
	if(!operatorStk.empty())
	{
		string top = operatorStk.top();
		cout << "				Top of operator stack: " << top << endl;
		operatorStk.pop();
		return top;
	}
	else
	{

		processError("compiler error; operator stack underflow");
	}
	return "";
}
void Compiler::pushOperand(string operand)
{
	cout << "				pushed operand: " << operand << endl;
	if(isLiteral(operand) && symbolTable.find(operand) == symbolTable.end())
	{
		insert(operand, whichType(operand), CONSTANT, whichValue(operand), YES, 1);
	}
	operandStk.push(operand);
}
	
string Compiler::popOperand()
{
	if(!operandStk.empty())
	{
		string top = operandStk.top();
		cout << "				Top of operand stack: " << top << endl;

		operandStk.pop();
		return top;
	}
	else
	{
		processError("compiler error; operand stack underflow");
	}	
	return "";
}





//-----------------------------------------------------------------------------------------------------------------------------------
/*
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
EMIT FUNCTIONS-----------------------------------------------------------------------------------------------
*/
void Compiler::emit(string label, string instruction, string operands, string comment)
{    
	//Turn on left justification in objectFile
	objectFile << left << setw(8) << label << setw(8) << instruction << setw(24) << operands << comment;
	objectFile << endl;
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
	//objectFile << endl;
	emit("global", "_start", "", "; program " + progName.substr(0,15));
	objectFile << endl << endl;
	emit("_start:");
	//objectFile << endl;
}

void Compiler::emitEpilogue(string, string)
{
	emit("","Exit", "{0}");
	objectFile << endl;
	emitStorage();
}

void Compiler::emitStorage()
{
	//cout << "Enter emitStorage" << endl;
	emit("SECTION", ".data");
	//objectFile << endl;
	
	map<string, SymbolTableEntry>::iterator itr;
	
	for(itr=symbolTable.begin(); itr != symbolTable.end(); ++itr)
	{
		if(itr->second.getAlloc() == YES && itr->second.getMode() == CONSTANT)
		{
			emit(itr->second.getInternalName(), "dd", itr->second.getValue(),"; " + itr->first);
		}
	}
	
	objectFile << "\n";
	
	emit("SECTION", ".bss");
	//objectFile << endl;
		for(itr=symbolTable.begin(); itr != symbolTable.end(); ++itr)
		{
			if(itr->second.getAlloc() == YES && itr->second.getMode() == VARIABLE)
			{
				emit(itr->second.getInternalName(), "resd", "1", "; " + itr->first);
			}
		}
	//cout << "Exit emitStorage" << endl;
}


void Compiler::emitReadCode(string operand, string) 
{
	string name;
	istringstream names(operand);
	while (getline(names, name, ','))
	{
		if(symbolTable.find(name) == symbolTable.end())
		{
			processError("reference to undefined symbol");
		}
		if(symbolTable.find(name)->second.getDataType() != INTEGER)
		{
			processError("can't read variables of this type");
		}
		if(symbolTable.find(name)->second.getMode() != VARIABLE)
		{
			processError("attempting to read to a read-only location");
		}
		emit("","call", "ReadInt", "; read int; value placed in eax");
		emit("", "mov","[" + symbolTable.find(name)->second.getInternalName() + "],eax", "; store eax at " + symbolTable.find(name)->first);
		contentsOfAReg = symbolTable.find(name)->second.getInternalName();
	}
	
}

void Compiler::emitWriteCode(string operand, string)
{
	string name;
	
	static bool definedStorage = false;
	
	istringstream names(operand);
	while (getline(names, name, ','))
	{
		if(symbolTable.find(name) == symbolTable.end())
		{
			processError("reference to undefined symbol");
		}
		if(symbolTable.find(name)->second.getInternalName() != contentsOfAReg)
		{
			emit("","mov", "eax,[" + symbolTable.find(name)->second.getInternalName() + "]", "; load " + symbolTable.find(name)->first + " in eax" );
			contentsOfAReg = symbolTable.find(name)->second.getInternalName();
		}
		if(symbolTable.find(name)->second.getDataType() == INTEGER || symbolTable.find(name)->second.getDataType() != BOOLEAN)
		{
			emit("", "call", "WriteInt", "; write int in eax to standard out");
		}
		emit("", "call", "Crlf", "; write \\r\\n to standard out");
	}
}
void Compiler::emitAssignCode(string operand1, string operand2)        // op2 = op1
{
	//emit("Enter emitAssignCode");
	//cout << "operand1: " << operand1 << endl;
	//cout << "operand2: " << operand2 << endl;
	//cout << "contentsOfAReg: " << contentsOfAReg << endl;
	if (symbolTable.find(operand1) == symbolTable.end())
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to undefined symbol " + operand2);
	}
	if(symbolTable.find(operand1)->second.getDataType() != symbolTable.find(operand2)->second.getDataType())
	{
		processError("incompatible types");
	}
	if(symbolTable.find(operand2)->second.getMode() != VARIABLE)
	{
		processError("can't read variables of this type");
	}
	if(operand1 == operand2)
	{
		//emit(symbolTable.find(operand1)->second.getValue(), symbolTable.find(operand1)->second.getInternalName());
		//emit(symbolTable.find(operand2)->second.getValue(), symbolTable.find(operand2)->second.getInternalName());

		//emit("HERE???");
		return;
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg)
	{
			//emit("Debug in emitAssign:",symbolTable.find(operand2)->second.getValue());

		emit("", "mov", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; AReg = " + operand1);
		contentsOfAReg = symbolTable.find(operand1)->second.getInternalName();
	}
	//emit code to store the contents of that register into the memory location pointed to by operand2
	//emit("Debug in emitAssign: ",symbolTable.find(operand2)->second.getValue());
	emit("","mov","[" + symbolTable.find(operand2)->second.getInternalName() + "],eax", "; " + operand2 + " = AReg");
	contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();
	//if operand1 is a temp then free its name for reuse
	if(isTemporary(operand1))
	{
		freeTemp();
	}
	//emit("Exiting emitAssignCode");
}
void Compiler::emitAdditionCode(string operand1, string operand2)       // op2 +  op1
{
	if(symbolTable.find(operand1) == symbolTable.end())
	{
		processError("reference to undefined symbol " + operand1);
	}
	if(symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to undefined symbol " + operand2);
	}
	if(symbolTable.find(operand1)->second.getDataType() != INTEGER || symbolTable.find(operand2)->second.getDataType() != INTEGER)
	{
		processError("illegal type");
	}
	if((symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg) && isTemporary(contentsOfAReg))
	{
		//emit("Debug in emitAddition[isTemp(eax) && op1 != eax && op2 != eax]:",symbolTable.find(contentsOfAReg)->second.getValue());
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");// emit code to store that temp into memory
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);//change the allocate entry for the temp in the symbol table to yes deassign it
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		//emit("Debug in emitAddition[op1 != eax && op2 != eax]:",symbolTable.find(operand2)->second.getValue());
		emit("", "mov", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();
	}

	if (contentsOfAReg == symbolTable.find(operand2)->second.getInternalName())
	{
		//emit code to perform register-memory addition with operand 1
		emit("", "add", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; AReg = " + operand2 + " + " + operand1);
	}
	else
	{
		//emit code to perform register-memory addition with operand 2
		emit("", "add", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand1 + " + " + operand2);
	}
	
	if(isTemporary(operand1))
	{
		freeTemp();		
	}
	if(isTemporary(operand2))
	{
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(INTEGER);
	//cout << "contentsOfAReg: " << contentsOfAReg << endl;
	pushOperand(contentsOfAReg);
}
void Compiler::emitSubtractionCode(string operand1, string operand2)    // op2 -  op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != INTEGER || symbolTable.find(operand2)->second.getDataType() != INTEGER)
	{
		processError("illegal type");
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");// emit code to store that temp into memory
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);//change the allocate entry for the temp in the symbol table to yes deassign it
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();
	}
	if (contentsOfAReg == symbolTable.find(operand2)->second.getInternalName())
	{
		emit("", "sub", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; AReg = " + operand2 + " - " + operand1);
	}
	else
	{
		emit("", "sub", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2 + " - " + operand1);
	}
	
	if(isTemporary(operand1))
	{
		freeTemp();		
	}
	if(isTemporary(operand2))
	{
		freeTemp();		
	}
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}
void Compiler::emitMultiplicationCode(string operand1, string operand2) // op2 *  op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != INTEGER || symbolTable.find(operand2)->second.getDataType() != INTEGER)
	{
		processError("illegal type");
	}
	if(isTemporary(contentsOfAReg) && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "[" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "], eax", "; deassign AReg");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(!isTemporary(contentsOfAReg) && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg)
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg)
	{
				//emit("Debug in emitMultiplication[op1 & op2 != eax]:",symbolTable.find(operand2)->second.getValue());

		emit("", "mov", "eax, [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();

	}
	if (contentsOfAReg == symbolTable.find(operand2)->second.getInternalName())
	{
						//emit("Debug in emitMultiplication[op2 == eax]:",symbolTable.find(operand1)->second.getValue());

		emit("", "imul", "dword [" + symbolTable.find(operand1)->second.getInternalName() + "]", "; AReg = " + operand2 + " * " + operand1);
	}
	else
	{
				//emit("Debug in emitMultiplication[else clause]:",symbolTable.find(operand2)->second.getValue());

		emit("", "imul", "dword [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand1 + " * " + operand2);
	}
	if(isTemporary(operand1))
    {
      freeTemp();   
    }
	if(isTemporary(operand2))
    {
      freeTemp();   
    }
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}
void Compiler::emitDivisionCode(string operand1, string operand2)       // op2 /  op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != INTEGER || symbolTable.find(operand2)->second.getDataType() != INTEGER)
	{
		processError("illegal type");
	}
	if(isTemporary(contentsOfAReg) && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "[" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "], eax", "; deassign AReg");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(!isTemporary(contentsOfAReg) && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax, [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();
	}
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");
	emit("", "idiv", "dword [" + symbolTable.find(operand1)->second.getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);
	//emit("","xchg", "eax,edx", "; exchange quotient and remainder");
    if(isTemporary(operand1))
    {
      freeTemp();   
    }
    if(isTemporary(operand2))
    {
      freeTemp();   
    }
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(INTEGER);
	pushOperand(contentsOfAReg);
	
}
void Compiler::emitModuloCode(string operand1, string operand2)         // op2 %  op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != INTEGER || symbolTable.find(operand2)->second.getDataType() != INTEGER)
	{
		processError("illegal type");
	}
	if(isTemporary(contentsOfAReg) && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "[" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "],eax", "; deassign AReg");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(!isTemporary(contentsOfAReg) && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg)
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();
	}
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");
	emit("", "idiv", "dword [" + symbolTable.find(operand1)->second.getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);
	emit("","xchg", "eax,edx", "; exchange quotient and remainder");
    if(isTemporary(operand1))
    {
      freeTemp();   
    }
    if(isTemporary(operand2))
    {
      freeTemp();   
    }
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}
void Compiler::emitNegationCode(string operand1, string)           // -op1
{
	if(symbolTable.find(operand1) == symbolTable.end() )
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != INTEGER)
	{
		processError("illegal type");
	}
	if (isTemporary(contentsOfAReg) && (symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg))
	{

		emit("", "mov", "[" +  symbolTable.find(contentsOfAReg)->second.getInternalName() + "],eax", "; deassign AReg");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && (symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if (symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg)
	{
		emit("","mov", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]");
		contentsOfAReg = operand1;	
	}
	emit("","neg","eax");
	
	if (isTemporary(operand1))
	{
		freeTemp();
	}
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}
void Compiler::emitNotCode(string operand1, string)               // !op1
{
	if(symbolTable.find(operand1) == symbolTable.end() )
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != BOOLEAN)
	{
		processError("illegal type");
	}
	if (isTemporary(contentsOfAReg) && (symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg))
	{

		emit("", "mov", "[" +  symbolTable.find(contentsOfAReg)->second.getInternalName() + "],eax", "; deassign AReg");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if (!isTemporary(contentsOfAReg) && (symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if (contentsOfAReg != operand1)
	{
		emit("","mov", "eax, [" + symbolTable.find(operand1)->second.getInternalName() + "]");
		contentsOfAReg = operand1;	
	}
	if (!isTemporary(contentsOfAReg) && (symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if (symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg)
	{
		emit("","mov", "eax, [" + symbolTable.find(operand1)->second.getInternalName() + "]");
		contentsOfAReg = operand1;	
	}
	emit("","not","eax");
	
	if (isTemporary(operand1))
	{
		freeTemp();
	}
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
void Compiler::emitAndCode(string operand1, string operand2)            // op2 && op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != BOOLEAN || symbolTable.find(operand2)->second.getDataType() != BOOLEAN)
	{
		processError("illegal type");
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("", "mov", "eax, [" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "]", "; AReg = " + contentsOfAReg);
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax, [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();

	}
	if (contentsOfAReg == symbolTable.find(operand2)->second.getInternalName())
	{
		emit("","and", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; AReg = " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.find(operand1)->second.getInternalName())
	{
		emit("","and", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand1 + " and " + operand2);
	}
    if(isTemporary(operand1))
    {
      freeTemp();   
    }
    if(isTemporary(operand2))
    {
      freeTemp();   
    }
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
	
}
void Compiler::emitOrCode(string operand1, string operand2)             // op2 || op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != BOOLEAN || symbolTable.find(operand2)->second.getDataType() != BOOLEAN)
	{
		processError("illegal type");
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("", "mov", "eax, [" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "]", "; AReg = " + contentsOfAReg);
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax, [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();

	}
	if (contentsOfAReg == symbolTable.find(operand2)->second.getInternalName())
	{
		emit("","or", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; AReg = " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.find(operand1)->second.getInternalName())
	{
		emit("","or", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand1 + " and " + operand2);
	}
    if(isTemporary(operand1))
    {
      freeTemp();   
    }
    if(isTemporary(operand2))
    {
      freeTemp();   
    }
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
void Compiler::emitEqualityCode(string operand1, string operand2)       // op2 == op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != symbolTable.find(operand2)->second.getDataType())
	{
		processError("illegal type");
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("","mov", "[" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "], eax");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax, [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();

	}
	if(symbolTable.find(operand2)->second.getInternalName() == contentsOfAReg)
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.find(operand1)->second.getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}
	
	string L1 = getLabel();
	emit("", "je", L1);
	
	emit("", "mov", "eax, [FALSE]");
	insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	symbolTable.find("false")->second.setInternalName("FALSE");
	
	string L2 = getLabel();
	emit("","jmp", L2);
	
	emit("." + L1 + ":");
	
	emit("", "mov", "eax, [TRUE]");
	insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	symbolTable.find("true")->second.setInternalName("TRUE");
	
	emit("." + L2 + ":");
	
    if(isTemporary(operand1))
    {
      freeTemp();   
    }
    if(isTemporary(operand2))
    {
      freeTemp();   
    }
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
void Compiler::emitInequalityCode(string operand1, string operand2)     // op2 != op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != symbolTable.find(operand2)->second.getDataType())
	{
		processError("illegal type");
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("","mov", "[" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "], eax");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax, [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();

	}
	if(symbolTable.find(operand2)->second.getInternalName() == contentsOfAReg)
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.find(operand1)->second.getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}
	
	string L1 = getLabel();
	emit("", "jne", L1);
	
	emit("", "mov", "eax, [FALSE]");
	insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	symbolTable.find("false")->second.setInternalName("FALSE");
	
	string L2 = getLabel();
	emit("","jmp", L2);
	
	emit("." + L1 + ":");
	
	emit("", "mov", "eax, [TRUE]");
	insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	symbolTable.find("true")->second.setInternalName("TRUE");
	
	emit("." + L2 + ":");
	
    if(isTemporary(operand1))
    {
      freeTemp();   
    }
    if(isTemporary(operand2))
    {
      freeTemp();   
    }
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
void Compiler::emitLessThanCode(string operand1, string operand2)       // op2 <  op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != symbolTable.find(operand2)->second.getDataType())
	{
		processError("illegal type");
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("","mov", "[" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "], eax");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax, [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();

	}
	if(symbolTable.find(operand2)->second.getInternalName() == contentsOfAReg)
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.find(operand1)->second.getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}
	
	string L1 = getLabel();
	emit("", "jl", L1);
	
	emit("", "mov", "eax, [FALSE]");
	insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	symbolTable.find("false")->second.setInternalName("FALSE");
	
	string L2 = getLabel();
	emit("","jmp", L2);
	
	emit("." + L1 + ":");
	
	emit("", "mov", "eax, [TRUE]");
	insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	symbolTable.find("true")->second.setInternalName("TRUE");
	
	emit("." + L2 + ":");
	
    if(isTemporary(operand1))
    {
      freeTemp();   
    }
    if(isTemporary(operand2))
    {
      freeTemp();   
    }
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) // op2 <= op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != symbolTable.find(operand2)->second.getDataType())
	{
		processError("illegal type");
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("","mov", "[" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "], eax");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax, [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();
	}
	if(symbolTable.find(operand2)->second.getInternalName() == contentsOfAReg)
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.find(operand1)->second.getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}
	
	string L1 = getLabel();
	emit("", "jle", L1);
	
	emit("", "mov", "eax, [FALSE]");
	insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	symbolTable.find("false")->second.setInternalName("FALSE");
	
	string L2 = getLabel();
	emit("","jmp", L2);
	
	emit("." + L1 + ":");
	
	emit("", "mov", "eax, [TRUE]");
	insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	symbolTable.find("true")->second.setInternalName("TRUE");
	
	emit("." + L2 + ":");
	
    if(isTemporary(operand1))
    {
      freeTemp();   
    }
    if(isTemporary(operand2))
    {
      freeTemp();   
    }
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
void Compiler::emitGreaterThanCode(string operand1, string operand2)    // op2 >  op1
{
	cout << "DO WE GET HERE???" << endl;
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != symbolTable.find(operand2)->second.getDataType())
	{
		processError("illegal type");
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("","mov", "[" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "], eax", "; deassign AReg");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax, [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();
	}
	if(symbolTable.find(operand2)->second.getInternalName() == contentsOfAReg)
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.find(operand1)->second.getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}
	
	string L1 = getLabel();
	emit("", "jg", L1);
	
	emit("", "mov", "eax, [FALSE]");
	insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	symbolTable.find("false")->second.setInternalName("FALSE");
	
	string L2 = getLabel();
	emit("","jmp", L2);
	
	emit("." + L1 + ":");
	
	emit("", "mov", "eax, [TRUE]");
	insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	symbolTable.find("true")->second.setInternalName("TRUE");
	
	emit("." + L2 + ":");
	
    if(isTemporary(operand1))
    {
      freeTemp();   
    }
    if(isTemporary(operand2))
    {
      freeTemp();   
    }
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) // op2 >= op1
{
	if(symbolTable.find(operand1) == symbolTable.end() || symbolTable.find(operand2) == symbolTable.end())
	{
		processError("reference to an undefined constant");
	}
	if(symbolTable.find(operand1)->second.getDataType() != symbolTable.find(operand2)->second.getDataType())
	{
		processError("illegal type");
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		emit("","mov", "[" + symbolTable.find(contentsOfAReg)->second.getInternalName() + "], eax");
		symbolTable.find(contentsOfAReg)->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		contentsOfAReg = "";
	}
	if(symbolTable.find(operand1)->second.getInternalName() != contentsOfAReg && symbolTable.find(operand2)->second.getInternalName() != contentsOfAReg)
	{
		emit("", "mov", "eax, [" + symbolTable.find(operand2)->second.getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.find(operand2)->second.getInternalName();
	}
	if(symbolTable.find(operand2)->second.getInternalName() == contentsOfAReg)
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand1)->second.getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.find(operand1)->second.getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.find(operand2)->second.getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}
	
	string L1 = getLabel();
	emit("", "jge", L1);
	
	emit("", "mov", "eax, [FALSE]");
	insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	symbolTable.find("false")->second.setInternalName("FALSE");
	
	string L2 = getLabel();
	emit("","jmp", L2);
	
	emit("." + L1 + ":");
	
	emit("", "mov", "eax, [TRUE]");
	insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	symbolTable.find("true")->second.setInternalName("TRUE");
	
	emit("." + L2 + ":");
	
    if(isTemporary(operand1))
    {
      freeTemp();   
    }
    if(isTemporary(operand2))
    {
      freeTemp();   
    }
	
	contentsOfAReg = getTemp();
	symbolTable.find(contentsOfAReg)->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}


/*
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
PRODUCTIONS--------------------------------------------------------------------------------------------------
*/
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
	nextToken();
	execStmts();
	cout << "returning from execStmts to beginEndStmt" << endl;
	if (token != "end")
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
				cout << "Here" << endl;
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
			else
			{
				processError("boolean expected after \"not\"");
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
	cout << "Entering ids()" << endl;
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


void Compiler::execStmts()      // stage 1, production 2
{	
	cout << "entering execStmts()" << endl;
	if (isNonKeyId(token) || token == "read" || token == "write")	
	{
		cout << "calling execStmt in execStmts" << endl;
		execStmt();	
		cout << "returning from execStmt to execStmts" << endl;
		nextToken();
		cout << "calling execStmts in execStmts(recursive)" << endl;
		execStmts();	
		//cout << "returning from execStmts to execStmts(recursive)" << endl;

	}	
	else if (token != "end")	
	{		
		processError("expected non-key identifier, \"read\", \"write\", or \"end\"");	
	}
	//cout << "Exiting execStmts" << endl;
}
void Compiler::execStmt()       // stage 1, production 3
{	  
	cout << "Entering execStmt()" << endl;
	if(isNonKeyId(token))
	{
		cout << "calling assignStmt in execStmt" << endl;
		assignStmt();
		cout << "returning from assignStmt to execStmt" << endl;

	}
	else if(token == "read")
	{
		cout << "calling readStmt in execStmt" << endl;
		
		readStmt();
		cout << "returning from readStmt to execStmt" << endl;

	}
	else if(token == "write")
	{
		cout << "calling writeStmt in execStmt" << endl;

		writeStmt();
		cout << "returning from writeStmt to execStmt" << endl;

	}
	else if (nextToken() != "end")
	{
		cout << "calling execStmt in execStmt(recursive)" << endl;
		execStmt();
		cout << "returning from execStmt to execStmts" << endl;

	}
	else
	{
		processError("expected non-key identifier, \"read\", or \"write\"");
	}
	//cout << "Exiting execStmt" << endl;
}
void Compiler::assignStmt()     // stage 1, production 4
{	 
	cout << "Entering assignStmt()" << endl;
	if(!isNonKeyId(token))
	{
		processError("assignStmt error");
	}
	pushOperand(token);
	nextToken();
	if(token != ":=")
	{
		processError("assignStmt error");
	}
	pushOperator(":=");
	nextToken();
	cout << "calling express in assignStmt" << endl;
	express();
	cout << "returning from express to assignStmts" << endl;

	if(token != ";")
	{
		processError("';' expected");
	}
	cout << "stack modifications in assignStmt" << endl;
	string rhs = popOperand();
	string lhs = popOperand();
	code(popOperator(), rhs, lhs);
	//cout << "Exiting assignStmt" << endl;
}
void Compiler::readStmt()       // stage 1, production 5
{	
	cout << "Entering readStmt()" << endl;
	if(token != "read")
	{
		processError("readStmt error");
	}
	//cout << "here" << endl;
	nextToken();
	if(token != "(")
	{
		processError("Expecting (");
	}
	nextToken();
	string id = ids();
	if(token != ")")
	{
		processError("Expecting )");
	}
	code("read", id);
	nextToken();
	if(token != ";")
	{
		processError("Expecting ;");
	}
	
	//cout << "Exiting readStmt" << endl;
}
void Compiler::writeStmt()      // stage 1, production 7
{	  
	cout << "Entering writeStmt()" << endl;
	if(token != "write")
	{
		processError("Expecting write");
	}
	nextToken();
	if(token != "(")
	{
		processError("Expecting (");
	}
	nextToken();
	string id = ids();
	if(token != ")")
	{
		processError("Expecting )");
	}
	code("write", id);
	nextToken();
	if(token != ";")
	{
		processError("Expecting ;");
	}
	cout << "Exiting writeStmt" << endl;
}
void Compiler::express()        // stage 1, production 9
{
	cout << "Entering express()" << endl;
	
	//cout << "Token in express: " << token << endl;
	if(token == "not" || token == "true" || token == "false" || token == "(" ||
	   token == "+" || token == "-" || isInteger(token) || isNonKeyId(token))
	   {
		   cout << "calling term in express" << endl;
		   term();
			cout << "returning from term to express" << endl;
	
		   cout << "calling expresses in express" << endl;
		   expresses();
		   cout << "returning from expresses to express" << endl;

	   }
	
	
	//cout << "Exiting express" << endl;	
}
void Compiler::expresses()      // stage 1, production 10
{
	cout << "Entering expresses()" << endl;
	if(token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">")
	{
		pushOperator(token);
		nextToken();
		cout << "calling term in expresses" << endl;
		term();
		cout << "returning from term to expresses" << endl;

		cout << "stack modifications in expresses" << endl;
		string rhs = popOperand();
		string lhs = popOperand();
		code(popOperator(), rhs, lhs);
		cout << "calling expresses in expresses(recursive)" << endl;
		expresses();
		cout << "returning from expresses to expresses" << endl;

	}
	if(token == ")" || token == ";")
	{
		//cout << "return nothing expresses" << endl;
		return;
	}
	
	//cout << "Exiting expresses" << endl;
}
void Compiler::term()           // stage 1, production 11
{	  
	cout << "Entering term()" << endl;
	
	if(token == "not" || token == "true" || token == "false" || token == "(" || token == "+" ||
	   token == "-" || isInteger(token) || isNonKeyId(token))
	   {
		   cout << "calling factor in term" << endl;
		   factor();
			cout << "returning from factor to term" << endl;

		   cout << "calling terms in term" << endl;
		   terms();
			cout << "returning from terms to term" << endl;

	   }
	
	//cout << "Exiting term" << endl;
}
void Compiler::terms()          // stage 1, production 12
{	  
	cout << "Entering terms()" << endl;
	if(token == "-" || token == "+" || token == "or")
	{
		//cout << "---token--- in terms: " << token << endl;
		pushOperator(token);
		nextToken();
		cout << "calling factor in terms" << endl;
		factor();
		cout << "returning from factor to terms" << endl;

		cout << "stack modifications in terms" << endl;
		string rhs = popOperand();
		string lhs = popOperand();
		code(popOperator(), rhs, lhs);
		cout << "calling terms in terms(recursive)" << endl;
		terms();
		cout << "returning from terms to terms" << endl;

	}
	if(token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" ||
	   token == ">" || token == ")" || token == ";" )
	   {
		   return;
	   }
	//cout << "Exiting terms" << endl;
}
void Compiler::factor()         // stage 1, production 13
{	 
	cout << "Entering factor()" << endl;
	
	if(token == "not" || token == "true" || token == "false" || token == "(" || token == "+" ||
	   token == "-" || isInteger(token) || isNonKeyId(token))
	   {
		    cout << "calling part in factor" << endl;
			part();
			cout << "returning from part to factor" << endl;

			cout << "calling factors in factor" << endl;
			factors();  
			cout << "returning from factors to factor" << endl;

	   }

	
	//cout << "Exiting factor()" << endl; 
}
void Compiler::factors()        // stage 1, production 14
{
	cout << "Entering factors()" << endl;
	if(token == "*" || token == "div" || token == "mod" || token == "and")
	{
		pushOperator(token);
		nextToken();
		cout << "calling part in factors" << endl;
		part();
		cout << "returning from part to factors" << endl;

		cout << "stack modifications in factors" << endl;
			string rhs = popOperand();
			string lhs = popOperand();
			code(popOperator(), rhs, lhs);
		cout << "calling factors in factors(recursive)" << endl;
		factors();
		cout << "returning from factors to factors" << endl;

	}
	else if(token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" ||
	   token == ">" || token == ")" || token == ";" || token == "-" || token == "+" || token  == "or")
	{
		return;
	}
	   
	//cout << "Exiting factors" << endl;	
}
void Compiler::part()           // stage 1, production 15
{	
	cout << "Entering part()" << endl;
	if(token == "not")
	{
		//cout << "Token = not" << endl;
		nextToken();
		if(token == "(")
		{
			nextToken();
			cout << "calling express in part" << endl;
			express();
			cout << "returning from express to part" << endl;

			if (token != ")")
			{
				processError("')' expected");
			}
			//cout << "stack problems in part/not" << endl;
			code("not", popOperand());
			nextToken();
		}
		else if(isBoolean(token))
		{
			if(token == "true")
			{
				pushOperand("false");
			}
			else if(token == "false")
			{
				pushOperand("true");
			}
			nextToken();
		}
		else if(isNonKeyId(token))
		{
			code("not", token);
			nextToken();
		}
		else
		{
			processError("(, bool, or non-key-id not after not");
		}
		
	}
	else if(token == "+")
	{
		//cout << "token = +" << endl;
		nextToken();
		if(token == "(")
		{
			nextToken();
			cout << "calling express in part" << endl;
			express();
			cout << "returning from express to part" << endl;

			if (token != ")")
			{
				processError("')' expected");
			}
			nextToken();
		}
		else if (isInteger(token) || isNonKeyId(token))
		{
			pushOperand(token);
			nextToken();
		}
		else
		{
			processError("(, int, or non key id is not after +");
		}
	}
	else if(token == "-")
	{
		//cout << "token = -" << endl;
		nextToken();
		if(token == "(")
		{
			nextToken();
			cout << "calling express in part" << endl;

			express();
			cout << "returning from express to part" << endl;

			if (token != ")")
			{
				processError("')' expected");
			}
			//cout << "stack problems in part/-" << endl;
			code("neg", popOperand());
			nextToken();
		}
		else if(isInteger(token))
		{
			string negativeNum = "-" + token;
			pushOperand(negativeNum);
			nextToken();
		}
		else if(isNonKeyId(token))
		{
			code("neg", token);
			nextToken();
		}
		else
		{
			processError("(, int, or non key id is not after +");
		}
	}
	else if(token == "(")
	{
		//cout << "token = (" << endl;
		nextToken();
		cout << "calling express in part" << endl;
		express();
		cout << "returning from express to part" << endl;

		if (token != ")")
		{
			processError("')' expected");
		}
		nextToken();
	}
	else if(isInteger(token) || isBoolean(token) || isNonKeyId(token))
	{
		//cout << "token = int, bool, or non key id" << endl;
		pushOperand(token);
		nextToken();
	}
	else
	{
		processError("expected non_key_id, integer, \"not\", \"true\", \"false\", '(', '+', or '-'");
	}
}
