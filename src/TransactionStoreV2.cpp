#include "TransactionStoreV2.h"

//O(log(n)) for binary_search
Transaction TransactionStoreV2::findTransaction(const std::string &accNo, int txNo) 
{
    auto accIt = accounts.find(accNo);
    if(accIt != accounts.end())
    {
        auto transIt = std::lower_bound(accIt->second->transactions.begin(), accIt->second->transactions.end(), txNo, 
        [](const Transaction& first, int second){ return (first.txNo < second); });
        
        if(transIt != accIt->second->transactions.end() && transIt->txNo == txNo)
        {
            return *(transIt);
        }
        else
        {
            //throw exception
            throw std::exception();
        }
    }
    else
    {
        //throw exception
        throw std::exception();
    }    
}

//O(1)
std::vector<Transaction> TransactionStoreV2::findTransactions(const std::string &accNo)
{
    auto accIt = accounts.find(accNo);
    if(accIt != accounts.end())
    {
        return accIt->second->transactions;
    }
    else
    {
        //throw exception
        throw std::exception();
    }      
}

//O(1)
double TransactionStoreV2::calculateAverageAmount(const std::string &accNo) 
{
    auto accIt = accounts.find(accNo);
    if(accIt != accounts.end())
    {
        return static_cast<double>(accIt->second->averageAmount / 100.0);
    }
    else
    {
        //throw exception
        throw std::exception();
    }
}

//TODO: pomijanie duplikatów
void TransactionStoreV2::setTransactions(const std::vector<Transaction> &transactions)
{
    for(auto trans : transactions)
    {
        auto accIt = accounts.find(trans.accNo);
        if(accIt != accounts.end())
        {
            accIt->second->transactions.push_back(trans);
            accIt->second->averageAmount += static_cast<long>(trans.amount * 100.0);
        }
        else 
        {
            auto accInsertType = accounts.insert(AccountUnOrderMap::value_type(trans.accNo, std::make_unique<AccountTransactions>(trans.accNo)));
            if(accInsertType.second)
            {
                accInsertType.first->second->transactions.push_back(trans);
                accInsertType.first->second->averageAmount += static_cast<long>(trans.amount * 100.0);
            }
        }
    }

    for(auto accIt = accounts.begin(); accIt != accounts.end(); ++accIt)
    {
        accIt->second->averageAmount /= static_cast<double>(accIt->second->transactions.size());
        std::sort(accIt->second->transactions.begin(), accIt->second->transactions.end(), 
        [](const Transaction& first, const Transaction& second){ return (first.txNo < second.txNo); });
    }
}
