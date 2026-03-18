#pragma once

#include <string>
#include <unordered_map>
#include <vector>

std::vector<int> text_to_bytes(const std::string& text);
std::unordered_map<int, std::string> bytes_to_unicode_map();
std::vector<std::string> bytes_to_symbols(const std::vector<int>& bytes);