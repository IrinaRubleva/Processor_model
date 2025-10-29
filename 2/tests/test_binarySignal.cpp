#include <gtest/gtest.h>
#include "binarySignal.h"
#include <random>



TEST(BinarySignalTest, DefaultConstructors){
    BinarySignal signal;
    EXPECT_EQ(signal.getSize(), 0);
    EXPECT_EQ(signal.getTotalDuration(), 0);
} 

TEST(BinarySignalTest, StringConstructors){
    BinarySignal signal("1110010");
    size_t size = signal.getSize();
    EXPECT_EQ(signal[3], false);
    EXPECT_EQ(size, 4);
    EXPECT_EQ(signal.getTotalDuration(), 7);
}


TEST(BinarySignalTest, EmptyStringConstructors){
    BinarySignal signal2("");
    EXPECT_EQ(signal2.getSize(), 0);
}

TEST(BinarySignalTest, SingleConstructors){
    BinarySignal signal(false, 5);
    EXPECT_EQ(signal[3], false);
    EXPECT_EQ(signal.getSize(), 1);
}

TEST(BinarySignalTest, ThrowsConstructors){
    EXPECT_THROW(BinarySignal signal(false, 0), std::invalid_argument);
}



TEST(BinarySignalTest, EqOperator){
    BinarySignal signal("1001");
    BinarySignal res;
    res = signal;
    EXPECT_EQ(res.getSize(), signal.getSize());

     for (std::size_t i = 0; i < signal.getTotalDuration(); ++i){
        EXPECT_EQ(res[i], signal[i]);
     }
}


TEST(BinarySignalTest, PlusEqOperator){
    BinarySignal signal1("0001");
    BinarySignal signal2("01");

    signal1 += signal2;
    EXPECT_EQ(signal1.getTotalDuration(), 6);
    EXPECT_EQ(signal1[0], false);
    EXPECT_EQ(signal1[1], false);
    EXPECT_EQ(signal1[2], false);
    EXPECT_EQ(signal1[3], true);
    EXPECT_EQ(signal1[4], false);
    EXPECT_EQ(signal1[5], true);
}

TEST(BinarySignalTest, DublicateOperator){
    std::random_device rd;
    unsigned int seed = rd();
    std::mt19937 gen(seed);
    
    try {
        BinarySignal signal(true, 5);
        
        BinarySignal result1 = signal * 3;
        EXPECT_EQ(result1.getSize(), 1); 
        EXPECT_EQ(result1.getTotalDuration(), 15);
        
        BinarySignal result2 = 3 * signal;
        EXPECT_EQ(result2.getSize(), 1);
        EXPECT_EQ(result2.getTotalDuration(), 15);
        
        EXPECT_EQ(signal.getSize(), 1);
        EXPECT_EQ(signal.getTotalDuration(), 5);
        
        std::uniform_int_distribution<> dis(0, result1.getTotalDuration() - 1);
        
        for (int i = 0; i < 10; ++i) {
            int randomTime = dis(gen);
            bool level = result1[randomTime];
            EXPECT_TRUE(level == true);
        }
    }
    catch (...) {
        std::cout << "Test failed with seed: " << seed << std::endl;
        throw;
    }
}


TEST(BinarySignalTest, Inversion) {
    BinarySignal signal("1001");
    signal.inversionSignal();
    
    EXPECT_FALSE(signal[0]);
    EXPECT_TRUE(signal[1]);
    EXPECT_TRUE(signal[2]);
    EXPECT_FALSE(signal[3]);
}



TEST(BinarySignalTest, Insert) {
    BinarySignal signal("1111");
    BinarySignal ins("00");
    signal.insertSignal(2, ins);

    EXPECT_EQ(signal.getTotalDuration(), 6);
    EXPECT_EQ(signal.getSize(), 3);
    EXPECT_TRUE(signal[0]);
    EXPECT_TRUE(signal[1]);
    EXPECT_FALSE(signal[2]);
    EXPECT_FALSE(signal[3]);
    EXPECT_TRUE(signal[4]);
    EXPECT_TRUE(signal[5]);
}

TEST(BinarySignalTest, Delete) {
    BinarySignal signal("1010110");
    signal.deleteSignal(2, 4);
    EXPECT_EQ(signal.getSize(), 2);
    EXPECT_EQ(signal.getTotalDuration(), 3);
    EXPECT_TRUE(signal[0]);
    EXPECT_FALSE(signal[1]);
    EXPECT_FALSE(signal[2]);
}

TEST(BinarySignalTest, ThrowsDelete) {
    BinarySignal signal("11");
    EXPECT_THROW(signal.deleteSignal(5, 1), std::out_of_range);
}


TEST(BinarySignalTest, FormatSignal) {
    BinarySignal signal("1100");
    std::string formatted = signal.formatSignal();
    EXPECT_FALSE(formatted.empty());
}

TEST(BinarySignalTest, OutputOperator) {
    BinarySignal signal("10");
    std::ostringstream os;
    
    os << signal;
    
    EXPECT_FALSE(os.str().empty());
}

TEST(BinarySignalTest, InputOperator) {
    BinarySignal signal;
    std::istringstream iss("101001.");
    
    iss >> signal;
    
    EXPECT_EQ(signal.getTotalDuration(), 6);
    EXPECT_EQ(signal.getSize(), 5);
}


TEST(BinarySignalTest, BackMethod) {
    BinarySignal signal("10");
    EXPECT_EQ(signal.back().getLevel(), false);
    
    const BinarySignal& const_signal = signal;
    EXPECT_EQ(const_signal.back().getLevel(), false);
}



TEST(BinarySignalTest, BackMethodEmpty) {
    BinarySignal signal;
    EXPECT_THROW(signal.back(), std::invalid_argument);
}


TEST(BinarySignalTest, InsertAtVeryEnd) {
    BinarySignal signal("11"); 
    BinarySignal ins("00");
    
    signal.insertSignal(2, ins);
    
    EXPECT_EQ(signal.getTotalDuration(), 4);
    EXPECT_TRUE(signal[0]);
    EXPECT_TRUE(signal[1]);
    EXPECT_FALSE(signal[2]);
    EXPECT_FALSE(signal[3]);
}


TEST(BinarySignalTest, DeleteToVeryEnd) {
    BinarySignal signal("110011");
    signal.deleteSignal(4, 10);
    
    EXPECT_EQ(signal.getTotalDuration(), 4);
    EXPECT_TRUE(signal[0]);
    EXPECT_TRUE(signal[1]);
    EXPECT_FALSE(signal[2]);
    EXPECT_FALSE(signal[3]);
}


TEST(BinarySignalTest, DeleteWithLeftOffset) {
    BinarySignal signal("1111111111"); 
    signal.deleteSignal(2, 5);
    EXPECT_EQ(signal.getTotalDuration(), 5);
    EXPECT_EQ(signal.getSize(), 1);
}

TEST(BinarySignalTest, DeleteMultipleStates) {
    BinarySignal signal("110011"); 
    signal.deleteSignal(1, 4);
    
    EXPECT_EQ(signal.getTotalDuration(), 2);
}

TEST(BinarySignalTest, InsertWithLeftOffset) {
    BinarySignal signal("1111111111");
    BinarySignal ins("0");
    
    signal.insertSignal(3, ins);
    
    EXPECT_EQ(signal.getTotalDuration(), 11);
    EXPECT_EQ(signal.getSize(), 3);
}


TEST(BinarySignalTest, StringConstructorInvalidChar) {
    EXPECT_THROW(BinarySignal signal("101a01"), std::invalid_argument);
    EXPECT_THROW(BinarySignal signal("11 00"), std::invalid_argument);
    EXPECT_THROW(BinarySignal signal("11\t01"), std::invalid_argument);

    EXPECT_THROW(BinarySignal signal("101x01"), std::invalid_argument);
}
