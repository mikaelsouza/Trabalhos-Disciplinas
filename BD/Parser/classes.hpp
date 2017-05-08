#include <iostream>
#include <vector>
#ifndef CLASSES_HPP
#define CLASSES_HPP

class Category{
public:
    int main_category;
    int id;
    std::string name;
    Category(int main_category, int id, std::string name){
        this->main_category = main_category;
        this->id = id;
        this->name = name;
    }
};

class Review{
public:
    std::string date;
    std::string customer;
    int rating;
    int votes;
    int helpful;

    Review(std::string date, std::string customer, int rating, int votes, int helpful){
        this->date = date;
        this->customer = customer;
        this->rating = rating;
        this->votes = votes;
        this->helpful = helpful;
    }

    void print_values(){
        std::cout << this->date << std::endl;
        std::cout << this->customer << std::endl;
        std::cout << this->rating << std::endl;
        std::cout << this->votes << std::endl;
        std::cout << this->helpful << std::endl;
    }
};

class Product{
public:
    bool discontinued = false;
    std::string ASIN;
    std::string title;
    std::string group;
    int salesrank;
    std::vector<int> categories;
    int total;
    std::vector<Review> reviews;

    void print_values(){
        std::cout << discontinued << std::endl;
        std::cout << ASIN << std::endl;
        std::cout << title << std::endl;
        std::cout << group << std::endl;
        std::cout << salesrank << std::endl;
        std::cout << total << std::endl;
        for(auto i = categories.begin(); i != categories.end(); i++){
            std::cout << *i << std::endl;
        }
        for(auto i = reviews.begin(); i != reviews.end(); i++){
            i->print_values();
        }

    }
};

class Similar{
public:
    std::string ASIN;
    std::vector<std::string> similar_list;

    Similar(std::string ASIN, std::vector<std::string> similar_list){
        this->ASIN = ASIN;
        this->similar_list = similar_list;
    }

};

#endif
