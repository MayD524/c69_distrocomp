#pragma once

#ifndef SCRIPT_LANG_HPP
#define SCRIPT_LANG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "./common.hpp"
#include "./system.hpp"

using namespace std;

enum scriptType {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_NULL,
    TYPE_ARRAY,
    TYPE_OBJECT,
    TYPE_PTR
};

enum EnumTokens {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_INC,
    OP_DEC,
    OP_CMP,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_ASSIGN,
    // TODO :
    OP_ASSIGN_ADD,
    OP_ASSIGN_SUB,
    OP_ASSIGN_MUL,
    OP_ASSIGN_DIV,
    OP_INT_DIV,
    // TODO ^^
    OP_JMP,
    OP_JNZ,
    OP_JNE,
    OP_JZ,
    OP_JE,
    OP_JG,
    OP_JL,
    OP_JGE,
    OP_JLE,
    OP_FUN,
    OP_CALL,
    OP_RET,
    OP_OUT,
    OP_IN,
    OP_LABEL,
    DATA
};

const map<string, EnumTokens> TokenTree = {
    {"add", OP_ADD},
    {"sub", OP_SUB},
    {"mul", OP_MUL},
    {"div", OP_DIV},
    {"mod", OP_MOD},
    {"inc", OP_INC},
    {"dec", OP_DEC},
    {"cmp", OP_CMP},
    {"and", OP_AND},
    {"or", OP_OR},
    {"not", OP_NOT},
    {"assign", OP_ASSIGN},
    {"assign_add", OP_ASSIGN_ADD},
    {"assign_sub", OP_ASSIGN_SUB},
    {"assign_mul", OP_ASSIGN_MUL},
    {"assign_div", OP_ASSIGN_DIV},
    {"int_div", OP_INT_DIV},
    {"jmp", OP_JMP},
    {"jnz", OP_JNZ},
    {"jne", OP_JNE},
    {"jz", OP_JZ},
    {"je", OP_JE},
    {"jg", OP_JG},
    {"jl", OP_JL},
    {"jge", OP_JGE},
    {"jle", OP_JLE},
    {"fun", OP_FUN},
    {"call", OP_CALL},
    {"ret", OP_RET},
    {"out", OP_OUT},
    {"in", OP_IN},
};

struct memory_object {
    unsigned int size;
    scriptType   type;
    string       data;
};

struct tokenData {
    EnumTokens   type;
    scriptType   dataType;
    string       data;
};

struct parsedLineData {
    unsigned int lineNumber;
    string       line;
    vector<tokenData>  tokens;
};

struct memPtr {
    unsigned int ptr;
    scriptType   type;
};

class ScriptLang {

public:
    vector<bool> flags = { 
        false, // zero flag
        false, // equal flag
        false, // greater flag
        false, // less flag
        false // equal flag
    };
    vector<parsedLineData> parsedLines;
    map<string, memPtr> memory;
    vector<memory_object> stack;

    scriptType getType(string& dt) {
        if (dt.find("*") != string::npos) {
            return TYPE_PTR;
        }
        // check if variable is float
        else if (dt.find('.') != string::npos) {
            return TYPE_FLOAT;
        }
        // check if variable is int
        else if (dt.find_first_not_of("0123456789") == string::npos) {
            return TYPE_INT;
        }
        // check if variable is string
        else if (dt[0] == '\"' && dt[dt.length() - 1] == '\"') {
            dt.erase(0, 1);
            dt.erase(dt.length() - 1, 1);
            return TYPE_STRING;
        }
        // check if variable is bool
        else if (dt.find("true") != string::npos || dt.find("false") != string::npos) {
            return TYPE_BOOL;
        }
        // check if variable is null
        else if (dt.find("null") != string::npos) {
            return TYPE_NULL;
        }
        else 
            return TYPE_OBJECT; // it's a variable name
    }

public:
    ScriptLang()  {}
    ~ScriptLang() {}

    vector<tokenData> tokenize(const string& line) {
        // split the line into token strings
        vector<tokenData> finalTokens;
        vector<string> tokens;
        split(line, " ", tokens);

        for (string tok : tokens) 
        {
            if (tok.length() == 0) continue;
            
            // check if the token is a keyword
            if (TokenTree.find(tok) != TokenTree.end()) {
                tokenData t;
                t.type = TokenTree.at(tok);
                t.data = tok;
                finalTokens.push_back(t);
            } else {
                // check if the token starts with a '"'
                if (tok[0] == '\"') {
                    tokenData t;
                    t.type = DATA;
                    t.dataType = TYPE_STRING;
                    // join the tokens together till the end of the string
                    for (int x = 1; x < tokens.size(); x++) {
                        t.data += tokens[x];
                        if (tokens[x][tokens[x].length() - 1] != '\"') {
                            t.data += " ";
                        } else {
                            break;
                        }
                    }
                    replace(t.data, "\"", "");
                    finalTokens.push_back(t);
                } else {
                    scriptType type = getType(tok);
                    tokenData t;
                    t.type = DATA;
                    t.dataType = type;
                    if (type == TYPE_PTR)
                        // remove the *
                        tok = tok.substr(1);
                    t.data = tok;
                    finalTokens.push_back(t);
                }
            }
        }
        return finalTokens;
    }

    void preParse(vector<string>& dt) {
        // replace all symbols with their equivalent
        // remove all comments
        for (string& line : dt) {
            line = line.substr(0, line.find(";"));
        }
    }

    void parse(vector<string>& data) {
        preParse(data);
        unsigned int lineNumber = 0;
        for (string line : data) {
            parsedLineData pl;
            pl.lineNumber = lineNumber;
            pl.line = line;
            pl.tokens = tokenize(line);
            parsedLines.push_back(pl);
            lineNumber++;
        }
    }

    memory_object get(const string& name) {
        if (memory.find(name) != memory.end()) {
            return stack[memory[name].ptr];
        }
        return memory_object();
    }

    memory_object& getMemoryRef(const string& ptr_name) {
        return stack[memory[ptr_name].ptr];
    }

    static memory_object getNull() {
        memory_object m;
        m.type = TYPE_NULL;
        return m;
    }

    template <typename T>
    T mathGetSecond(tokenData& token, const scriptType& objType) {
        int object2 = 0;
        if (token.dataType != TYPE_PTR)
        {
            int type = getType(token.data);
            if (type != objType) {
                printf("Error: Can't add different types\n");
                return 0;
            }
            object2 = atoi(token.data.c_str());
        } else {
            if (memory.find(token.data) == memory.end()) {
                printf("Error: Variable not found\n");
                return 0;
            }
            memPtr ptr2;
            ptr2 = memory.at(token.data);
            memory_object obj2 = stack[ptr2.ptr];
            if (objType != obj2.type) {
                printf("Error: Can't add different types\n");
                return 0;
            }
            object2 = atoi(obj2.data.c_str());
        }
        return object2;
    }

    memPtr& makeVariable(const string& name, string data) {
        bool isPtr = false;
        memPtr ptr;
        memory_object m;
        if (memory.find(name) != memory.end()) {
            ptr = memory.at(name);
            m = stack[ptr.ptr];
            if (m.type == TYPE_PTR) {
                isPtr = true;
            }
        } else {
            ptr.ptr = stack.size();
            memory[name] = ptr;
        }
        if (isPtr) {
            m.data = data;
            m.type = getType(data);
            stack[ptr.ptr] = m;
        } else {
            m.data = data;
            m.type = getType(data);
            stack.push_back(m);
        }
        return memory[name];
    }

    void execute() {
        int filePos = 0;
        while (filePos < parsedLines.size()){
        //for (int i = 0; i < parsedLines.size(); i++) {
            parsedLineData& pl = parsedLines[filePos];
            tokenData t = pl.tokens[0];
            
            switch (t.type) {
                case OP_ASSIGN:
                {
                    bool isPtr = false;
                    memPtr ptr;
                    if (memory.find(pl.tokens[1].data) != memory.end()) {
                        ptr = memory.at(pl.tokens[1].data);
                        isPtr = true;
                    } else {
                        ptr.ptr = stack.size();
                        ptr.type = getType(pl.tokens[1].data);
                        memory[pl.tokens[1].data] = ptr;
                    }
                    if (pl.tokens.size() > 2)
                    {
                        // push the value to the stack
                        memory_object obj;
                        obj.size = pl.tokens[2].data.length();
                        obj.type = getType(pl.tokens[2].data);
                        obj.data = pl.tokens[2].data;
                        if (isPtr) {
                            stack[ptr.ptr] = obj;
                        } else {
                            // wtf is wrong with this?
                            stack.push_back(obj);
                        }
                        break;
                    }
                }
                break;

                case OP_INC:
                {
                    if (memory.find(pl.tokens[1].data) == memory.end()) {
                        printf("Error: Variable not found\n");
                        break;
                    }
                    memPtr ptr = memory.at(pl.tokens[1].data);
                    memory_object obj = stack[ptr.ptr];
                    if (obj.type == TYPE_INT) {
                        obj.data = to_string(atoi(obj.data.c_str()) + 1);
                    } else if (obj.type == TYPE_FLOAT) {
                        obj.data = to_string(atof(obj.data.c_str()) + 1);
                    } else {
                        printf("Error: Can't increment non-numeric variable\n");
                    }
                    stack[ptr.ptr] = obj;
                }
                break;

                case OP_DEC:
                {
                    if (memory.find(pl.tokens[1].data) == memory.end()) {
                        printf("Error: Variable not found\n");
                        break;
                    }
                    memPtr ptr = memory.at(pl.tokens[1].data);
                    memory_object obj = stack[ptr.ptr];
                    if (obj.type == TYPE_INT) {
                        obj.data = to_string(atoi(obj.data.c_str()) - 1);
                    } else if (obj.type == TYPE_FLOAT) {
                        obj.data = to_string(atof(obj.data.c_str()) - 1);
                    } else {
                        printf("Error: Can't decrement non-numeric variable\n");
                    }
                    stack[ptr.ptr] = obj;
                }
                break;

                case OP_ADD:
                {
                    if (pl.tokens[1].dataType != TYPE_PTR && memory.find(pl.tokens[1].data) == memory.end()) {
                        printf("Error: First position must be a pointer\n");
                        break;
                    }
                    memory_object& obj = getMemoryRef(pl.tokens[1].data);
                    int object2 = mathGetSecond<int>(pl.tokens[2], obj.type);
                    
                    int result = atoi(obj.data.c_str()) + object2;
                    obj.data = to_string(result);
                }
                break;

                case OP_SUB:
                {
                    if (pl.tokens[1].dataType != TYPE_PTR && memory.find(pl.tokens[1].data) == memory.end()) {
                        printf("Error: First position must be a pointer\n");
                        break;
                    }
                    memory_object& obj = getMemoryRef(pl.tokens[1].data);
                    int object2 = mathGetSecond<int>(pl.tokens[2], obj.type);
                    
                    int result = atoi(obj.data.c_str()) - object2;
                    obj.data = to_string(result);
                }
                break;

                case OP_MUL:
                {
                    if (pl.tokens[1].dataType != TYPE_PTR && memory.find(pl.tokens[1].data) == memory.end()) {
                        printf("Error: First position must be a pointer\n");
                        break;
                    }
                    memory_object& obj = getMemoryRef(pl.tokens[1].data);
                    int object2 = mathGetSecond<int>(pl.tokens[2], obj.type);
                    
                    int result = atoi(obj.data.c_str()) * object2;
                    obj.data = to_string(result);
                
                }
                break;

                case OP_DIV:
                {
                    if (pl.tokens[1].dataType != TYPE_PTR && memory.find(pl.tokens[1].data) == memory.end()) {
                        printf("Error: First position must be a pointer\n");
                        break;
                    }
                    memory_object& obj = getMemoryRef(pl.tokens[1].data);
                    int object2 = mathGetSecond<int>(pl.tokens[2], obj.type);
                    
                    int result = atoi(obj.data.c_str()) / object2;
                    obj.data = to_string(result);
                }
                break;

                case OP_MOD:
                {
                    if (pl.tokens[1].dataType != TYPE_PTR && memory.find(pl.tokens[1].data) == memory.end()) {
                        printf("Error: First position must be a pointer\n");
                        break;
                    }
                    memory_object& obj = getMemoryRef(pl.tokens[1].data);
                    int object2 = mathGetSecond<int>(pl.tokens[2], obj.type);
                    
                    int result = atoi(obj.data.c_str()) % object2;
                    obj.data = to_string(result);
                }
                break;

                case OP_OUT:
                {
                    // printing the value givin by the ptr
                    if (pl.tokens[1].dataType == TYPE_PTR && memory.find(pl.tokens[1].data) != memory.end()) {
                        // print the value
                        printf("%s\n", get(pl.tokens[1].data).data.c_str());
                        break;
                    }
                    printf("%s\n", pl.tokens[1].data.c_str());
                }
                break;

                case OP_JMP:
                {
                    int newLocation = 0;
                    if (pl.tokens[1].dataType == TYPE_PTR && memory.find(pl.tokens[1].data) != memory.end()) {
                        newLocation = atoi(get(pl.tokens[1].data).data.c_str());
                    } else {
                        newLocation = atoi(pl.tokens[1].data.c_str());
                    }
                    if (newLocation < 0 || newLocation >= parsedLines.size()) {
                        printf("Error: Jump location out of bounds\n");
                        break;
                    }
                    filePos = newLocation - 1;
                }
                break;
            }
            filePos++;
        }
        
    }

    void displayPtrs() {
        for (auto& ptr : memory) {
            cout << ptr.first << ": " << ptr.second.ptr << ":" << stack[ptr.second.ptr].data << endl;
        }
    }

    void load(const string& path) {
        vector<string> data;
        readFile(path, data);
    }
};


#endif