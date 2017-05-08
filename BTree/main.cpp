#include "parser.hpp"


void print_register(Register r){
    std::cout << "ID: " << r.id << std::endl;
    std::cout << "Titulo: " << r.title <<std::endl;
    std::cout << "Ano: " << r.year <<std::endl;
    std::cout << "Autores: " << r.authors <<std::endl;
    std::cout << "Citações: " << r.citations << std::endl;
    std::cout << "Atualização: " << r.update_date <<std::endl;
    std::cout << "Snippet: " << r.snippet <<std::endl;
}

int main() {

    DiskHash disk_hash;
    Btree1 btree1;
    Btree2 btree2;
    ItemType1 item;
    ItemType2 item2;
    //Número total de blocos do hash.
    int total_blocks_hash = 0;
    std::string file_name;
    char title[300];
    int id;

    int option;

    do {
        std::cout << "Escolha a opção:" << std::endl;
        std::cout << "(1) - upload <file>" << std::endl;
        std::cout << "(2) - findrec <ID>" << std::endl;
        std::cout << "(3) - seek1 <ID>" << std::endl;
        std::cout << "(4) - seek2 <titulo>" << std::endl;
        std::cout << "(0) - sair" << std::endl;

        std::cin >> option;

        if(option == 1){

            std::cout << "Insira o caminho do arquivo" << std::endl;
            std::cin >> file_name;
            std::cout << "Aguarde!" << std::endl;

            Hash data;
            parse_file(file_name, data);
            //Cria o arquivo bucket que guarda os dados e o arquivo de overflow para ser usado posteriormente
            std::fstream bin_file("buckets.bin", std::ios::out | std::ios::binary);
            std::fstream overflow_file("overflow.bin", std::ios::out | std::ios::binary);
            overflow_file.close();
            for (int i = 0; i < data.buckets.size(); i++){
                //Insere um conjunto de registros no hash em disco e nas árvores
                disk_hash.insert_registers(data.buckets[i], bin_file, btree1, btree2, i, total_blocks_hash);
            }
        }

        if(option == 2){
            int count_blocks = 0;
            std::cout << "Insira ID para busca" << std::endl;
            std::cin >> id;
            //Abre o arquivo buckets e busca o registro nele.
            std::fstream bin_file2("buckets.bin", std::ios::in | std::ios::binary);
            Register r = disk_hash.retrieve_register(id, bin_file2, count_blocks);
            //Se o registro tiver valores padroes de -1, quer dizer que o registro não existe
            if((r.id == -1) && (r.citations == -1) && (r.year == -1) &&
               (strcmp("-1", r.authors) == 0) && (strcmp("-1", r.snippet) == 0) &&
               (strcmp("-1", r.title) == 0) && (strcmp("-1", r.update_date) == 0)) {
                std::cout << "ID não encontrado" << std::endl;
            } else {
                //Imprime os valores do registro encontrado
                print_register(r);
                std::cout << "Total de blocos: " << total_blocks_hash << std::endl;
                std::cout <<  "Blocos lidos: " << count_blocks << std::endl;
            }
            bin_file2.close();

        }
        if(option == 3){
            Hash hash;
            int count_blocks = 0;
            std::cout << "Insira ID para busca" << std::endl;
            std::cin >> id;
            //Abre o arquivo bucket para leitura
            std::fstream bin_file2("buckets.bin", std::ios::in | std::ios::binary);
            //Se for encontrado na árvore o id buscado, o item será modificado com os valores
            //necessários para buscar o Registro no arquivo de dados.
            if(btree1.retrieve_register(id % hash.SIZE, item)) {
                Register r = disk_hash.retrieve_register_tree(item.data_pointer, id, bin_file2);
                // Se for um valor padrão então o dado não foi encontrado no disco.
                if((r.id == -1) && (r.citations == -1) && (r.year == -1) &&
                   (strcmp("-1", r.authors) == 0) && (strcmp("-1", r.snippet) == 0) &&
                   (strcmp("-1", r.title) == 0) && (strcmp("-1", r.update_date) == 0)) {
                    std::cout << "ID não encontrado" << std::endl;
                } else {
                    //Se encontrado imprime os dados
                    print_register(r);
                    std::cout << "Total de blocos: " << btree1.num_nodes << std::endl;
                    std::cout << "Blocos lidos: " << btree1.node_quantity << std::endl;
                }
            } else {
                //Se não está presente na árvore então o dado não foi encontrado.
                std::cout << "ID não encontrado" << std::endl;
            }
        }
        if(option == 4){
            std::string aux;
            //Realiza leitura bugada do C/C++
            std::getline(std::cin, aux);
            std::getline(std::cin, aux);
            strcpy(title, aux.c_str());
            //Verifica se a chave passada está presente na árvore
            //Se sim, modifica o item2 para ter os resultados esperados
            //para buscar o registro necessário.
            if(btree2.retrieve_register(title, item2)) {
                //Se a chave está presente no disco busca o registro no arquivo de dados.
                print_register(get_register(title, item2.data_pointer));
                std::cout << "Total de blocos: " << btree2.num_nodes << std::endl;
                std::cout << "Blocos lidos: " << btree2.node_quantity << std::endl;

            } else {
                std::cout << "Título não encontrado" << std::endl;
            }
        }
    } while (option != 0);

}
