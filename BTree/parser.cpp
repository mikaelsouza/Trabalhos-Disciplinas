#include "parser.hpp"

void replace_string(std::string &subject, std::string &search, std::string &replace) {

    //Busca uma substring dentro da string e troca por outra string.
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

void parse_file(std::string file_path, Hash &data){
    Register r;
    unsigned long size;
    std::ifstream infile(file_path);

    std::string substitution_string = "\"\"";
    std::string empty_string = "";
    std::string line;
    std::vector<std::string> split_line;

    while(std::getline(infile, line)){

        // Remove as aspas "" e da split nas linhas pelas " que sobram.
        replace_string(line, substitution_string, empty_string);
        boost::split(split_line, line, boost::is_any_of("\""));

        //Conserta strings com tamanho 13 e ignora os outros.
        size = split_line.size();

        if(size == 13) {
            split_line = fix_broken_line(split_line);
        } else if(size != 15){
            continue;
        }
        create_register(r, split_line);
        data.insert_register(r);
    }
}

std::vector<std::string> fix_broken_line(std::vector<std::string> split_line) {
    //Se não houver título ou autor, corrige o split para que os dados possam ser lidos
    std::vector<std::string> fixed_line;
    unsigned long size;
    int i;
    //Verifica as posicoes onde se espera que esteja incorreto o título ou o autor e os modifica.
    if (split_line[2] == ";;" || split_line[6] == ";;") {
        for (auto token : split_line) {
            if (token == ";;") {
                fixed_line.push_back(";");
                fixed_line.push_back("");
                fixed_line.push_back(";");
            } else {
                fixed_line.push_back(token);
            }
        }
    } else {
        size = split_line.size();
        for (i = 0; i < size - 1; i++) {
            fixed_line.push_back(split_line[i]);
        }
        //Conserta string se o snippet for null
        fixed_line.push_back(";");
        fixed_line.push_back("NULL");
        fixed_line.push_back("\n");
    }
    return fixed_line;
}

void create_register(Register &r, std::vector<std::string> &split_line){
    //Cria o registro a partir da string copiando os dados da string que recebeu split.
    r.id = std::stoi(split_line[1]);
    strcpy(r.title, split_line[3].c_str());
    r.year = std::stoi(split_line[5]);
    memcpy(r.authors, split_line[7].c_str(), 99);
    r.title[299] = '\0';
    r.citations = std::stoi(split_line[9]);
    strcpy(r.update_date, split_line[11].c_str());
    memcpy(r.snippet, split_line[13].c_str(), 99);
    r.snippet[99] = '\0';
}