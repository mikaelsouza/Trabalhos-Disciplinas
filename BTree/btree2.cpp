#include <fstream>
#include "btree2.hpp"

Btree2::Btree2() {
    root_pointer = -1;
    int i;
    num_nodes = 0;
    index_file = fopen("index_file2", "wb+");
    for (i = 0; i < MAX_KEYS_2; i++) {
        this->current_node.node_pointers[i] = -1;
        strcpy(this->current_node.keys[i].key, "-1");
    }
    this->current_node.node_pointers[i] = -1;
    this->current_node.keys_count = 0;
}
Btree2::~Btree2() { fclose(this->index_file); }
void Btree2::insert_register(char key[300], long hash_position) {
    bool move_up;
    long new_right;
    ItemType2 new_item;
    ItemType2 item;
    strcpy(item.key, key);
    //item.key = key;
    item.data_pointer = hash_position;

    push_down(item, root_pointer, move_up, new_item, new_right);

    if (move_up) {
        current_node.keys_count = 1;
        current_node.keys[0] = new_item;
        current_node.node_pointers[0] = root_pointer;
        current_node.node_pointers[1] = new_right;

        num_nodes += 1;
        root_pointer = num_nodes;
        write_node(root_pointer * BLOCK_SIZE, current_node);
    }
}

Node2 Btree2::get_node(long node_offset) {
    node_quantity += 1;
    char block[BLOCK_SIZE];
    Node2 node;

    fseek(index_file, node_offset, SEEK_SET);
    fread(block, 1, BLOCK_SIZE, this->index_file);
    memcpy(&node, block, sizeof(node));
    return node;
}

void Btree2::push_down(ItemType2 &current_item, long current_root, bool &move_up,
                      ItemType2 &new_item, long &new_right) {
    int location;

    if (current_root == -1) {
        move_up = true;
        new_item = current_item;
        new_right = -1;
    } else {
        current_node = this->get_node(current_root * BLOCK_SIZE);

        if (search_node(current_item.key, location)) {
        }

        push_down(current_item, current_node.node_pointers[location + 1], move_up,
                  new_item, new_right);

        if (move_up) {
            current_node =
                    get_node(current_root * BLOCK_SIZE);

            if (current_node.keys_count < MAX_KEYS_2) {
                move_up = false;
                add_item(new_item, new_right, current_node, location + 1);
                write_node(current_root * BLOCK_SIZE, current_node);

            } else {
                move_up = true;
                split(new_item, new_right, current_root, location, new_item, new_right);
            }
        }
    }
}

void Btree2::write_node(long offset, Node2 node) {
    char block[BLOCK_SIZE];

    fseek(this->index_file, offset, SEEK_SET);

    memcpy(block, &node, sizeof(node));

    fwrite(block, 1, BLOCK_SIZE, this->index_file);
}

void Btree2::split(ItemType2 &current_item, long current_right, long current_root,
                  int location, ItemType2 &new_item, long &new_right) {
    int j, median;
    Node2 right_node;

    int min_keys = floor(MAX_KEYS_2 / 2);

    if (location < min_keys) {
        median = min_keys;
    } else {
        median = min_keys + 1;
    }

    current_node = get_node(current_root * BLOCK_SIZE);

    for (j = median; j < MAX_KEYS_2; j++) {
        right_node.keys[j - median] = current_node.keys[j];
        right_node.node_pointers[j - median + 1] =
                current_node.node_pointers[j + 1];
    }

    right_node.keys_count = MAX_KEYS_2 - median;
    current_node.keys_count = median;

    if (location < min_keys) {
        add_item(current_item, current_right, current_node, location + 1);
    } else {
        add_item(current_item, current_right, right_node, location - median + 1);
    }

    new_item = current_node.keys[current_node.keys_count - 1];
    right_node.node_pointers[0] =
            current_node.node_pointers[current_node.keys_count];
    current_node.keys_count -= 1;

    write_node(current_root * BLOCK_SIZE, current_node);

    num_nodes += 1;
    new_right = num_nodes;
    write_node(new_right * BLOCK_SIZE, right_node);
}

void Btree2::add_item(ItemType2 &new_item, long new_right, Node2 &node,
                     int location) {
    int j;
    for (j = node.keys_count; j > location; j--) {
        node.keys[j] = node.keys[j - 1];
        node.node_pointers[j + 1] = node.node_pointers[j];
    }
    node.keys[location] = new_item;
    node.node_pointers[location + 1] = new_right;
    node.keys_count += 1;
}

bool Btree2::retrieve_register(char key[300], ItemType2 &item) {
    node_quantity = 0;
    long current_root;
    int location;
    bool found = false;

    current_root = root_pointer;

    while((current_root != -1) && (!found)){
        current_node = get_node(current_root * BLOCK_SIZE);
        if(search_node(key, location)){
            found = true;
            item = current_node.keys[location];
        } else {
            current_root = current_node.node_pointers[location + 1];
        }
    }
    return found;
}

bool Btree2::search_node(char target[300], int &location) {

    bool found = false;

    if (strcmp(target, current_node.keys[0].key) < 0) {

        location = -1;
    } else {
        location = current_node.keys_count - 1;
        while (strcmp(target, current_node.keys[location].key) < 0 && (location > 0)) {

            location -= 1;
        }
        if (strcmp(target, current_node.keys[location].key) == 0) {
            found = true;
        }
    }
    return found;
}

