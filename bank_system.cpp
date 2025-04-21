#include <iostream>
#include <string>
#include <vector>
#include<ctime>
#include <iomanip>
#include <algorithm>
#include <random>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <filesystem>


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
    bool isApproved() const {return approved;}
    double getAmount() const { return amount; }
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
  
    public:
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
    public:
        double accountBalance;
        std::string accountNumber;
        bool overdraftProtection;
    

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
        bool hasOverdraftProtection() const { return overdraftProtection; }
        
        void displayCardInfo() const {
            BankCard::displayCardInfo();
            std::cout << "Account Number: " << accountNumber << std::endl;
            std::cout << "Current Balance: $" << std::fixed << std::setprecision(2) << accountBalance << std::endl;
            std::cout << "Overdraft Protection: " << (overdraftProtection ? "Enabled" : "Disabled") << std::endl;
            std::cout << "------------------------------" << std::endl;
        }
    };

    class CreditCard : public BankCard {
        public:
            double currentBalance;
            double interestRate;
            std::string billingCycle;
            double minimumPayment;
        
       
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
            double getInterestRate() const { return interestRate; }
             
            double getCardLimit() const { return cardLimit; }

            void setCurrentBalance(double balance) { currentBalance = balance; }
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
        
    

        class BankCardSystem {
            public:
                std::vector<BankCard*> cards;
                const string dataFile = "bank_cards.txt";

                ~BankCardSystem() {
                    for (auto card : cards) {
                        delete card;
                    }
                    cards.clear();
                }
            
                void addCard(BankCard* card) {
                    cards.push_back(card);
                    std::cout << "Card successfully added to the system." << std::endl;
                    saveCardsToFile();
                }
            
                BankCard* findCard(const std::string& cardNumber) {
                    for (auto card : cards) {
                        if (card->getCardNumber() == cardNumber) {
                            return card;
                        }
                    }
                    return nullptr;
                }
            
                void displayAllCards() const {
                    if (cards.empty()) {
                        std::cout << "No cards in the system." << std::endl;
                        return;
                    }
            
                    std::cout << "All Cards in the System:" << std::endl;
                    std::cout << "======================" << std::endl;
            
                    for (const auto card : cards) {
                        card->displayCardInfo();
                    }
                }
         
             // NEW: Save all cards to text file
    void saveCardsToFile() {
        cout << "Attempting to save " << cards.size() << " cards to file " << dataFile << endl;
        ofstream outFile(dataFile);
        if (!outFile) {
            cerr << "Error opening file for writing." << endl;
            return;
        }

        for (auto card : cards) {
            if (dynamic_cast<DebitCard*>(card)) {
                DebitCard* dc = dynamic_cast<DebitCard*>(card);
                outFile << "DEBIT|" 
                << dc->getCardNumber() << "|"
                << dc->getCardholderName() << "|"
                << dc->isActive() << "|"
                << dc->getBalance() << "|"
                << (dc->hasOverdraftProtection() ? "1" : "0") << "\n";
            }
            else if (dynamic_cast<CreditCard*>(card)) {
                CreditCard* cc = dynamic_cast<CreditCard*>(card);
                outFile << "CREDIT|" 
                       << cc->getCardNumber() << "|"
                       << cc->getCardholderName() << "|"
                       << cc->isActive() << "|"
                       << cc->getBalance() << "|"
                       << cc->getInterestRate() << "|"
                       << cc->getCardLimit() << "\n";
            }
        }
        outFile.close();
        cout << "Successfully saved cards to file." << endl;
    }

    // NEW: Load cards from text file at startup
    void loadCardsFromFile() {
        ifstream inFile(dataFile);
        if (!inFile) {
            cout << "No existing data file found. Starting fresh." << endl;
            return;
        }

        // Clear existing cards
        for (auto card : cards) {
            delete card;
        }
        cards.clear();

        string line;
        while (getline(inFile, line)) {
            vector<string> tokens;
            string token;
            istringstream tokenStream(line);
            
            while (getline(tokenStream, token, '|')) {
                tokens.push_back(token);
            }

            if (tokens.empty()) continue;

            if (tokens[0] == "DEBIT" && tokens.size() >= 6) {
                try {
                    string cardNum = tokens[1];
                    string name = tokens[2];
                    bool active = tokens[3] == "1";
                    double balance = stod(tokens[4]);
                    bool overdraft = tokens[5] == "1";

                    DebitCard* dc = new DebitCard(name, balance, overdraft);
                    dc->cardNumber = cardNum; // Override generated number
                    if (!active) dc->deactivate();
                    cards.push_back(dc);
                } catch (...) {
                    cerr << "Error parsing debit card data: " << line << endl;
                }
            }
            else if (tokens[0] == "CREDIT" && tokens.size() >= 7) {
                try {
                    string cardNum = tokens[1];
                    string name = tokens[2];
                    bool active = tokens[3] == "1";
                    double balance = stod(tokens[4]);
                    double interestRate = stod(tokens[5]);
                    double limit = stod(tokens[6]);

                    CreditCard* cc = new CreditCard(name, limit, interestRate);
                    cc->currentBalance = balance;
                    cc->cardNumber = cardNum;
                    if (!active) cc->deactivate();
                    cards.push_back(cc);
                } catch (...) {
                    cerr << "Error parsing credit card data: " << line << endl;
                }
            }
        }
        inFile.close();
    }
};

// MODIFIED: Added save calls after operations that modify data
void processMenuChoice(int choice, BankCardSystem& cardSystem) {
    string name, cardNumber;
    double amount, limit, rate, initialBalance;
    bool overdraft;

    if (choice == 1) {
        cin.ignore();
        cout << "Enter cardholder name: ";
        getline(cin, name);

        cout << "Enter initial balance: $";
        cin >> initialBalance;

        cout << "Enable overdraft protection? (1 for Yes, 0 for No): ";
        cin >> overdraft;

        DebitCard* debitCard = new DebitCard(name, initialBalance, overdraft);
        cardSystem.addCard(debitCard);
        debitCard->displayCardInfo();
    }
    else if (choice == 2) {
        cin.ignore();
        cout << "Enter cardholder name: ";
        getline(cin, name);

        cout << "Enter credit limit: $";
        cin >> limit;

        cout << "Enter annual interest rate (as decimal, e.g., 0.1895 for 18.95%): ";
        cin >> rate;

        CreditCard* creditCard = new CreditCard(name, limit, rate);
        cardSystem.addCard(creditCard);
        creditCard->displayCardInfo();
    }
    // ... (other menu options remain similar) ...
    else if (choice == 8) {
        cin.ignore();
        cout << "Enter card number: ";
        getline(cin, cardNumber);

        BankCard* card = cardSystem.findCard(cardNumber);
        if (!card) {
            cout << "Card not found." << endl;
        }
        else {
            int action;
            cout << "Current status: " << (card->isActive() ? "Active" : "Inactive") << endl;
            cout << "Enter 1 to activate or 0 to deactivate: ";
            cin >> action;

            if (action == 1) {
                card->activate();
            }
            else {
                card->deactivate();
            }
            cardSystem.saveCardsToFile();  // Save after status change
            cout << "Card status updated." << endl;
        }
    }
}
            
            // Simple menu-based interface to demonstrate the system
            void displayMenu() {
                std::cout << "\nBank Card System Menu:" << std::endl;
                std::cout << "1. Create a new debit card" << std::endl;
                std::cout << "2. Create a new credit card" << std::endl;
                std::cout << "3. Display all cards" << std::endl;
                std::cout << "4. Process a transaction" << std::endl;
                std::cout << "5. Make a payment to credit card" << std::endl;
                std::cout << "6. Make a deposit to debit card" << std::endl;
                std::cout << "7. View transaction history" << std::endl;
                std::cout << "8. Activate/Deactivate a card" << std::endl;
                std::cout << "9. Exit" << std::endl;
                std::cout << "Enter your choice: ";
            }
            
            int main() {
                // Seed random number generator
                srand(static_cast<unsigned int>(time(nullptr)));
                BankCardSystem cardSystem;
                
                cardSystem.loadCardsFromFile();
          
                int choice;
                std::string name, cardNumber;
                double amount, limit, rate, initialBalance;
                bool overdraft;
            
                do {
                    displayMenu();
                    std::cin >> choice;
            
                    // Handle different menu options
                    if (choice == 1) {
                        std::cin.ignore(); // Clear the newline from the buffer
                        std::cout << "Enter cardholder name: ";
                        std::getline(std::cin, name);
            
                        std::cout << "Enter initial balance: $";
                        std::cin >> initialBalance;
            
                        std::cout << "Enable overdraft protection? (1 for Yes, 0 for No): ";
                        std::cin >> overdraft;
            
                        DebitCard* debitCard = new DebitCard(name, initialBalance, overdraft);
                        cardSystem.addCard(debitCard);
                        debitCard->displayCardInfo();
                    }
                    else if (choice == 2) {
                        std::cin.ignore(); // Clear the newline from the buffer
                        std::cout << "Enter cardholder name: ";
                        std::getline(std::cin, name);
            
                        std::cout << "Enter credit limit: $";
                        std::cin >> limit;
            
                        std::cout << "Enter annual interest rate (as decimal, e.g., 0.1895 for 18.95%): ";
                        std::cin >> rate;
            
                        CreditCard* creditCard = new CreditCard(name, limit, rate);
                        cardSystem.addCard(creditCard);
                        creditCard->displayCardInfo();
                    }
                    else if (choice == 3) {
                        cardSystem.displayAllCards();
                    }
                    else if (choice == 4) {
                        std::cin.ignore(); // Clear the newline from the buffer
                        std::cout << "Enter card number: ";
                        std::getline(std::cin, cardNumber);
            
                        BankCard* card = cardSystem.findCard(cardNumber);
                        if (!card) {
                            std::cout << "Card not found." << std::endl;
                        }
                        else {
                            std::cout << "Enter transaction amount: $";
                            std::cin >> amount;
            
                            std::cin.ignore(); // Clear the newline
                            std::cout << "Enter merchant name: ";
                            std::string merchant;
                            std::getline(std::cin, merchant);
            
                            std::cout << "Enter transaction description: ";
                            std::string description;
                            std::getline(std::cin, description);
            
                            Transaction transaction(amount, description, merchant);
                            card->processTransaction(transaction);
                        }
                    }
                    else if (choice == 5) {
                        std::cin.ignore(); // Clear the newline from the buffer
                        std::cout << "Enter credit card number: ";
                        std::getline(std::cin, cardNumber);
            
                        BankCard* card = cardSystem.findCard(cardNumber);
                        if (!card) {
                            std::cout << "Card not found." << std::endl;
                        }
                        else {
                            CreditCard* creditCard = dynamic_cast<CreditCard*>(card);
                            if (!creditCard) {
                                std::cout << "This is not a credit card." << std::endl;
                            }
                            else {
                                std::cout << "Enter payment amount: $";
                                std::cin >> amount;
            
                                creditCard->makePayment(amount);
                            }
                        }
                    }
                    else if (choice == 6) {
                        std::cin.ignore(); // Clear the newline from the buffer
                        std::cout << "Enter debit card number: ";
                        std::getline(std::cin, cardNumber);
            
                        BankCard* card = cardSystem.findCard(cardNumber);
                        if (!card) {
                            std::cout << "Card not found." << std::endl;
                        }
                        else {
                            DebitCard* debitCard = dynamic_cast<DebitCard*>(card);
                            if (!debitCard) {
                                std::cout << "This is not a debit card." << std::endl;
                            }
                            else {
                                std::cout << "Enter deposit amount: $";
                                std::cin >> amount;
            
                                debitCard->deposit(amount);
                            }
                        }
                    }
                    else if (choice == 7) {
                        std::cin.ignore(); // Clear the newline from the buffer
                        std::cout << "Enter card number: ";
                        std::getline(std::cin, cardNumber);
            
                        BankCard* card = cardSystem.findCard(cardNumber);
                        if (!card) {
                            std::cout << "Card not found." << std::endl;
                        }
                        else {
                            card->displayTransactionHistory();
                        }
                    }
                    else if (choice == 8) {
                        std::cin.ignore(); // Clear the newline from the buffer
                        std::cout << "Enter card number: ";
                        std::getline(std::cin, cardNumber);
            
                        BankCard* card = cardSystem.findCard(cardNumber);
                        if (!card) {
                            std::cout << "Card not found." << std::endl;
                        }
                        else {
                            int action;
                            std::cout << "Current status: " << (card->isActive() ? "Active" : "Inactive") << std::endl;
                            std::cout << "Enter 1 to activate or 0 to deactivate: ";
                            std::cin >> action;
            
                            if (action == 1) {
                                card->activate();
                                std::cout << "Card activated." << std::endl;
                            }
                            else {
                                card->deactivate();
                                std::cout << "Card deactivated." << std::endl;
                            }
                        }
                    }
                    else if (choice == 9) {
                        std::cout << "Exiting the Bank Card System. Goodbye!" << std::endl;
                    }
                    else {
                        std::cout << "Invalid choice. Please try again." << std::endl;
                    }
                } while (choice != 9);
            
                return 0;
            
            }