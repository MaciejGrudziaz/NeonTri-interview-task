#ifndef TRANSACTION_STORE_V2
#define TRANSACTION_STORE_V2

#include <unordered_map>
#include <map>
#include <memory>
#include <algorithm>
#include "Database.h"

struct AccountTransactions
{
    std::string accNo;
    std::vector<Transaction> transactions;
    long averageAmount;

    AccountTransactions(const std::string& accNo)
        : accNo(accNo)
        , averageAmount(0.0)
    {}
};

class TransactionStoreV2: public Database
{
public:
    Transaction findTransaction(const std::string &accNo, int txNo) override;
    std::vector<Transaction> findTransactions(const std::string &accNo) override;
    double calculateAverageAmount(const std::string &accNo) override;
    void setTransactions(const std::vector<Transaction> &transactions) override;

private:
    typedef std::unordered_map<std::string, std::unique_ptr<AccountTransactions> > AccountUnOrderMap;
    AccountUnOrderMap accounts;
};

#endif //TRANSACTION_STORE_V2