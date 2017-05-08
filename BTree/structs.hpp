#ifndef _STRUCTS
#define _STRUCTS

#define HASH_SIZE 200003
#include <vector>
#include "btree1.hpp"
#include "btree2.hpp"

typedef struct Register{
    int id;
    int year;
    int citations;
    char authors[100];
    char snippet[100];
    char title[300];
    char update_date[20];
} Register;

class Hash{
public:
    const int SIZE = HASH_SIZE;
    std::vector<std::vector<Register>> buckets;
    Hash();
    void insert_register(Register r);
};

class DiskHash{
public:
    char buffer[BLOCK_SIZE];
    void insert_registers(std::vector<Register> registers, std::fstream &bin_file, Btree1 &btree, Btree2 &btree2, int position, int &total_blocks);
    Register retrieve_register(int id, std::fstream &bin_file, int &count_blocks);
    void insert_in_overflow(Register r, std::fstream &overflow_file);
    bool retrieve_from_overflow(int id, Register &input_reg);
    Register retrieve_register_tree(long hash_position, int id, std::fstream &bin_file);
};

Register get_register(char *key, long position);

#endif
