#ifndef BTREE1_HPP
#define BTREE1_HPP

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 4096
#define MAX_KEYS 170

typedef struct {
    long data_pointer;
    int key;
} ItemType1;

typedef struct {
    ItemType1 keys[MAX_KEYS];
    long node_pointers[MAX_KEYS + 1];
    int keys_count;
} Node1;


class Btree1 {
public:
    int node_quantity;
    Node1 current_node;
    int num_nodes;
    long root_pointer;
    FILE *index_file;

    Btree1();
    ~Btree1();

    Node1 get_node(long node_offset);
    void insert_register(int key, long hash_position);
    void push_down(ItemType1 &current_item, long root_pointer, bool &move_up,
                   ItemType1 &new_item, long &new_right);
    void split(ItemType1 &current_item, long current_right, long current_root,
               int location, ItemType1 &new_item, long &new_right);
    void add_item(ItemType1 &new_item, long new_right, Node1 &node, int location);
    bool retrieve_register(int key, ItemType1 &item);
    bool search_node(int target, int &location);
    void write_node(long offset, Node1 node);
};

#endif