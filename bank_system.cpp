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

    void approve() {approved = true;}
    void decline() {approved = false;}
    bool isApproved() const {return amount;}
    std::string getDescription() const {return description;} 
    std::string getMerchantName() const { return merchantName;}
    std::string getTransactionId() const {return transactionId;}

    std::string getFormattedDate() const {
        char buffer[26];
        struct tm* timeinfo = localtime(&timestamp);
        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", timeinfo);
        return std::string(buffer);
    }

    void displayTransaction() const {
        std::cout << "Transaction ID: " << transactionId << std::endl;
        std::cout << "Date: " << getFormattedDate() << std::endl;
        std::cout << "Amount: $" << std::fixed << std::setprecision(2) << amount << std::endl;
        std::cout << "Description: " << description << std::endl;
        std::cout << "Merchant: " << merchantName << std::endl;
        std::cout << "Status: " << (approved ? "Approved" : "Declined") << std::endl;
        std::cout << "------------------------------" << std::endl;
    }
};

class BankCard{
    protected:
    std::string cardNumber;
    std::string cardholderName;
    std::string expirationDate;
    int cvv;
    double cardLimit;
    bool active;
    std::vector<Transaction> transactionHistory;

    std::string generateCardNumber() const {
        std::stringstream ss;
        for (int i = 0; i < 4; ++i){
            if(i > 0) ss <<"-";
            ss << std::setfill('0') << std::setw(4) << (rand() % 10000);
        }
        return ss.str();
    }

    public:
    BankCard(const std::string& name, double limit) : cardholderName(name), cardLimit(limit), active(true){
        cardNumber = generateCardNumber();
        cvv = 100 + (rand() % 900);

        std::time_t t = std::time(nullptr);
        struct tm* timeinfo = localtime(&t);
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << (timeinfo->tm_mon + 1) << "/"
           << (timeinfo->tm_year + 1900 + 3) % 100;
        expirationDate = ss.str();
    }
    virtual ~BankCard() {}

    bool isActive() const {return active;}
    void activate() { active = true; }
    void deactivate() { active = false; }
    
    std::string getCardNumber() const { return cardNumber; }
    std::string getCardholderName() const { return cardholderName; }

    void displayCardInfo() const {
        std::cout << "Card Type: " << getCardType() << std::endl;
        std::cout << "Card Number: " << cardNumber << std::endl;
        std::cout << "Cardholder: " << cardholderName << std::endl;
        std::cout << "Expiration Date: " << expirationDate << std::endl;
        std::cout << "Status: " << (active ? "Active" : "Inactive") << std::endl;
        std::cout << "------------------------------" << std::endl;
    }
    
    virtual bool processTransaction(Transaction& transaction) {
        if (!active) {
            std::cout << "Card is inactive. Transaction declined." << std::endl;
            transaction.decline();
            return false;
        }
        
        transactionHistory.push_back(transaction);
        return true;
    }

    void displayTransactionHistory() const {
        if (transactionHistory.empty()) {
            std::cout << "No transaction history available." << std::endl;
            return;
        }
        
        std::cout << "Transaction History for card ending with " 
                  << cardNumber.substr(cardNumber.length() - 4) << ":" << std::endl;
        std::cout << "------------------------------" << std::endl;
        
        for (const auto& transaction : transactionHistory) {
            transaction.displayTransaction();
        }
    }
    
    virtual std::string getCardType() const {
        return "Generic Bank Card";
    }
};

class DebitCard : public BankCard {
    private:
        double accountBalance;
        std::string accountNumber;
        bool overdraftProtection;
    
    public:
        DebitCard(const std::string& name, double initialBalance, bool overdraft = false)
            : BankCard(name, initialBalance), accountBalance(initialBalance), 
              overdraftProtection(overdraft) {
            std::stringstream ss;
            ss << "ACCT" << std::setfill('0') << std::setw(8) << (rand() % 100000000);
            accountNumber = ss.str();
        }
        
        double getBalance() const { return accountBalance; }
        
        void deposit(double amount) {
            if (amount > 0) {
                accountBalance += amount;
                std::cout << "Deposited $" << std::fixed << std::setprecision(2) << amount 
                          << ". New balance: $" << accountBalance << std::endl;
            } else {
                std::cout << "Invalid deposit amount." << std::endl;
            }
        }
        
        bool withdraw(double amount) {
            if (amount <= 0) {
                std::cout << "Invalid withdrawal amount." << std::endl;
                return false;
            }
            
            if (amount > accountBalance && !overdraftProtection) {
                std::cout << "Insufficient funds. Withdrawal declined." << std::endl;
                return false;
            }
            
            accountBalance -= amount;
            std::cout << "Withdrew $" << std::fixed << std::setprecision(2) << amount 
                      << ". New balance: $" << accountBalance << std::endl;
                      
            if (accountBalance < 0) {
                std::cout << "Warning: Account is overdrawn." << std::endl;
            }
            
            return true;
        }

        std::string getCardType() const override {
            return "Debit Card";
        }
        
        void displayCardInfo() const {
            BankCard::displayCardInfo();
            std::cout << "Account Number: " << accountNumber << std::endl;
            std::cout << "Current Balance: $" << std::fixed << std::setprecision(2) << accountBalance << std::endl;
            std::cout << "Overdraft Protection: " << (overdraftProtection ? "Enabled" : "Disabled") << std::endl;
            std::cout << "------------------------------" << std::endl;
        }
    };

    class CreditCard : public BankCard {
        private:
            double currentBalance;
            double interestRate;
            std::string billingCycle;
            double minimumPayment;
        
        public:
            CreditCard(const std::string& name, double limit, double rate)
                : BankCard(name, limit), currentBalance(0), interestRate(rate) {
                billingCycle = "Monthly";
                minimumPayment = 25.0;
            }
            
            double getBalance() const { return currentBalance; }
            double getAvailableCredit() const { return cardLimit - currentBalance; }
            
            void makePayment(double amount) {
                if (amount <= 0) {
                    std::cout << "Invalid payment amount." << std::endl;
                    return;
                }
                
                currentBalance -= amount;
                if (currentBalance < 0) currentBalance = 0;
                
                std::cout << "Payment of $" << std::fixed << std::setprecision(2) << amount 
                          << " applied. New balance: $" << currentBalance << std::endl;
            }
            
            void applyInterest() {
                double interest = currentBalance * (interestRate / 12); // Monthly interest
                currentBalance += interest;
                std::cout << "Interest of $" << std::fixed << std::setprecision(2) << interest 
                          << " applied. New balance: $" << currentBalance << std::endl;
            }
            
            bool processTransaction(Transaction& transaction) override {
                double amount = transaction.getAmount();
                
                if (!BankCard::processTransaction(transaction)) {
                    return false;
                }
                
                if (currentBalance + amount > cardLimit) {
                    std::cout << "Transaction exceeds credit limit. Transaction declined." << std::endl;
                    transaction.decline();
                    return false;
                }
                
                currentBalance += amount;
                transaction.approve();
                std::cout << "Transaction approved. New balance: $" 
                          << std::fixed << std::setprecision(2) << currentBalance 
                          << ". Available credit: $" << (cardLimit - currentBalance) << std::endl;
                
                return true;
            }
            
            std::string getCardType() const override {
                return "Credit Card";
            }
            
            void displayCardInfo() const {
                BankCard::displayCardInfo();
                std::cout << "Credit Limit: $" << std::fixed << std::setprecision(2) << cardLimit << std::endl;
                std::cout << "Current Balance: $" << currentBalance << std::endl;
                std::cout << "Available Credit: $" << (cardLimit - currentBalance) << std::endl;
                std::cout << "Interest Rate: " << (interestRate * 100) << "%" << std::endl;
                std::cout << "Minimum Payment: $" << minimumPayment << std::endl;
                std::cout << "------------------------------" << std::endl;
            }
        };
        
    

