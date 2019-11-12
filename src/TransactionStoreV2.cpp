#include "TransactionStoreV2.h"

//O(log(n)) for binary_search
Transaction TransactionStoreV2::findTransaction(const std::string &accNo, int txNo) 
{
    if(txNo < 0) throw TransactionException(accNo, txNo); 

    auto accIt = getAccount(accNo);
    
    auto transIt = transactionsBinarySearch(accIt, static_cast<unsigned int>(txNo));

    return *(transIt);
}

TransactionStoreV2::AccountsMap::iterator TransactionStoreV2::getAccount(const std::string& accNo)
{
    auto accIt = accounts.find(accNo);

    if(accIt != accounts.end())
    {
        return accIt;
    }
    else
    {
        throw AccountException(accNo);
    }    
}

std::vector<Transaction>::iterator TransactionStoreV2::transactionsBinarySearch(AccountsMap::iterator accountIt, unsigned int txNo)
{
    auto transIt = std::lower_bound(accountIt->second->transactions.begin(), accountIt->second->transactions.end(), txNo, 
        [](const Transaction& object, int val){ return (object.txNo < val); });
    
    if(transIt == accountIt->second->transactions.end() || transIt->txNo != txNo)
        throw TransactionException(accountIt->second->accNo, txNo);

    return transIt;
}

//O(1)
std::vector<Transaction> TransactionStoreV2::findTransactions(const std::string &accNo)
{
    auto accIt = getAccount(accNo);

    return accIt->second->transactions; 
}

//O(1)
double TransactionStoreV2::calculateAverageAmount(const std::string &accNo) 
{
    auto accIt = getAccount(accNo);

    return accIt->second->averageAmount;
}

void TransactionStoreV2::setTransactions(const std::vector<Transaction> &transactions)
{
    accounts.clear();

    loadAccountsTransactionData(transactions);

    sortTransactionsData();

    calculateAverageOfTransactions();
}


void TransactionStoreV2::loadAccountsTransactionData(const std::vector<Transaction> &transactions)
{
    for(auto trans : transactions)
    {
        auto accountIt = accounts.find(trans.accNo);

        addTransactionToAccount(trans, accountIt);
    }
}

void TransactionStoreV2::addTransactionToAccount(const Transaction& transaction, AccountsMap::iterator accountIt)
{
    if(accountIt == accounts.end())
        accountIt = createAccount(transaction.accNo);

    if(!isTransactionDuplicate(transaction, accountIt))
    {
        accountIt->second->transactions.push_back(transaction);
    }
}

TransactionStoreV2::AccountsMap::iterator TransactionStoreV2::createAccount(const std::string& accNo)
{
    auto accInsertType = accounts.insert(AccountsMap::value_type(accNo, 
        std::make_unique<AccountTransactions>(accNo)));

    if(accInsertType.second)
    {
        return accInsertType.first;
    }
    else
    {
        throw AccountException(accNo);
    }  
}

bool TransactionStoreV2::isTransactionDuplicate(const Transaction& transaction, AccountsMap::iterator accountIt)
{
    auto accountTransactionsIt = std::find_if(accountIt->second->transactions.begin(), accountIt->second->transactions.end(), 
        [&transaction](const Transaction& val){ return (val.txNo == transaction.txNo); });

    return (accountTransactionsIt != accountIt->second->transactions.end());
}

void TransactionStoreV2::sortTransactionsData()
{
    for(auto accIt = accounts.begin(); accIt != accounts.end(); ++accIt)
    {
        std::sort(accIt->second->transactions.begin(), accIt->second->transactions.end(), 
            [](const Transaction& first, const Transaction& second){ return (first.txNo < second.txNo); });
    }
}

void TransactionStoreV2::calculateAverageOfTransactions()
{
    std::vector<std::shared_ptr<double> > partialAverages;
    std::shared_ptr<double> currAvg;
    for(auto accIt = accounts.begin(); accIt != accounts.end(); ++accIt)
    {
        partialAverages.clear();
        currAvg = std::make_shared<double>(0.0);
        partialAverages.push_back(currAvg);

        for(auto trans : accIt->second->transactions)
        {
            if((*currAvg) + trans.amount < std::numeric_limits<double>::max() && (*currAvg) > std::numeric_limits<double>::min())
            {
                (*currAvg) += trans.amount;
            }
            else
            {
                currAvg = std::make_shared<double>(0.0);
                (*currAvg) = trans.amount;

                partialAverages.push_back(currAvg);
            }
        }

        double totalAvg = 0.0;
        for(auto partAvg : partialAverages)
        {
            totalAvg += ((*partAvg) / accIt->second->transactions.size());
        }

        accIt->second->averageAmount = totalAvg;
    }
}
