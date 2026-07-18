#pragma once
#include <string>
using namespace std;

class MenuItem
{
protected:
    string name;
    string size;
    int    price;
    int    stock;

public:
    MenuItem(string n = "", string s = "", int p = 0, int st = 0)
        : name(n), size(s), price(p), stock(st) {  }

    MenuItem(const MenuItem& other)
        : name(other.name), size(other.size), price(other.price), stock(other.stock) { }

    virtual ~MenuItem() {}

    string getName() const
    {
        return name;
    }
    string getSize()  const 
    {
        return size;
    }
    int getPrice() const 
    {
        return price;
    }
    int getStock() const 
    {
        return stock;
    }

    void decreaseStock()
    {
        if (stock > 0)
            stock--; 
    }
    void increaseStock()
    { 
        stock++;
    }

    virtual string getCategory() const 
    {
        return "General";
    }
    virtual MenuItem* clone() const
    {
        return new MenuItem(*this); 
    }
};

class BBQItem : public MenuItem
{
public:
    BBQItem(string n, string s, int p) : MenuItem(n, s, p) {}

    string getCategory() const override 
    {
        return "BBQ";
    }
    MenuItem* clone() const override 
    {
        return new BBQItem(*this);
    }
};

class DessertItem : public MenuItem
{
public:
    DessertItem(string n, string s, int p) : MenuItem(n, s, p) {}
    string getCategory() const override 
    {
        return "Dessert";
    }
    MenuItem* clone() const override 
    {
        return new DessertItem(*this);
    }
};
