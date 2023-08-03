#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>
#include <bitset>
#include <assert.h>
#include <algorithm>
#include <typeinfo>

using std::bitset;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::ostream;
using std::string;
using std::stringstream;
using std::to_string;
using std::vector;

class Label
{

public:
	/* Constructor */
	Label(std::string name, int32_t address, string data_type = "instruction", string content = "")
	{
		this->name = name;
		this->address = address;
		this->data_type = data_type;
		this->content = content;
	}

	/* Getters */
	std::string getName()
	{
		return name;
	}

	int32_t getAddress()
	{
		return address;
	}

	string getData_type()
	{
		return data_type;
	}

	string getContent()
	{
		return content;
	}

private:
	/* Instance variables */
	std::string name;
	int32_t address;
	std::string data_type;
	std::string content;

	/* Overloading */
	friend bool operator==(Label label, string string);
	friend bool operator==(string string, Label label);
	friend ostream &operator<<(ostream &os, Label label);
};

/* Overloading functions */
bool operator==(Label label, string string)
{

	return label.getName() == string;
}

bool operator==(string string, Label label)
{

	return label.getName() == string;
}

ostream &operator<<(ostream &os, Label label)
{

	return os << label.getName();
}

/* Vector to store all the labels in the .data and .text section */
vector<Label> labels;

/* Data types supported in the .data section */
vector<string> data_types{

	".ascii", ".asciiz", ".word", ".byte", ".half"

};

/* Map that maps R - instructions in the format {instruction, function_code} */
map<string, string> R_Instructions{

	{"add", "100000"}, {"addu", "100001"}, {"and", "100100"}, {"div", "011010"}, {"divu", "011011"}, {"jalr", "001001"}, {"jr", "001000"}, {"mfhi", "010000"}, {"mflo", "010010"}, {"mthi", "010001"}, {"mtlo", "010011"}, {"mult", "011000"}, {"multu", "011001"}, {"nor", "100111"}, {"or", "100101"}, {"sll", "000000"}, {"sllv", "000100"}, {"slt", "101010"}, {"sltu", "101011"}, {"sra", "000011"}, {"srav", "000111"}, {"srl", "000010"}, {"srlv", "000110"}, {"sub", "100010"}, {"subu", "100011"}, {"syscall", "001100"}, {"xor", "100110"}, {"clo", "100001"}, {"clz", "100000"}, {"mul", "000010"}, {"madd", "000000"}, {"maddu", "000001"}, {"msub", "000100"}, {"msubu", "000101"}, {"tlt", "110010"}, {"tltu", "110011"}, {"tge", "110000"}, {"tgeu", "110001"}, {"tne", "110110"}, {"teq", "110100"}

};

/* Map that maps I - instructions in the format {instruction, op_code} */
map<string, string> I_Instructions{

	{"addi", "001000"}, {"addiu", "001001"}, {"andi", "001100"}, {"beq", "000100"}, {"bgez", "000001"}, {"bgezal", "000001"}, {"bgtz", "000111"}, {"blez", "000110"}, {"bltz", "000001"}, {"bne", "000101"}, {"lb", "100000"}, {"lbu", "100100"}, {"lh", "100001"}, {"lhu", "100101"}, {"lui", "001111"}, {"lw", "100011"}, {"ori", "001101"}, {"sb", "101000"}, {"slti", "001010"}, {"sc", "111000"}, {"sltiu", "001011"}, {"sh", "101001"}, {"sw", "101011"}, {"xori", "001110"}, {"tlti", "000001"}, {"bltzal", "000001"}, {"swr", "101110"}, {"swl", "101010"}, {"ll", "110000"}, {"tltiu", "000001"}, {"lwr", "100110"}, {"lwl", "100010"}, {"tgei", "000001"}, {"tgeiu", "000001"}, {"tnei", "000001"}, {"teqi", "000001"}

};

map<string, string> J_Instructions{

	{"j", "000010"}, {"jal", "000011"}

};

map<string, int> registers = {

	{"$zero", 0}, {"$at", 1}, {"$v0", 2}, {"$v1", 3}, {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7}, {"$t0", 8}, {"$t1", 9}, {"$t2", 10}, {"$t3", 11}, {"$t4", 12}, {"$t5", 13}, {"$t6", 14}, {"$t7", 15}, {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19}, {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23}, {"$t8", 24}, {"$t9", 25}, {"$k0", 26}, {"$k1", 27}, {"$gp", 28}, {"$sp", 29}, {"$fp", 30}, {"$ra", 31}

};

/* Looks for the address of the label from the label list */
int label_address(string label)
{

	for (auto &elem : labels)
	{

		if (label == elem)
		{

			return elem.getAddress();
		}
	}

	return -1;
}

/* Changes the register to its corresponding 5-bit address */
string reg_address(string reg)
{

	string result;

	for (char const &c : reg)
	{

		if (isdigit(c) != 0)
		{

			return reg;
		}
		else
			break;
	}

	auto it = registers.find(reg);

	if (it != registers.end())
	{

		result = bitset<5>(it->second).to_string();

		return result;
	}
	else
		return "";
}

/* Function that assembles an R - type instruction in machine code */
string makeR_type(string instruction, string rd, string rs, string rt, string shamt, string funct)
{

	string op = "000000";
	string dest_reg = reg_address(rd);
	string first_reg = reg_address(rs);
	string second_reg = reg_address(rt);

	int temp = stoi(shamt);
	string shift_amt = bitset<5>(temp).to_string();

	if (first_reg.empty())
		first_reg = "00000";

	if (second_reg.empty())
		second_reg = "00000";

	if (dest_reg.empty())
		dest_reg = "00000";

	if (instruction == "clo" || instruction == "clz" || instruction == "mul" ||
		instruction == "madd" || instruction == "maddu" || instruction == "msub" ||
		instruction == "msubu")
		op = "011100";

	return op + first_reg + second_reg + dest_reg + shift_amt + funct;
}

/* Function that assembles an J - type instruction in machine code */
string makeJ_type(string op, string address)
{

	int temp = stoi(address);
	string address_val = bitset<26>(temp).to_string();

	return op + address_val;
}
/* Function that assembles an I - type instruction in machine code */
string makeI_type(string instruction, string op, string rs, string rt, string immediate)
{

	string first_reg = reg_address(rs);
	string dest_reg = reg_address(rt);
	int temp = stoi(immediate);
	string imm_val = bitset<16>(temp).to_string();

	if (first_reg.empty())
		first_reg = "00000";

	if (dest_reg.empty())
		dest_reg = "00000";

	return op + first_reg + dest_reg + imm_val;
}

/* Get rid of any spaces or tabs at the start or at the end of a line */
string trim(string line)
{
	string formatted_line;

	if (line.empty())
		return "";

	if (line.at(0) == '\t' || line.at(0) == ' ')
	{

		formatted_line = line.substr(1);
	}
	else
		formatted_line = line;

	if (formatted_line.back() == '\t' || formatted_line.back() == ' ')
	{

		formatted_line = formatted_line.substr(0, line.size() - 1);
	}
	return formatted_line;
}

/* Remove the comments in the file */
stringstream removeComments(ifstream &is)
{

	char ch;		  //The ch variable which will be used as the temporary memory
	stringstream out; //Stringstream variable that will be output

	/* The main algorithm that will delete comments */
	while (is >> std::noskipws >> ch)
	{ //Reads the input stream including whitespace

		/* Conditional check to see if there is a hastag (#) */
		if (ch == '#')
		{
			do
			{
				is >> ch;
				if (is.peek() == '\n')
				{
					break;
				}
			} while (is.peek() != EOF);
			is >> ch;
		}
		if (is.peek() != EOF)
			out << ch; //Concatenates the character to the output string
	}
	/*
    string x = out.str();
    cout << x;
    */
	return out;
}

/* The first parsing function that will store the labels */
void firstParse(string iss)
{
	int data_count = 0;
	int instruction_count = 0;

	string line;		   //String variable that reads each line
	string formatted_line; //String variable that stores the trimmed line
	stringstream is(iss);

	/* While loop that will look for the .data segment */
	while (getline(is, line))
	{

		formatted_line = trim(line);

		if (formatted_line == ".data")
			break;
	}

	/* Once the .data segment is found, store the labels */
	while (getline(is, line))
	{

		formatted_line = trim(line);

		if (!formatted_line.empty())
		{

			/* Checking if any of the data types is contained within the line */
			for (const auto &data_type : data_types)
			{

				if (formatted_line.find(data_type) != string::npos)
				{

					size_t delimiter = formatted_line.find(':');
					string temp = formatted_line.substr(delimiter + 1);
					size_t delimiter2 = temp.find(' ');

					string temp2 = temp.substr(delimiter2 + 1);

					if (temp2.find('"') != string::npos)
					{

						temp2.erase(remove(temp2.begin(), temp2.end(), '"'), temp2.end());
					}

					Label newLabel(formatted_line.substr(0, delimiter), 0x10000000 + (data_count)*4, data_type, temp2);

					data_count++;

					labels.push_back(newLabel);
				}
			}
		}
		if (formatted_line == ".text")
			break;
	}

	/* Parsing the .text segment */
	while (getline(is, line))
	{

		formatted_line = trim(line);

		if (!formatted_line.empty())
		{

			/* If a label is found */
			if (formatted_line.find(':') != string::npos)
			{
				bool format = true;

				if (formatted_line.find(' ') == string::npos)
					format = false;

				size_t delimiter = formatted_line.find(':');

				Label newLabel(formatted_line.substr(0, delimiter), 0x400000 + (instruction_count * 4));

				if (format == true)
					instruction_count++;

				labels.push_back(newLabel);
			}
			else
				instruction_count++;
		}
	}
}

stringstream secondParse(stringstream &is)
{
	int PC = 0;
	string line;
	string formatted_line;
	stringstream result;

	/* Looking for the .text segment */
	while (getline(is, line))
	{

		formatted_line = trim(line);

		if (formatted_line == ".text")
			break;
	}

	/* Assembling the .text segment */
	while (getline(is, line))
	{

		formatted_line = trim(line);

		if (!formatted_line.empty())
		{

			/* Ignore the labels */
			if (formatted_line.find(": ") != string::npos)
			{ //For false-style formatting

				size_t delimiter = formatted_line.find(' ');
				formatted_line = formatted_line.substr(delimiter + 1);
			}
			else if (formatted_line.find(':') != string::npos)
				continue; //For true-style formatting

			/* Assembling the instructions */
			vector<string> tokens;			 //Vector to store the tokens
			stringstream ss(formatted_line); //Change formatted_line into a stringstream
			string intermediate;			 //Intermediate string variable to hold the tokens

			/* Tokenize the line */
			while (getline(ss, intermediate, ' '))
			{

				if (!intermediate.empty())
				{
					/* Remove the commas at the end and store them in the tokens vector */
					if (intermediate.back() == ',')
					{

						tokens.push_back(intermediate.substr(0, intermediate.size() - 1));
					}
					else
						tokens.push_back(intermediate);
				}
			}

			map<string, string>::iterator it;

			if (tokens[0] == "add")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], tokens[3], "0", it->second) << endl;
			}
			else if (tokens[0] == "addu")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], tokens[3], "0", it->second) << endl;
			}
			else if (tokens[0] == "addi")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[2], tokens[1], tokens[3]) << endl;
			}
			else if (tokens[0] == "addiu")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[2], tokens[1], tokens[3]) << endl;
			}
			else if (tokens[0] == "and")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], tokens[3], "0", it->second) << endl;
			}
			else if (tokens[0] == "andi")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[2], tokens[1], tokens[3]) << endl;
			}
			else if (tokens[0] == "clo")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], "", "0", it->second) << endl;
			}
			else if (tokens[0] == "clz")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], "", "0", it->second) << endl;
			}
			else if (tokens[0] == "div")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "divu")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "mult")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "multu")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "mul")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], tokens[3], "0", it->second) << endl;
			}
			else if (tokens[0] == "madd")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "msub")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "maddu")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "msubu")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "nor")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], tokens[3], "0", it->second) << endl;
			}
			else if (tokens[0] == "or")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], tokens[3], "0", it->second) << endl;
			}
			else if (tokens[0] == "ori")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[2], tokens[1], tokens[3]) << endl;
			}
			else if (tokens[0] == "sll")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], "", tokens[2], tokens[3], it->second) << endl;
			}
			else if (tokens[0] == "sllv")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[3], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "sra")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], "", tokens[2], tokens[3], it->second) << endl;
			}
			else if (tokens[0] == "srav")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[3], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "srl")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], "", tokens[2], tokens[3], it->second) << endl;
			}
			else if (tokens[0] == "srlv")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[3], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "sub")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], tokens[3], "0", it->second) << endl;
			}
			else if (tokens[0] == "subu")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[3], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "xor")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], tokens[3], "0", it->second) << endl;
			}
			else if (tokens[0] == "xori")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[2], tokens[1], tokens[3]) << endl;
			}
			else if (tokens[0] == "lui")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, "00000", tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "slt")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], tokens[3], "0", it->second) << endl;
			}
			else if (tokens[0] == "sltu")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], tokens[3], "0", it->second) << endl;
			}
			else if (tokens[0] == "slti")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[2], tokens[1], tokens[3]) << endl;
			}
			else if (tokens[0] == "sltiu")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[2], tokens[1], tokens[3]) << endl;
			}
			else if (tokens[0] == "beq")
			{

				it = I_Instructions.find(tokens[0]);
				int temp = label_address(tokens[3]);

				if (temp == -1)
				{

					result << makeI_type(tokens[0], it->second, tokens[1], tokens[2], tokens[3]) << endl;
				}
				else
				{

					int relative_addr = temp - (0x400000 + ((PC * 4) + 4));
					result << makeI_type(tokens[0], it->second, tokens[1], tokens[2], to_string(relative_addr / 4)) << endl;
				}
			}
			else if (tokens[0] == "bgez")
			{

				it = I_Instructions.find(tokens[0]);
				int temp = label_address(tokens[2]);

				if (temp == -1)
				{

					result << makeI_type(tokens[0], it->second, tokens[1], "00001", tokens[2]) << endl;
				}
				else
				{

					int relative_addr = temp - (0x400000 + ((PC * 4) + 4));
					result << makeI_type(tokens[0], it->second, tokens[1], "00001", to_string(relative_addr / 4)) << endl;
				}
			}
			else if (tokens[0] == "bgezal")
			{

				it = I_Instructions.find(tokens[0]);
				int32_t temp = label_address(tokens[2]);

				if (temp == -1)
				{

					result << makeI_type(tokens[0], it->second, tokens[1], "10001", tokens[2]) << endl;
				}
				else
				{

					int relative_addr = temp - (0x400000 + ((PC * 4) + 4));
					result << makeI_type(tokens[0], it->second, tokens[1], "10001", to_string(relative_addr / 4)) << endl;
				}
			}
			else if (tokens[0] == "bgtz")
			{

				it = I_Instructions.find(tokens[0]);
				int temp = label_address(tokens[2]);

				if (temp == -1)
				{

					result << makeI_type(tokens[0], it->second, tokens[1], "00000", tokens[2]) << endl;
				}
				else
				{

					int relative_addr = temp - (0x400000 + ((PC * 4) + 4));
					result << makeI_type(tokens[0], it->second, tokens[1], "00000", to_string(relative_addr / 4)) << endl;
				}
			}
			else if (tokens[0] == "blez")
			{

				it = I_Instructions.find(tokens[0]);
				int temp = label_address(tokens[2]);

				if (temp == -1)
				{

					result << makeI_type(tokens[0], it->second, tokens[1], "00000", tokens[2]) << endl;
				}
				else
				{

					int relative_addr = temp - (0x400000 + ((PC * 4) + 4));
					result << makeI_type(tokens[0], it->second, tokens[1], "00000", to_string(relative_addr / 4)) << endl;
				}
			}
			else if (tokens[0] == "bltzal")
			{

				it = I_Instructions.find(tokens[0]);
				int temp = label_address(tokens[2]);

				if (temp == -1)
				{

					result << makeI_type(tokens[0], it->second, tokens[1], "10000", tokens[2]) << endl;
				}
				else
				{

					int relative_addr = temp - (0x400000 + ((PC * 4) + 4));
					result << makeI_type(tokens[0], it->second, tokens[1], "10000", to_string(relative_addr / 4)) << endl;
				}
			}
			else if (tokens[0] == "bltz")
			{

				it = I_Instructions.find(tokens[0]);
				int temp = label_address(tokens[2]);

				if (temp == -1)
				{

					result << makeI_type(tokens[0], it->second, tokens[1], "00000", tokens[2]) << endl;
				}
				else
				{

					int relative_addr = temp - (0x400000 + ((PC * 4) + 4));
					result << makeI_type(tokens[0], it->second, tokens[1], "00000", to_string(relative_addr / 4)) << endl;
				}
			}
			else if (tokens[0] == "bne")
			{

				it = I_Instructions.find(tokens[0]);
				int temp = label_address(tokens[3]);

				if (temp == -1)
				{

					result << makeI_type(tokens[0], it->second, tokens[1], tokens[2], tokens[3]) << endl;
				}
				else
				{

					int relative_addr = temp - (0x400000 + ((PC * 4) + 4));
					result << makeI_type(tokens[0], it->second, tokens[1], tokens[2], to_string(relative_addr / 4)) << endl;
				}
			}
			else if (tokens[0] == "j")
			{

				it = J_Instructions.find(tokens[0]);
				int temp = label_address(tokens[1]);

				if (temp == -1)
				{

					result << makeJ_type(it->second, tokens[1]) << endl;
				}
				else
				{

					result << makeJ_type(it->second, to_string(temp / 4)) << endl;
				}
			}
			else if (tokens[0] == "jal")
			{

				it = J_Instructions.find(tokens[0]);
				int temp = label_address(tokens[1]);

				if (temp == -1)
				{

					result << makeJ_type(it->second, tokens[1]) << endl;
				}
				else
				{

					result << makeJ_type(it->second, to_string(temp >> 2)) << endl;
				}
			}
			else if (tokens[0] == "jalr")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], tokens[2], "", "0", it->second) << endl;
			}
			else if (tokens[0] == "jr")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], "", "0", it->second) << endl;
			}
			else if (tokens[0] == "teq")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "teqi")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[1], "01100", tokens[2]) << endl;
			}
			else if (tokens[0] == "tne")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "tnei")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[1], "01110", tokens[2]) << endl;
			}
			else if (tokens[0] == "tge")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "tgeu")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "tgei")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[1], "01000", tokens[2]) << endl;
			}
			else if (tokens[0] == "tgeiu")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[1], "01001", tokens[2]) << endl;
			}
			else if (tokens[0] == "tlt")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "tltu")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], tokens[2], "0", it->second) << endl;
			}
			else if (tokens[0] == "tlti")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[1], "01010", tokens[2]) << endl;
			}
			else if (tokens[0] == "tltiu")
			{

				it = I_Instructions.find(tokens[0]);
				result << makeI_type(tokens[0], it->second, tokens[1], "01011", tokens[2]) << endl;
			}
			else if (tokens[0] == "lb")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "lbu")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "lh")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "lhu")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "lw")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "lwl")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "lwr")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "ll")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "sb")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "sh")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "sw")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "swl")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "swr")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "sc")
			{

				it = I_Instructions.find(tokens[0]);
				size_t open_bracket = tokens[2].find('(');
				size_t close_bracket = tokens[2].find(')');
				string rs = tokens[2].substr(open_bracket + 1, close_bracket - (open_bracket + 1));

				result << makeI_type(tokens[0], it->second, rs, tokens[1], tokens[2]) << endl;
			}
			else if (tokens[0] == "mfhi")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], "", "", "0", it->second) << endl;
			}
			else if (tokens[0] == "mflo")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], tokens[1], "", "", "0", it->second) << endl;
			}
			else if (tokens[0] == "mthi")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], "", "0", it->second) << endl;
			}
			else if (tokens[0] == "mtlo")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", tokens[1], "", "0", it->second) << endl;
			}
			else if (tokens[0] == "syscall")
			{

				it = R_Instructions.find(tokens[0]);
				result << makeR_type(tokens[0], "", "", "", "0", it->second) << endl;
			}

			PC++;
			tokens.clear();
		}
	}

	return result;
}

/* Main assembling function */
int assemble(string filename)
{
	ifstream infile;
	infile.open(filename);
	if (infile.is_open())
	{
		stringstream formatted_file = removeComments(infile);
		string no_comments = formatted_file.str();
		firstParse(no_comments);

		stringstream result = secondParse(formatted_file);

		string x = result.str();

		cout << x;
	}

	infile.close();
	return 0;
}

/* Main function */
int main()
{
	/* use the input_test.txt file */
	assemble("C:\\Users\\Bubung\\Documents\\input_test.txt");
	return 0;
};