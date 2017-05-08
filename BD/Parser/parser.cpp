#include <iostream>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <regex>
#include <pqxx/pqxx>
#include "classes.hpp"
#include "data_base.cpp"


//================= AREA DE FUNÇÕES =======================//

int get_product_quantity(std::ifstream &input, std::string &line){
    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "));
    std::getline(input, line);
    return std::stoi(splitted_line[2]);
}

std::string get_ASIN(std::ifstream &input, std::string &line){
    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "));
    std::getline(input, line);
    return splitted_line[1].substr(0, splitted_line[1].size() - 1);
}

bool is_discontinued(std::ifstream &input, std::string &line){
    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "));
    if(splitted_line[2].compare("discontinued") == 0){
        std::getline(input, line);
        return true;
    }
    return false;
}

std::string get_title(std::ifstream &input, std::string &line){

    int i;

    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "));
    int size = splitted_line.size() - 1;
    std::string title = "";

    for(i = 3; i < size; i++){
        title = title + splitted_line[i] + " ";
    }
    std::getline(input, line);
    title = title + splitted_line[i];
    return title.substr(0, title.size() - 1);
}

std::string get_group(std::ifstream &input, std::string &line){

    int i;

    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "));
    int size = splitted_line.size() - 1;
    std::string group = "";

    for(i = 3; i < size; i++){
        group = group + splitted_line[i] + " ";
    }
    std::getline(input, line);
    group = group + splitted_line[i];
    return group.substr(0, group.size() - 1);
}

int get_salesrank(std::ifstream &input, std::string &line){
    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "));
    std::getline(input, line);
    return std::stoi(splitted_line[3]);
}

std::vector<std::string> get_similar_list(std::ifstream &input, std::string &line){
    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "), boost::token_compress_on);
    splitted_line.erase(splitted_line.begin(), splitted_line.begin() + 3);
    int last_position = splitted_line.size();
    if(last_position > 0){
        last_position -= 1;
    }
    splitted_line[last_position] = splitted_line[last_position].substr(0, splitted_line[last_position].size() - 1);
    std::getline(input, line);
    return splitted_line;
}

// Para o caso do [guitar] em categorias.
std::string remove_stupid_string(std::string line){
    std::regex regex_string("\\[\\D+\\]");
    return regex_replace(line, regex_string, "");
}

std::string remove_quotation_marks(std::string line){
    std::regex regex_string("'");
    return regex_replace(line, regex_string, "''");
}

int parse_category(std::string &line, std::unordered_map<int, bool> &categories, pqxx::work* work){
    line = remove_stupid_string(line);
    int list_size;
    std::string category_name;
    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of("|[]"));
    int id;
    int id_super;

    list_size = floor(splitted_line.size() / 3);

    for(int i = 0; i < list_size; i++){
        id = std::stoi(splitted_line[(i * 3) + 2]);
        category_name = remove_quotation_marks(splitted_line[(i * 3) + 1]);
        if(i == 0){
            if(categories[id] == false){
                Category category(-1, id, category_name);
                categories[id] = true;
                insert_category(category, work);
            }
        } else {
            if(categories[id] == false){
                Category category(id_super, id, category_name);
                categories[id] = true;
                insert_category(category, work);
            }
        }
        id_super = id;
    }
    return id;
}

std::vector<int> get_categories(std::ifstream &input, std::string &line, std::unordered_map<int, bool> &categories, pqxx::work* work){
    int categories_quantity;
    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "));
    categories_quantity = std::stoi(splitted_line[3]);
    std::vector<int> categories_list;

    int id;

    for(int i = 0; i < categories_quantity; i++){
        std::getline(input, line);
        id = parse_category(line, categories, work);
        categories_list.push_back(id);
    }
    std::getline(input, line);
    return categories_list;
}

int get_total(std::ifstream &input, std::string &line){
    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "), boost::token_compress_on);
    return std::stoi(splitted_line[3]);
}

Review get_review(std::string &line){
    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "), boost::token_compress_on);
    return Review(splitted_line[1], splitted_line[3], std::stoi(splitted_line[5]), std::stoi(splitted_line[7]), std::stoi(splitted_line[9]));
}

std::vector<Review> get_reviews(std::ifstream &input, std::string &line){
    int review_quantity;
    std::vector<Review> reviews;
    std::vector<std::string> splitted_line;
    boost::split(splitted_line, line, boost::is_any_of(" "), boost::token_compress_on);
    review_quantity = std::stoi(splitted_line[5]);

    for(int i = 0; i < review_quantity; i++){
        std::getline(input, line);
        reviews.push_back(get_review(line));
    }
    std::getline(input, line);
    return reviews;
}

//====================== Fim de área de funções =============================//

Product parse_product(std::ifstream &input, std::string &line, std::vector<Similar> &similar_list, pqxx::work* work, std::unordered_map<int, bool> &categories){

    std::vector<std::string> aux_similar_list;
    Product aux_product;
    aux_product.ASIN = get_ASIN(input, line);
    if(is_discontinued(input, line)){
        aux_product.discontinued = true;
        //TODO: Insere no banco;
        return aux_product;
    }
    aux_product.discontinued = false;

    aux_product.title = remove_quotation_marks(get_title(input, line));

    aux_product.group = get_group(input, line);
    aux_product.salesrank = get_salesrank(input, line);

    aux_similar_list = get_similar_list(input, line);
    similar_list.push_back(Similar(aux_product.ASIN, aux_similar_list));
    aux_product.categories = get_categories(input, line, categories, work);

    aux_product.total = get_total(input, line);
    aux_product.reviews = get_reviews(input, line);

    return aux_product;
}
