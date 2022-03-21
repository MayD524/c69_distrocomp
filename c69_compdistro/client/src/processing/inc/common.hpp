#pragma once

#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

bool isFileExist(const string& path) {
    ifstream file(path);
    bool isGood = file.good();
    file.close();
    return isGood;
}

void readFile(const string& path, vector<string>& data) {
    if (!isFileExist(path)) {
        printf("File not found: %s\n", path.c_str());
        return;
    }
    ifstream file(path);
    if (!file.is_open()) {
        printf("File not open: %s\n", path.c_str());
        return;
    }
    string line;
    while (getline(file, line)) {
        data.push_back(line);
    }
    file.close();
}

void split(const string& str, const string& delim, vector<string>& tokens) {
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) {
            pos = str.length();
        }
        string token = str.substr(prev, pos - prev);
        if (!token.empty()) {
            tokens.push_back(token);
        }
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
}

void replace(string& str, const string& from, const string& to) {
    /*
        string& str -> the string to be manipulated
        const string& from -> the sub string to be replaced
        const string& to -> the string to replace the sub string
    */
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

#endif