#include <gtest/gtest.h>
#include <limits>
#include "TransactionStore.h"

static std::vector<Transaction> transactionsSet1 =
        {
                {"35102049000000990200522828", 3517, 3517.00},
                {"35102049000000990200522828", 3515, 3515.00},
                {"6102055610000330200008862",  6103, 6103.00},
                {"56102055610000310200008433", 5610, 5610.00},
                {"56102055610000310200008433", 5610, 5610.00},
                {"56102055610000310200008433", 5611, 5611.00},
                {"4830600000000200003900",     4830, 4830.00},
                {"7230600000000200006669",     7239, 7239.00},
                {"7230600000000200006669",     7238, 7238.00},
                {"7230600000000200006669",     7237, 7237.00},
                {"7230600000000200006669",     7236, 7236.00},
                {"7230600000000200006669",     7235, 7235.00},
                {"7230600000000200006669",     7234, 7234.00},
                {"7230600000000200006669",     7234, 7234.00},
                {"50102055581111101998100048", 501,  501.00},
                {"50102055581111101998100048", 503,  503.00},
                {"50102055581111101998100048", 501,  501.00},
        };

static std::vector<Transaction> transactionsSet2 =
        {
                {"7230600000000200006669",     355,  235.50},
                {"7230600000000200006669",     3424, -2512.54},
                {"7230600000000200006669",     241,  451235.21},
                {"35200442300000123",          352,  3242.12},
                {"35200442300000123",          352,  2321.50},
                {"35200442300000123",          322,  -325.13},
                {"9008420017418290055",        0,    8.00},
                {"9008420017418290055",        1,    -28.00},
                {"9008420017418290055",        2,    9.00},
                {"882346125300012378005",      445,  std::numeric_limits<double>::max()/1.5},
                {"882346125300012378005",      832,  std::numeric_limits<double>::max()/1.5},
                {"882346125300012378005",      54,   std::numeric_limits<double>::max()/3.0},
                {"882346125300012378005",      497,  std::numeric_limits<double>::max()/3.0},
                {"34600034880023477100324124340001",      47,   std::numeric_limits<double>::min()/2.0},
                {"34600034880023477100324124340001",      65,   std::numeric_limits<double>::min()/2.0},
                {"34600034880023477100324124340001",      44,   std::numeric_limits<double>::min()/2.0}
        };

TEST(txTests, findTransaction) {
    std::unique_ptr<Database> db = std::make_unique<TransactionStore>();
    db->setTransactions(transactionsSet1);

    Transaction transaction = db->findTransaction("56102055610000310200008433", 5611);

    EXPECT_EQ(5611.00, transaction.amount);
    EXPECT_ANY_THROW(db->findTransaction("invalid", 0));
}

TEST(txTests, findTransactions)
{
    std::unique_ptr<Database> db = std::make_unique<TransactionStore>();
    db->setTransactions(transactionsSet1);
    auto t = db->findTransactions("35102049000000990200522828");

    EXPECT_EQ(2, t.size());
    EXPECT_EQ(3515, t[0].txNo);
    EXPECT_EQ(3517, t[1].txNo);

    auto t2 = db->findTransactions("56102055610000310200008433");
    EXPECT_EQ(2, t2.size());
    EXPECT_EQ(5610, t2[0].txNo);
    EXPECT_EQ(5611, t2[1].txNo);
}

TEST(txTests, findWrongTransactions)
{
    std::unique_ptr<Database> db = std::make_unique<TransactionStore>();
    db->setTransactions(transactionsSet2);
    
    EXPECT_ANY_THROW(db->findTransactions("35923AFFA00035"));

    EXPECT_ANY_THROW(db->findTransactions("88234612530001237800"));
}

TEST(txTests, calculateAverageAmount) {
    std::unique_ptr<Database> db = std::make_unique<TransactionStore>();
    db->setTransactions(transactionsSet1);

    double avg = db->calculateAverageAmount("7230600000000200006669");

    EXPECT_EQ(723650, (int)(avg * 100));
}

TEST(txTests, calculateWrongAverageAmount) {
    std::unique_ptr<Database> db = std::make_unique<TransactionStore>();
    db->setTransactions(transactionsSet1);

    EXPECT_ANY_THROW(db->calculateAverageAmount("230600000000200006669"));

    EXPECT_ANY_THROW(db->calculateAverageAmount("SDFSD2523235DDF"));
}

TEST(txTests, calculateAverageAmountLimits) 
{
    std::unique_ptr<Database> db = std::make_unique<TransactionStore>();
    db->setTransactions(transactionsSet2);

    double avg = db->calculateAverageAmount("882346125300012378005");

    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::max()/2.0, avg);

    avg = db->calculateAverageAmount("34600034880023477100324124340001");

    EXPECT_DOUBLE_EQ(std::numeric_limits<double>::min()/2.0, avg);
}

TEST(txTests, databseReload)
{
    std::unique_ptr<Database> db = std::make_unique<TransactionStore>();
    db->setTransactions(transactionsSet1);

    double avg = db->calculateAverageAmount("7230600000000200006669");
    EXPECT_EQ(723650, static_cast<int>(avg * 100.0));

    auto trans = db->findTransaction("50102055581111101998100048", 501);
    EXPECT_EQ(50100, static_cast<int>(trans.amount * 100.0));

    db->setTransactions(transactionsSet2);

    avg = db->calculateAverageAmount("7230600000000200006669");
    EXPECT_EQ(14965272, static_cast<int>(avg * 100.0));

    EXPECT_ANY_THROW(db->findTransaction("50102055581111101998100048", 501));

    trans = db->findTransaction("9008420017418290055", 0);
    EXPECT_EQ(800, static_cast<int>(trans.amount * 100.0));
}

TEST(txTests, wrongAccountNumbers)
{
    std::unique_ptr<Database> db = std::make_unique<TransactionStore>();

    std::vector<Transaction> transactionsSetTemp(1);
    transactionsSetTemp[0] = {"", 0, 0};

    EXPECT_ANY_THROW(db->setTransactions(transactionsSetTemp));

    transactionsSetTemp[0] = {"$23^4m*fs@!455", 0, 0};

    EXPECT_ANY_THROW(db->setTransactions(transactionsSetTemp));

    transactionsSetTemp[0] = {"FA42350 0239 SD342", 0, 0};

    EXPECT_ANY_THROW(db->setTransactions(transactionsSetTemp));

    transactionsSetTemp[0] = {"5342374239DFSD003248dfsf6385MM01G", 0, 0};

    EXPECT_ANY_THROW(db->setTransactions(transactionsSetTemp));

    transactionsSetTemp[0] = {"2353452342ADFSDF2534", 0, 0};
    transactionsSetTemp.push_back({"dsdf32525 435345#$6", 0, 0});

    EXPECT_ANY_THROW(db->setTransactions(transactionsSetTemp));
}
