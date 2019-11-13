#include "TransactionStore.h"

//O(log(n)) for binary_search
Transaction TransactionStore::findTransaction(const std::string &accNo, int txNo) 
{
    if(txNo < 0) throw TransactionException(accNo, txNo); 

    auto accIt = getAccount(accNo);
    
    auto transIt = transactionsBinarySearch(accIt, static_cast<unsigned int>(txNo));

    return *(transIt);
}

TransactionStore::AccountsMap::iterator TransactionStore::getAccount(const std::string& accNo)
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

std::vector<Transaction>::iterator TransactionStore::transactionsBinarySearch(const AccountsMap::iterator& accountIt, unsigned int txNo)
{
    auto transIt = std::lower_bound(accountIt->second->transactions.begin(), accountIt->second->transactions.end(), txNo, 
        [](const Transaction& object, int val){ return (object.txNo < val); });
    
    if(transIt == accountIt->second->transactions.end() || transIt->txNo != txNo)
        throw TransactionException(accountIt->second->accNo, txNo);

    return transIt;
}

//O(1)
std::vector<Transaction> TransactionStore::findTransactions(const std::string &accNo)
{
    auto accIt = getAccount(accNo);

    return accIt->second->transactions; 
}

//O(1)
double TransactionStore::calculateAverageAmount(const std::string &accNo) 
{
    auto accIt = getAccount(accNo);

    return accIt->second->averageAmount;
}

void TransactionStore::setTransactions(const std::vector<Transaction> &transactions)
{
    accounts.clear();

    loadAccountsTransactionData(transactions);

    sortTransactionsData();

    calculateAveragesOfTransactions();
}


void TransactionStore::loadAccountsTransactionData(const std::vector<Transaction> &transactions)
{
    for(auto trans : transactions)
    {
        checkAccountNumber(trans.accNo);

        auto accountIt = accounts.find(trans.accNo);

        addTransactionToAccount(trans, accountIt);
    }
}

void TransactionStore::checkAccountNumber(std::string accNo)
{
    const std::pair<int,int> decCharVal(48,57);
    const std::pair<int,int> capitalLetVal(65,90);
    const std::pair<int,int> normalLetVal(97,122);

    if(accNo.size() > 32 || accNo.size() == 0) throw AccountException(accNo);

    std::for_each(accNo.begin(), accNo.end(), [&](char l){ 
        int charVal = static_cast<int>(l);

        if(charVal < decCharVal.first) throw AccountException(accNo);
        else if(charVal > decCharVal.second && charVal < capitalLetVal.first) throw AccountException(accNo);
        else if(charVal > capitalLetVal.second && charVal < normalLetVal.first) throw AccountException(accNo);
        else if(charVal > normalLetVal.second) throw AccountException(accNo);
    });
}

void TransactionStore::addTransactionToAccount(const Transaction& transaction, AccountsMap::iterator& accountIt)
{
    if(accountIt == accounts.end())
        accountIt = createAccount(transaction.accNo);

    if(!isTransactionDuplicate(transaction, accountIt))
    {
        accountIt->second->transactions.push_back(transaction);
    }
}

TransactionStore::AccountsMap::iterator TransactionStore::createAccount(const std::string& accNo)
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

bool TransactionStore::isTransactionDuplicate(const Transaction& transaction, const AccountsMap::iterator& accountIt)
{
    auto accountTransactionsIt = std::find_if(accountIt->second->transactions.begin(), accountIt->second->transactions.end(), 
        [&transaction](const Transaction& val){ return (val.txNo == transaction.txNo); });

    return (accountTransactionsIt != accountIt->second->transactions.end());
}

void TransactionStore::sortTransactionsData()
{
    for(auto accIt = accounts.begin(); accIt != accounts.end(); ++accIt)
    {
        std::sort(accIt->second->transactions.begin(), accIt->second->transactions.end(), 
            [](const Transaction& first, const Transaction& second){ return (first.txNo < second.txNo); });
    }
}

void TransactionStore::calculateAveragesOfTransactions()
{
    double totalAvg, count;

    for(auto accIt = accounts.begin(); accIt != accounts.end(); ++accIt)
    {
        accIt->second->averageAmount = calculateAccountAverage(accIt);
    }
}

double TransactionStore::calculateAccountAverage(const AccountsMap::iterator& accountIt)
{
    double totalAvg = 0.0, count = static_cast<double>(accountIt->second->transactions.size());

    for(const Transaction& trans : accountIt->second->transactions)
    {
        totalAvg += (trans.amount / count);
    }

    return totalAvg;
}
