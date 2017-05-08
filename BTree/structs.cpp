#include "structs.hpp"
#include <fstream>

Hash::Hash(){
    buckets.resize(SIZE);
}

void Hash::insert_register(Register r) {
    int position = r.id % this->SIZE;
    this->buckets[position].push_back(r);
}

void DiskHash::insert_registers(std::vector<Register> registers, std::fstream &bin_file, Btree1 &btree, Btree2 &btree2, int position, int &total_blocks) {

    int i;
    unsigned long size = registers.size();
    Register r[7];
    Register empty_register;

    //Verifica se o conjunto de registros tem mais de 7 elementos, se tiver manda eles pro overflow.

    if (size > 7) {
        for (i = 0; i < 7; i++) {
            r[i] = registers[i];
            btree2.insert_register(r[i].title, (int)bin_file.tellg() + i * (int)sizeof(Register));
        }
    } else {
        for (i = 0; i < size; i++) {
            r[i] = registers[i];
            btree2.insert_register(r[i].title, (int)bin_file.tellg() + i * (int)sizeof(Register));
        }
        //Se tiver menos de 7 elementos, completa o conjunto de 7 elementos com valores vazios
        empty_register.year = -1;
        empty_register.citations = -1;
        empty_register.id = -1;
        strcpy(empty_register.authors, "-1");
        strcpy(empty_register.snippet, "-1");
        strcpy(empty_register.title, "-1");
        strcpy(empty_register.update_date, "-1");

        while (i < 7) {
            r[i] = empty_register;
            i += 1;
        }
    }
    // Insere no buffer e manda pro disco.

    if (size){
        //Insere a a chave ID na árvore junto com a posicao do bloco do arquivo.
        btree.insert_register(position, bin_file.tellg());
        total_blocks += 1;
    }
    memcpy(buffer, r, sizeof(r));
    bin_file.write(buffer, BLOCK_SIZE);

    std::fstream overflow_file("overflow.bin", std::ios_base::app);

    while(i < size){
        //Cada elemento do Registro é adicionado com a chave título e o apontador de bloco para
        //o registro
        btree2.insert_register(registers[i].title, overflow_file.tellg());

        insert_in_overflow(registers[i], overflow_file);
        i += 1;
    }
    overflow_file.close();
};

Register DiskHash::retrieve_register(int id, std::fstream &bin_file, int &count_blocks) {

    bool search_overflow = true;

    //Verifica qual posicao do hash deve ser buscado.
    int hash_position = id % HASH_SIZE;
    int file_position = hash_position * BLOCK_SIZE;

    int i;
    Register r[7];
    char buffer[BLOCK_SIZE];
    //Pega o bloco da posicao acima e salva num buffer e o manda para um vetor de registros
    bin_file.seekp(file_position);
    bin_file.read(buffer, BLOCK_SIZE);
    memcpy(r, buffer, sizeof(r));
    count_blocks += 1;

    //Verifica se o registro está no vetor de registros
    for(i = 0; i < 7; i++) {
        if (r[i].id == id) {
            return r[i];
        }
    }

    Register aux;
    aux = r[6];

    //Se o ultimo elemento do vetor de registros for vazio, quer dizer que o vetor
    //contem menos de 7 elementos e portanto não será necessário buscar no overflow
    //Se conter mais de 7 elementos, pode ser que o resto esteja no overflow.
    if((aux.id == -1) && (aux.citations == -1) && (aux.year == -1) &&
       (strcmp("-1", aux.authors) == 0) && (strcmp("-1", aux.snippet) == 0) &&
       (strcmp("-1", aux.title) == 0) && (strcmp("-1", aux.update_date) == 0)){

        search_overflow = false;
    }

    Register new_register;
    new_register.year = -1;
    new_register.citations = -1;
    new_register.id = -1;
    strcpy(new_register.authors, "-1");
    strcpy(new_register.snippet, "-1");
    strcpy(new_register.title, "-1");
    strcpy(new_register.update_date, "-1");

    if(search_overflow){
        retrieve_from_overflow(id, new_register);
    }

    return new_register;
}

void DiskHash::insert_in_overflow(Register r, std::fstream &overflow_file) {

    int size_r = sizeof(r);
    char buffer_overflow[size_r];
    Register reg[1];
    reg[0] = r;

    memcpy(buffer_overflow, reg, sizeof(r));
    overflow_file.write(buffer_overflow, size_r);
}

bool DiskHash::retrieve_from_overflow(int id, Register &input_reg) {
    std::fstream overflow_file("overflow.bin", std::ios::in | std::ios::binary);
    Register reg[1];
    Register r;
    int size_r = sizeof(r);
    char overflow_buffer[size_r];
    //Busca sequencial no overflow procurando por id
    while(overflow_file.read(overflow_buffer, sizeof(reg))){
        memcpy(reg, overflow_buffer, sizeof(r));
        r = reg[0];
        if(r.id == id){
            input_reg = r;
            return true;
        }
    }
    return false;
}

Register DiskHash::retrieve_register_tree(long file_position, int id, std::fstream &bin_file) {

    int i;
    Register r[7];
    char buffer[BLOCK_SIZE];
    bin_file.seekp(file_position);
    bin_file.read(buffer, BLOCK_SIZE);
    memcpy(r, buffer, sizeof(r));

    for(i = 0; i < 7; i++) {
        if (r[i].id == id) {
            return r[i];
        }
    }

    Register aux;
    aux = r[6];

    if((aux.id == -1) && (aux.citations == -1) && (aux.year == -1) &&
       (strcmp("-1", aux.authors) == 0) && (strcmp("-1", aux.snippet) == 0) &&
       (strcmp("-1", aux.title) == 0) && (strcmp("-1", aux.update_date) == 0)){
        return aux;
    }

    Register new_register;


    retrieve_from_overflow(id, new_register);

    return new_register;
}
Register get_register(char *key, long position){
    std::fstream data_file("buckets.bin", std::ios::in | std::ios::binary);
    std::fstream overflow_file("overflow.bin", std::ios::in | std::ios::binary);

    Register r;
    //Busca posicao passada por parametro no arquivo de dados e verifica se a chave bate
    data_file.seekp(position);
    char buffer[sizeof(Register)];
    data_file.read(buffer, sizeof(Register));
    memcpy(&r, buffer, sizeof(Register));

    if(strcmp(r.title, key) == 0){
        return r;
    }
    //Se não bater, pega o registro no arquivo de overflow na posicao passada e retorna.
    overflow_file.seekp(position);
    overflow_file.read(buffer, sizeof(Register));
    memcpy(&r, buffer, sizeof(Register));

    return r;
}

