#include <iostream>
#include <string>
#include <vector>
#include<ctime>
#include <iomanip>
#include <algorithm>
#include <random>
#include <sstream>
using namespace std;

class Transaction{
    private:
    string transactionId;
    time_t timestamp;
    double amount;
    string description;
    string merchantName;
    bool approved;

    public:
    Transaction(double amt,const string& desc, const string& merchant): amount(amt), description(desc),merchantName(merchant),approved(false){
        timestamp=time(nullptr);

        stringstream ss;
        ss<<"TXN"<< setfill('0')<< setw(10)<< rand ()% 1000000000;
        transactionId=ss.str();
    }
};

