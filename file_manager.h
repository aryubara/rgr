#pragma once

#include<vector>
#include<string>
#include<cstdint>

vector<uint8_t> readFile(const string& path);
void writeFile(const string& path, const vector<uint8_t>& inputData);