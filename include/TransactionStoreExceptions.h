#ifndef TRANSACTION_STORE_EXCEPTIONS
#define TRANSACTION_STORE_EXCEPTIONS

#include <exception>
#include <string>

struct AccountException: public std::exception
{
    std::string accNo;
    AccountException(const std::string& accNo)
        : accNo(accNo)
    {}
};

struct TransactionException: public std::exception
{
    std::string accNo;
    unsigned int txNo;
    TransactionException(const std::string& accNo, unsigned int txNo)
        : txNo(txNo)
    {}
};

#endif //TRANSACTION_STORE_EXCEPTIONS
