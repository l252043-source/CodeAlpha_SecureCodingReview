#pragma once
#include <fstream>
#include <string>
using namespace std;

class StockManager
{
private:
    string* names;   
    int* qty;       
    int count;
    int capacity;

public:

    StockManager()
    {
        capacity = 100;
        count = 0;

        names = new string[capacity];
        qty = new int[capacity];
    }

    ~StockManager()
    {
        delete[] names;
        delete[] qty;
    }

    void loadStock()
    {
        ifstream file("stock.txt");
        string line;

        count = 0;

        while (getline(file, line))
        {
            if (line == "")
                continue;

            int pos = line.find(',');
            if (pos == -1)
                continue;

            string name = line.substr(0, pos);
            string qtyStr = line.substr(pos + 1);

            int value = 0;
            for (int i = 0; i < qtyStr.length(); i++)
            {
                if (qtyStr[i] >= '0' && qtyStr[i] <= '9')
                {
                    value = value * 10 + (qtyStr[i] - '0');
                }
            }

            // trim spaces
            if (name[0] == ' ')
                name = name.substr(1);

            if (name[name.length() - 1] == ' ')
                name = name.substr(0, name.length() - 1);

            // store in dynamic arrays
            if (count < capacity)
            {
                names[count] = name;
                qty[count] = value;
                count++;
            }
        }

        file.close();
    }

    bool reduceStock(string name, int quantity)
    {
        for (int i = 0; i < count; i++)
        {
            if (names[i] == name)
            {
                if (qty[i] >= quantity)
                {
                    qty[i] -= quantity;
                    saveStock();
                    return true;
                }
                return false;
            }
        }
        return false;
    }

    void saveStock()
    {
        ofstream file("stock.txt");

        for (int i = 0; i < count; i++)
        {
            file << names[i] << "," << qty[i] << "\n";
        }

        file.close();
    }

    int getStock(string name)
    {
        for (int i = 0; i < count; i++)
        {
            if (names[i] == name)
                return qty[i];
        }
        return 0;
    }
};