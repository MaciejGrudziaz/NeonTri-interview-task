#include "TransactionStore.h"

Transaction TransactionStore::findTransaction(const std::string &accNo, int txNo) 
{
    if(txNo < 0) throw TransactionException(accNo, txNo); 

    auto accIt = getAccount(accNo);
    
    auto transIt = transactionBinarySearch(accIt, static_cast<unsigned int>(txNo));

    return *(transIt);
}

//retrieving account from collection by account number [complexity: mostly O(1)]
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

//binary search for account's transaction [complexity: O(log(n))]
std::vector<Transaction>::iterator TransactionStore::transactionBinarySearch(const AccountsMap::iterator& accountIt, unsigned int txNo)
{
    auto transIt = std::lower_bound(accountIt->second->transactions.begin(), accountIt->second->transactions.end(), txNo, 
        [](const Transaction& object, int val){ return (object.txNo < val); });
    
    //if not transaction found or found transaction is wrong one throw exception
    if(transIt == accountIt->second->transactions.end() || transIt->txNo != txNo)
        throw TransactionException(accountIt->second->accNo, txNo);

    return transIt;
}

std::vector<Transaction> TransactionStore::findTransactions(const std::string &accNo)
{
    auto accIt = getAccount(accNo);

    return accIt->second->transactions; 
}

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

//loading transactions data to account's collection
void TransactionStore::loadAccountsTransactionData(const std::vector<Transaction> &transactions)
{
    for(auto trans : transactions)
    {
        checkAccountNumber(trans.accNo);

        auto accountIt = accounts.find(trans.accNo);    

        addTransactionToAccount(trans, accountIt);
    }
}

//checking if account number is correct [1-32 alphanum]
void TransactionStore::checkAccountNumber(std::string accNo)
{
    const std::pair<int,int> decCharVal(48,57);         //ascii dec values range for all decimals
    const std::pair<int,int> capitalLetVal(65,90);      //ascii dec values range for capital letters
    const std::pair<int,int> normalLetVal(97,122);      //ascii dec values range for normal letters

    if(accNo.size() > 32 || accNo.size() == 0) throw AccountException(accNo);

    //checking all letters if they're alphanum signs, if not throw exception
    std::for_each(accNo.begin(), accNo.end(), [&](char l){ 
        int charVal = static_cast<int>(l);

        if(charVal < decCharVal.first) throw AccountException(accNo);
        else if(charVal > decCharVal.second && charVal < capitalLetVal.first) throw AccountException(accNo);
        else if(charVal > capitalLetVal.second && charVal < normalLetVal.first) throw AccountException(accNo);
        else if(charVal > normalLetVal.second) throw AccountException(accNo);
    });
}

//adding single transaction to account
void TransactionStore::addTransactionToAccount(const Transaction& transaction, AccountsMap::iterator& accountIt)
{
    if(accountIt == accounts.end())                                 //if account doesn't exist in collection
        accountIt = createAccount(transaction.accNo);               //create one

    if(!isTransactionDuplicate(transaction, accountIt))             //if transaction isn't duplicate
    {
        accountIt->second->transactions.push_back(transaction);     //add it to account's transactions
    }
}

//creating account and adding it to collection
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
        throw AccountException(accNo);          //if account can't be created throw exception
    }  
}

//checking if transaction is a duplicate [if transaction with the same number exists in the account's transactions collection]
bool TransactionStore::isTransactionDuplicate(const Transaction& transaction, const AccountsMap::iterator& accountIt)
{
    auto accountTransactionsIt = std::find_if(accountIt->second->transactions.begin(), accountIt->second->transactions.end(), 
        [&transaction](const Transaction& val){ return (val.txNo == transaction.txNo); });

    return (accountTransactionsIt != accountIt->second->transactions.end());
}

//sorting transactions for all account's by ascending by transaction's number
void TransactionStore::sortTransactionsData()
{
    for(auto accIt = accounts.begin(); accIt != accounts.end(); ++accIt)
    {
        std::sort(accIt->second->transactions.begin(), accIt->second->transactions.end(), 
            [](const Transaction& first, const Transaction& second){ return (first.txNo < second.txNo); });
    }
}

//calculating average of transactions values for all account's
void TransactionStore::calculateAveragesOfTransactions()
{
    double totalAvg, count;

    for(auto accIt = accounts.begin(); accIt != accounts.end(); ++accIt)
    {
        accIt->second->averageAmount = calculateAccountAverage(accIt);
    }
}

//calculating average value of transactions for single account in respect to double type limits
double TransactionStore::calculateAccountAverage(const AccountsMap::iterator& accountIt)
{
    double totalAvg = 0.0, count = static_cast<double>(accountIt->second->transactions.size());

    for(const Transaction& trans : accountIt->second->transactions)
    {
        //if all single transaction amount's values are divided by total count of transactions then the limit of double type will not be exceeded
        totalAvg += (trans.amount / count);         
    }

    return totalAvg;
}
