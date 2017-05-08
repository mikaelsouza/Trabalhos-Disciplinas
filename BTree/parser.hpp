#ifndef _PARSER
#define _PARSER

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "structs.hpp"

void replace_string(std::string &subject, std::string &search, std::string &replace);
void parse_file(std::string file_path, Hash &data);
std::vector<std::string> fix_broken_line(std::vector<std::string>);
void create_register(Register &r, std::vector<std::string> &split_line);

#endif
