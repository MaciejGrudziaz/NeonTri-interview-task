#ifndef TRANSACTION_STORE_V2
#define TRANSACTION_STORE_V2

#include <unordered_map>
#include <map>
#include <memory>
#include <algorithm>
#include <exception>
#include <limits>
#include "Database.h"
#include "TransactionStoreExceptions.h"

struct AccountTransactions
{
    std::string accNo;
    std::vector<Transaction> transactions;
    double averageAmount;

    AccountTransactions(const std::string& accNo)
        : accNo(accNo)
        , averageAmount(0.0)
    {}
};

class TransactionStore: public Database
{
public:
    Transaction findTransaction(const std::string &accNo, int txNo) override;
    std::vector<Transaction> findTransactions(const std::string &accNo) override;
    double calculateAverageAmount(const std::string &accNo) override;
    void setTransactions(const std::vector<Transaction> &transactions) override;

private:
    typedef std::unordered_map<std::string, std::unique_ptr<AccountTransactions> > AccountsMap;
    AccountsMap accounts;

    AccountsMap::iterator getAccount(const std::string& accNo);
    std::vector<Transaction>::iterator transactionBinarySearch(const AccountsMap::iterator& accountIt, unsigned int txNo);

    void loadAccountsTransactionData(const std::vector<Transaction> &transactions);
    void checkAccountNumber(std::string accNo);
    void addTransactionToAccount(const Transaction& transaction, AccountsMap::iterator& accounIt);
    AccountsMap::iterator createAccount(const std::string& accNo);
    bool isTransactionDuplicate(const Transaction& transaction, const AccountsMap::iterator& accountIt);

    void sortTransactionsData();
    void calculateAveragesOfTransactions();
    double calculateAccountAverage(const AccountsMap::iterator& accountIt);
};



#endif //TRANSACTION_STORE_V2