#include <iostream>
#include <unordered_map>
#include <pqxx/pqxx>
#include "parser.cpp"
#include "data_base.cpp"

int main(int argc, char *argv[]){

    if(argc != 6){
        std::cout << "Os argumentos devem ser na seguinte ordem: caminho_do_arquivo nome_do_db usuario senha hostaddress" << std::endl;
        return 1;
    }

    std::string file_name = argv[1];
    std::string dbname = argv[2];
    std::string user = argv[3];
    std::string password = argv[4];
    std::string host = argv[5];
    std::ifstream input(file_name);
    std::string line;
    std::unordered_map<int, bool> categories;
    std::unordered_map<std::string, bool> product_inserted;
    std::getline(input, line);
    std::getline(input, line);

    Product aux_product;
    int i;
    int product_quantity = get_product_quantity(input, line);
    int j = 1;

    std::vector<Similar> similar_list;

    pqxx::connection connection("dbname=" + dbname + " user=" + user + " password=" + password + " hostaddr=" + host + "  port=5432");
    pqxx::work *global_work = new pqxx::work(connection);

    reset_schema(global_work);

    create_tables(global_work);

    //Pula ID e espaço em branco
    std::getline(input, line);
    std::getline(input, line);

    for(i = 0; i < product_quantity; i++){
        if(j % 100 == 0){
            std::cout << "Já inseriu: " << j << std::endl;
            if(j % 50000 == 0){
                std::cout << "Realizando Commit" << std::endl;
                global_work->commit();
                delete global_work;
                global_work = new pqxx::work(connection);
            }
        }
        j += 1;
        aux_product = parse_product(input, line, similar_list, global_work, categories);
        product_inserted[aux_product.ASIN] = true;
        insert_product(aux_product, global_work);
        for(auto category_id: aux_product.categories){
            insert_product_category(aux_product.ASIN, category_id, global_work);
        }
        for(auto review: aux_product.reviews){
            insert_review(review, global_work, aux_product.ASIN);
        }
        getline(input, line);
        getline(input, line);
    }

    std::cout << "Inserindo similares, aguarde." << std::endl;
    j = 1;
    for(auto value: similar_list){
        j += 1;
        if(j % 50000 == 0){
            global_work->commit();
            delete global_work;
            global_work = new pqxx::work(connection);
        }
        insert_similar(value, global_work, product_inserted);
    }
    global_work->commit();
    delete global_work;
    connection.disconnect();
    return 0;
}
