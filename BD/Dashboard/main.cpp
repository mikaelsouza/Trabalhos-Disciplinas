#include <iostream>
#include <iomanip>
#include <pqxx/pqxx>
#include "TextTable.h"

int main(int argc, char *argv[]){

    if(argc != 5){
        std::cout << "Os argumentos devem ser na seguinte ordem: nome_do_db usuario senha hostaddress" << std::endl;
        return 1;
    }

    std::string dbname = argv[1];
    std::string user = argv[2];
    std::string password = argv[3];
    std::string host = argv[4];

    pqxx::connection connection("dbname=" + dbname + " user=" + user + " password=" + password + " hostaddr=" + host + "  port=5432");
    pqxx::nontransaction ntrans(connection);

    int option;

    std::cout << "Dashboard - " + dbname + ", " + user + ", " + host << std::endl;

    while(true){

        std::cout << "Insira a opcao para acessar cada consulta (0-7): " << std::endl;
        std::cout << "1 - Listar os 5 comentarios mais uteis e com maior avaliação e os 5 comentarios mais uteis e com menor avaliacao" << std::endl;
        std::cout << "2 - Listar produtos similares com maiores vendas " << std::endl;
        std::cout << "3 - Mostrar a evolução diaria das medias de avaliacao ao longo do tempo de um produto" << std::endl;
        std::cout << "4 - Listar os 10 produtos lideres de venda em cada grupo de produtos" << std::endl;
        std::cout << "5 - Listar os 10 produtos com a maior media de avaliacoes uteis positivas por produto" << std::endl;
        std::cout << "6 - Listar as 5 categorias de produto com a maior media de avaliacoes uteis positivas (demorada)" << std::endl;
        std::cout << "7 - Listar os 10 clientes que mais fizeram comentarios por grupo de produto (demorada)" << std::endl;
        std::cout << "0 - Sair" << std::endl;

        std::cin >> option;

        if(option == 1){

            int c = 0;

            std::cout << "Insira o ASIN do produto: " << std::endl;

            std::string asin;
            std::cin >> asin;

            std::string query = "SELECT * FROM (Select * from Review where asin = '" + asin + "' order by helpful desc, rating desc LIMIT 5) AS A UNION ALL (SELECT * from Review where asin = '" + asin + "' order by helpful desc, rating LIMIT 5)";

            pqxx::result results(ntrans.exec(query));

            TextTable *t = new TextTable('-', '|', '+');

            std::cout << "5 comentarios mais uteis e com maior avaliacao do produto " + asin << std::endl;
            t->add( "ID" );
            t->add( "ASIN" );
            t->add( "Customer" );
            t->add( "Date" );
            t->add( "Rating" );
            t->add( "Votes" );
            t->add( "Helpful" );
            t->endOfRow();

            for(auto linha:results){
                c++;
                if(c == 6){
                    std::cout << *t;
                    std::cout << std::endl;
                    std::cout << "5 comentarios mais uteis e com menor avaliacao do produto " +  asin << std::endl;
                    delete t;
                    t = new TextTable('-', '|', '+');
                    t->add( "ID" );
                    t->add( "ASIN" );
                    t->add( "Customer" );
                    t->add( "Date" );
                    t->add( "Rating" );
                    t->add( "Votes" );
                    t->add( "Helpful" );
                    t->endOfRow();
                }
                for(auto elemento:linha){
                    std::string aux = elemento.c_str();
                    t->add(aux);
                }
                t->endOfRow();
            }
        std::cout << *t << std::endl << std::endl;

        } else if(option == 2){

            std::cout << "Insira o ASIN do produto: " << std::endl;
            std::string asin;
            std::cin >> asin;

            std::string query = "SELECT * FROM produto WHERE asin IN (SELECT asin_similar FROM similares WHERE asin = '" + asin + "') AND salesrank < (SELECT salesrank FROM produto WHERE asin = '" + asin + "')";

            pqxx::result results(ntrans.exec(query));

            TextTable t('-', '|', '+');

            t.add( "ASIN" );
            t.add( "TITLE" );
            t.add( "GRUPO" );
            t.add( "SALESRANK" );
            t.add( "REVIEWS_TOTAL" );
            t.endOfRow();

            for(auto linha:results){
                for(auto elemento:linha){
                    std::string aux = elemento.c_str();
                    t.add(aux);
                }
                t.endOfRow();
            }
            std::cout << t;
        } else if(option == 3){
            std::cout << "Insira o ASIN do produto: " << std::endl;
            std::string asin;
            std::cin >> asin;

            std::string query = "SELECT data, round(AVG(rating), 2) FROM review WHERE asin = '" + asin + "' GROUP BY data;";

            pqxx::result results(ntrans.exec(query));

            TextTable t('-', '|', '+');

            t.add( "ASIN" );
            t.add( "DATA" );
            t.add( "MEDIA" );

            t.endOfRow();

            for(auto linha:results){
                t.add(asin);
                for(auto elemento:linha){

                    std::string aux = elemento.c_str();
                    t.add(aux);

                }
                t.endOfRow();
            }
            std::cout << t;
        } else if(option == 4){
            std::string query = "SELECT asin, title, grupo, salesrank, reviews_total FROM (SELECT *, ROW_NUMBER() OVER (PARTITION BY grupo order by salesrank) AS Row_ID FROM produto WHERE salesrank > 0) AS A WHERE Row_ID <= 10 ORDER BY grupo;";
            pqxx::result results(ntrans.exec(query));

            TextTable t('-', '|', '+');

            t.add( "ASIN" );
            t.add( "TITLE" );
            t.add( "GROUP" );
            t.add( "SALESRANK" );
            t.add( "REVIEW_TOTAL");

            t.endOfRow();

            for(auto linha:results){
                for(auto elemento:linha){

                    std::string aux = elemento.c_str();
                    t.add(aux);

                }
                t.endOfRow();
            }
            std::cout << t;
        } else if(option == 5){
            std::string query = "SELECT asin, ROUND(AVG(helpful), 2) FROM review GROUP BY asin ORDER BY AVG(helpful) desc LIMIT 10;";
            pqxx::result results(ntrans.exec(query));

            TextTable t('-', '|', '+');

            t.add( "ASIN" );
            t.add( "MEDIA" );

            t.endOfRow();

            for(auto linha:results){
                for(auto elemento:linha){
                    std::string aux = elemento.c_str();
                    t.add(aux);
                }
                t.endOfRow();
            }
            std::cout << t;

        } else if(option == 6){
            std::string query = "SELECT c.category, ROUND(AVG(helpful), 2) FROM category c JOIN category_prod cp ON c.id = cp.id JOIN produto p ON cp.asin = p.asin JOIN review r ON r.asin = p.asin GROUP BY c.category ORDER BY AVG(helpful) desc limit 5;";
            pqxx::result results(ntrans.exec(query));

            TextTable t('-', '|', '+');

            t.add( "CATEGORY" );
            t.add( "MEDIA" );

            t.endOfRow();

            for(auto linha:results){
                for(auto elemento:linha){
                    std::string aux = elemento.c_str();
                    t.add(aux);
                }
                t.endOfRow();
            }
            std::cout << t;

        } else if(option == 7){
            std::string query = "SELECT * FROM (SELECT *, ROW_NUMBER() OVER (PARTITION BY grupo ORDER BY qnt DESC) AS rank from (SELECT count(id) as qnt, customer, grupo FROM (produto NATURAL JOIN review) as q GROUP BY grupo, customer) AS x) AS w WHERE rank <= 10;";

            pqxx::result results(ntrans.exec(query));

            TextTable t('-', '|', '+');

            t.add( "QUANTIDADE" );
            t.add( "CUSTOMER" );
            t.add( "GRUPO" );
            t.add( "RANK" );

            t.endOfRow();

            for(auto linha:results){
                for(auto elemento:linha){
                    std::string aux = elemento.c_str();
                    t.add(aux);
                }
                t.endOfRow();
            }
            std::cout << t;
        } else if (option == 0){
            return 0;
        } else {
            std::cout << "Opcao invalida, tente novamente" << std::endl;
        }
    }
    connection.disconnect();
    return 0;
}
