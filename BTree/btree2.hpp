#ifndef BTREE2_HPP
#define BTREE2_HPP


#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>


#define BLOCK_SIZE 4096
#define MAX_KEYS_2 12

typedef struct {
    long data_pointer;
    char key[300];
} ItemType2;

typedef struct {
    ItemType2 keys[MAX_KEYS_2];
    long node_pointers[MAX_KEYS_2 + 1];
    int keys_count;
} Node2;


class Btree2 {
public:
    int node_quantity;
    Node2 current_node;
    int num_nodes;
    long root_pointer;
    FILE *index_file;

    Btree2();
    ~Btree2();

    Node2 get_node(long node_offset);
    void insert_register(char key[300], long hash_position);
    void push_down(ItemType2 &current_item, long root_pointer, bool &move_up,
                   ItemType2 &new_item, long &new_right);
    void split(ItemType2 &current_item, long current_right, long current_root,
               int location, ItemType2 &new_item, long &new_right);

    void add_item(ItemType2 &new_item, long new_right, Node2 &node, int location);
    bool retrieve_register(char key[300], ItemType2 &item);
    bool search_node(char target[300], int &location);
    void write_node(long offset, Node2 node);
};

#endif