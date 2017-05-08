#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <unordered_map>
#include "classes.hpp"
#ifndef DATA_BASE_CPP
#define DATA_BASE_CPP

void print_sucessful_message(std::string table, std::string action){
    std::cout << table << " has been " << action << " successfully" << std::endl;
}

void create_tables(pqxx::work* work){

    std::string create_product =
    "CREATE TABLE PRODUTO"\
    "	( ASIN			CHAR(10)		NOT NULL,"\
    "	  TITLE			VARCHAR(460)	NOT NULL,"\
    "	  GRUPO			VARCHAR(30)		NOT NULL,"\
    "	  SALESRANK		INT				NOT NULL,"\
    "	  REVIEWS_TOTAL	INT,"\
    "	  PRIMARY KEY (ASIN));";

    std::string create_similar =
    "CREATE TABLE SIMILARES"\
    "	(	ASIN			CHAR(10)	NOT NULL,"\
    "		ASIN_SIMILAR	CHAR(10)	NOT NULL,"\
    "		PRIMARY KEY (ASIN,ASIN_SIMILAR),"\
    "		FOREIGN KEY(ASIN) REFERENCES PRODUTO(ASIN),"\
    "		FOREIGN KEY(ASIN_SIMILAR) REFERENCES PRODUTO(ASIN));";

    std::string create_category =
    "CREATE TABLE CATEGORY"\
    "	(	ID				INT			NOT NULL,"\
    "		CATEGORY		VARCHAR(100),"\
    "		CATEGORY_UPPER	INT,"\
    "		PRIMARY KEY(ID),"\
    "		FOREIGN KEY (CATEGORY_UPPER) REFERENCES CATEGORY(ID));";

    std::string create_category_prod =
    "CREATE TABLE CATEGORY_PROD"\
    "	(	ASIN 	CHAR(10)	NOT NULL,"\
    "		ID		INT			NOT NULL,"\
    "		PRIMARY KEY(ASIN,ID),"\
    "		FOREIGN KEY(ID) REFERENCES CATEGORY(ID),"\
    "		FOREIGN KEY(ASIN) REFERENCES PRODUTO(ASIN));";

    std::string create_review =
    "CREATE TABLE REVIEW"
    "	(	ID  SERIAL PRIMARY KEY,"\
    "		ASIN 		CHAR(10)	NOT NULL,"\
    "		CUSTOMER	CHAR(15)	NOT NULL,"\
    "		DATA		DATE		NOT NULL,"\
    "		RATING		INT			NOT NULL,"\
    "		VOTES		INT			NOT NULL,"\
    "		HELPFUL		INT			NOT NULL,"\
    "		FOREIGN KEY(ASIN) REFERENCES PRODUTO(ASIN));";


    work->exec(create_product);
    print_sucessful_message("product", "created");
    work->exec(create_similar);
    print_sucessful_message("similar", "created");
    work->exec(create_category);
    print_sucessful_message("category", "created");
    work->exec(create_category_prod);
    print_sucessful_message("category_prod", "created");
    work->exec(create_review);
    print_sucessful_message("review", "created");

}

void reset_schema(pqxx::work* work){
    std::string del;
    std::string create;

    del =
    "DROP SCHEMA public CASCADE";

    create =
    "CREATE SCHEMA public";

    work->exec(del);
    work->exec(create);
}

void insert_review(Review &review, pqxx::work* work, std::string ASIN){
    std::string insert_review_sql;

    insert_review_sql =
    "INSERT INTO review (asin, customer, data, rating, votes, helpful) VALUES"\
    "('" + ASIN + "', '" + review.customer + "', '" + review.date + "', " + std::to_string(review.rating) + "," + std::to_string(review.votes) + ", " + std::to_string(review.helpful) + ")";

    work->exec(insert_review_sql);
}

void insert_category(Category &category, pqxx::work* work){
    std::string insert_category_sql;

    if(category.main_category == -1){
        insert_category_sql =
        "INSERT INTO category VALUES"\
        "(" + std::to_string(category.id) + ", '" + category.name + "', NULL);";
    } else {
        insert_category_sql =
        "INSERT INTO category VALUES"\
        "(" + std::to_string(category.id) + ", '" + category.name + "', " + std::to_string(category.main_category) + ")";
    }
    work->exec(insert_category_sql);
}

void insert_product_category(std::string ASIN, int id, pqxx::work* work){
    std::string insert_category_sql;

    insert_category_sql =
    "INSERT INTO category_prod VALUES"\
    "('" + ASIN + "', " + std::to_string(id) + ");";

    work->exec(insert_category_sql);
}

void insert_similar(Similar similar, pqxx::work* work, std::unordered_map<std::string, bool> &inserted_product){
    std::string insert_similar_sql;

    for(auto values: similar.similar_list){
        if(inserted_product[values] == true){
            insert_similar_sql =
            "INSERT INTO similares VALUES"\
            "('" + similar.ASIN + "', '" + values + "');";
            work->exec(insert_similar_sql);
        }
    }
}

void insert_product(Product &product, pqxx::work* work){

    std::string insert_category_sql;

    insert_category_sql =
    "INSERT INTO produto VALUES"\
    "('" + product.ASIN + "', '" + product.title + "', '" + product.group + "', " + std::to_string(product.salesrank) + ", " + std::to_string(product.total) +");";

    work->exec(insert_category_sql);
}

#endif
