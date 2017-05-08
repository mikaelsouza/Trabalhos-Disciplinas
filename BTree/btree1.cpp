#include "btree1.hpp"



Btree1::Btree1() {
    //Inicializa os valores da árvore
    root_pointer = -1;
    int i;
    num_nodes = 0;
    //Abre o arquivo para escrita em binário
    index_file = fopen("index_file1", "wb+");

    for (i = 0; i < MAX_KEYS; i++) {
        //Inicializa os apontadores
        this->current_node.node_pointers[i] = -1;
        this->current_node.keys[i].key = -1;
    }
    this->current_node.node_pointers[i] = -1;
    this->current_node.keys_count = 0;
}

Btree1::~Btree1() { fclose(this->index_file); }

void Btree1::insert_register(int key, long hash_position) {
    //Insere chave e apontador de registro
    bool move_up;
    long new_right;
    ItemType1 new_item;
    ItemType1 item;

    item.key = key;
    item.data_pointer = hash_position;

    //Função auxiliar que vai encontrar posicionamento correto do dado a ser gravado
    push_down(item, root_pointer, move_up, new_item, new_right);

    if (move_up) {
        //Se o dado subiu
        current_node.keys_count = 1;
        current_node.keys[0] = new_item;
        current_node.node_pointers[0] = root_pointer;
        current_node.node_pointers[1] = new_right;

        num_nodes += 1;
        root_pointer = num_nodes;
        //Escreve as modificações no arquivo.
        write_node(root_pointer * BLOCK_SIZE, current_node);
    }
}

Node1 Btree1::get_node(long node_offset) {
    //Função de leitura dos dados para ler o no
    node_quantity += 1;
    char block[BLOCK_SIZE];
    Node1 node;

    fseek(index_file, node_offset, SEEK_SET);
    fread(block, 1, BLOCK_SIZE, this->index_file);
    memcpy(&node, block, sizeof(node));
    return node;
}

void Btree1::push_down(ItemType1 &current_item, long current_root, bool &move_up,
                      ItemType1 &new_item, long &new_right) {
    int location;

    if (current_root == -1) {
        move_up = true;
        new_item = current_item;
        new_right = -1;
    } else {
        current_node = this->get_node(current_root * BLOCK_SIZE); // uao magica
        //Verifica onde o elemento deve ser gravado, alterando location e verifica se o elemento
        //já não foi inserido na árvore
        if (search_node(current_item.key, location)) {
            std::cerr << "Inserindo ID repetido." << std::endl;
            exit(1);
        }

        //Chamada recursiva para identificar posicionamento correto do dado a ser inserido
        push_down(current_item, current_node.node_pointers[location + 1], move_up,
                  new_item, new_right);

        if (move_up) {
            //Leitura do dado em disco
            current_node =
                    get_node(current_root * BLOCK_SIZE); // Talvez isso esteja errado.
            //Se não for necessário colocar o elemento para o bloco pai
            if (current_node.keys_count < MAX_KEYS) {
                move_up = false;
                add_item(new_item, new_right, current_node, location + 1);
                write_node(current_root * BLOCK_SIZE, current_node);

            } else {
                //Se for necessário realiza o split e altera a flag move_up para true
                move_up = true;
                split(new_item, new_right, current_root, location, new_item, new_right);
            }
        }
    }
}

void Btree1::write_node(long offset, Node1 node) {

    //Função auxiliar para escrever bloco.
    char block[BLOCK_SIZE];

    fseek(this->index_file, offset, SEEK_SET);

    memcpy(block, &node, sizeof(node));

    fwrite(block, 1, BLOCK_SIZE, this->index_file);
}

void Btree1::split(ItemType1 &current_item, long current_right, long current_root,
                  int location, ItemType1 &new_item, long &new_right) {
    int j, median;
    Node1 right_node;

    int min_keys = floor(MAX_KEYS / 2);
    //Acha ponto de quebra

    if (location < min_keys) {
        median = min_keys;
    } else {
        median = min_keys + 1;
    }
    //Le o no no disco
    current_node = get_node(current_root * BLOCK_SIZE);

    //Pega os dados relevantes para o nó que será criado
    for (j = median; j < MAX_KEYS; j++) {
        right_node.keys[j - median] = current_node.keys[j];
        right_node.node_pointers[j - median + 1] =
                current_node.node_pointers[j + 1];
    }

    //Seta valores do novo nó
    right_node.keys_count = MAX_KEYS - median;
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
    //Escreve o novo nó atual e o nó que foi criado a direita.
    write_node(current_root * BLOCK_SIZE, current_node);

    num_nodes += 1;
    new_right = num_nodes;
    write_node(new_right * BLOCK_SIZE, right_node);
}

void Btree1::add_item(ItemType1 &new_item, long new_right, Node1 &node,
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

bool Btree1::retrieve_register(int key, ItemType1 &item) {
    node_quantity = 0;
    long current_root;
    int location;
    bool found = false;

    current_root = root_pointer;
    //Verifica usando searchnode o location correto que a chave deve estar
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

bool Btree1::search_node(int target, int &location) {

    bool found = false;

    if (target < current_node.keys[0].key) {
        location = -1;
    } else {
        location = current_node.keys_count - 1;
        while ((target < current_node.keys[location].key) && (location > 0)) {
            location -= 1;
        }
        if (target == current_node.keys[location].key) {
            found = true;
        }
    }

    return found;
}
