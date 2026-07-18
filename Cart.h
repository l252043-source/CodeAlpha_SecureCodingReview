#pragma once
#include "MenuItem.h"
#include <iostream>
#include <fstream>
using namespace std;

class Cart
{
private:
    static const int maxCart = 50;
    static int       totalOrders;

    MenuItem* items[maxCart];
    int       qty[maxCart];
    int       count;

public:
    Cart() : count(0)
    {
        for (int i = 0; i < maxCart; i++)
        {
            items[i] = nullptr;
            qty[i] = 0;
        }
    }

    ~Cart()
    {
        clear();
    }

    Cart(const Cart& other) : count(other.count)
    {
        for (int i = 0; i < maxCart; i++)
        {
            items[i] = nullptr;
            qty[i] = 0;
        }
        for (int i = 0; i < count; i++)
        {
            if (other.items[i])
                items[i] = other.items[i]->clone();
            qty[i] = other.qty[i];
        }
    }

    Cart& operator=(const Cart& other)
    {
        if (this == &other) return *this;
        clear();
        count = other.count;
        for (int i = 0; i < count; i++)
        {
            items[i] = other.items[i]->clone();
            qty[i] = other.qty[i];
        }
        return *this;
    }

    void add(MenuItem* item)
    {
        for (int i = 0; i < count; i++)
        {
            if (items[i]->getName() == item->getName() &&
                items[i]->getSize() == item->getSize())
            {
                qty[i]++;
                return;
            }
        }
        if (count < maxCart)
        {
            items[count] = item->clone();
            qty[count] = 1;
            count++;
        }
    }

    void remove(int index)
    {
        if (index < 0 || index >= count) return;

        if (qty[index] > 1)
        {
            qty[index]--;
            return;
        }

        delete items[index];
        for (int i = index; i < count - 1; i++)
        {
            items[i] = items[i + 1];
            qty[i] = qty[i + 1];
        }
        items[count - 1] = nullptr;
        count--;
    }

    void clear()
    {
        for (int i = 0; i < count; i++)
        {
            delete items[i];
            items[i] = nullptr;
        }
        count = 0;
    }

    void exportReceipt()
    {
        totalOrders++;
        ofstream file("receipt.txt", ios::app);
        if (!file.is_open()) return;

        file << "========= DINE 360 RECEIPT =========\n\n";
        for (int i = 0; i < count; i++)
        {
            file << items[i]->getName()
                << " x" << qty[i]
                << " = Rs. " << items[i]->getPrice() * qty[i]
                << "\n";
        }
        file << "\n----------------------------------\n";
        file << "TOTAL: Rs. " << getTotal() << "\n";
        file << "Orders served: " << totalOrders << "\n";
        file << "==================================\n\n";
        file.close();
    }

    int getTotal() const
    {
        int total = 0;
        for (int i = 0; i < count; i++)
            total += items[i]->getPrice() * qty[i];
        return total;
    }

    int getQuantity(const string& name, const string& size = "") const
    {
        for (int i = 0; i < count; i++)
        {
            if (items[i]->getName() == name)
            {
                if (size.empty() || items[i]->getSize() == size)
                    return qty[i];
            }
        }
        return 0;
    }

    int getCount() const 
    { 
        return count;
    }
    MenuItem* getItem(int i) const 
    {
        return items[i];
    }
    int getQty(int i) const 
    {
        return qty[i];
    }

    MenuItem* operator[](int i) 
    {
        return items[i]; 
    }

    static int getTotalOrders()
    {
        return totalOrders;
    }

    friend ostream& operator<<(ostream& out, const Cart& c);
};

int Cart::totalOrders = 0;

ostream& operator<<(ostream& out, const Cart& c)
{
    for (int i = 0; i < c.count; i++)
        out << c.items[i]->getName() << " x" << c.qty[i] << "\n";
    return out;
}