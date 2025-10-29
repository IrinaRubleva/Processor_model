#include <gtest/gtest.h>
#include "signalState.h"


TEST(SignalStateTest, DefaultConstructor){
    SignalState state;
    EXPECT_FALSE(state.getLevel());
    EXPECT_EQ(state.getDuration(), 1);
}
TEST(SignalStateTest, ParamConstructor){
    SignalState state(true, 3);
    EXPECT_TRUE(state.getLevel());
    EXPECT_EQ(state.getDuration(), 3);
}
TEST(SignalStateTest, StringConstructor){
    SignalState state("111001");
    EXPECT_TRUE(state.getLevel());
    EXPECT_EQ(state.getDuration(), 3);
}


TEST(SignalStateTest, SET){
    SignalState state;
    
    state.setLevel(false);
    state.setDuration(5); 
    EXPECT_FALSE(state.getLevel()); 
    EXPECT_EQ(state.getDuration(), 5);

    state.setLevel('0');
    EXPECT_FALSE(state.getLevel()); 

    state.setLevelInt(1);
    EXPECT_TRUE(state.getLevel());

    state.setState(true, 2);
    EXPECT_TRUE(state.getLevel());
    EXPECT_EQ(state.getDuration(), 2);
}

TEST(SignalStateTest,  Lengthening){
    SignalState state(true, 3);
    state.lengtheningState(2);
    EXPECT_TRUE(state.getLevel());
    EXPECT_EQ(state.getDuration(), 5);
}
TEST(SignalStateTest, Sortening){
    SignalState state(true, 3);
    state.shorteningState(2);
    EXPECT_TRUE(state.getLevel());
    EXPECT_EQ(state.getDuration(), 1);
}


TEST(SignalStateTest, Inversion) {
    SignalState state(true, 5);
    state.inversionState();
    EXPECT_FALSE(state.getLevel());
    EXPECT_EQ(state.getDuration(), 5);
}


TEST(SignalStateTest, InputStateValid) {
    SignalState state;
    std::istringstream input("1 8");
    state.inputState(input);
    
    EXPECT_TRUE(state.getLevel());
    EXPECT_EQ(state.getDuration(), 8);
}
TEST(SignalStateTest, InputStateInvalidLevel) {
    SignalState state;
    std::istringstream input("2 5");
    
    EXPECT_THROW(state.inputState(input), std::invalid_argument);
}
TEST(SignalStateTest, InputStateInvalidDuration) {
    SignalState state;
    std::istringstream input("1 #&%");
    
    EXPECT_THROW(state.inputState(input), std::invalid_argument);
}


TEST(SignalStateTests, OutputStateValid){
    SignalState state(true, 7);
    std::ostringstream output;
    state.outputState(output);
    std::string result = output.str();
    
    EXPECT_NE(result.find("level=true"), std::string::npos);
    EXPECT_NE(result.find("duration=7"), std::string::npos);
    EXPECT_NE(result.find("SignalState"), std::string::npos);
}


TEST(SignalStateTest, FormatState) {
    SignalState highState(true, 3);
    SignalState lowState(false, 2);
    
    EXPECT_EQ(highState.formatState().length(), 3);
    EXPECT_EQ(lowState.formatState().length(), 2);
}


TEST(SignalStateTest, StreamOperatorOutput) {
    SignalState state(true, 4);
    std::ostringstream output;
    
    output << state;
    std::string result = output.str();
    
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("SignalState"), std::string::npos);
}



TEST(SignalStateTest, StreamOperatorInput) {
    SignalState state;
    std::istringstream input("0 8");
    
    input >> state;
    
    EXPECT_FALSE(state.getLevel()); 
    EXPECT_EQ(state.getDuration(), 8);
}

TEST(SignalStateTest, StreamOperatorInputTrush) {
    SignalState state;
    std::istringstream input("   1     10   ");
    
    state.inputState(input);
    
    EXPECT_TRUE(state.getLevel());
    EXPECT_EQ(state.getDuration(), 10);
}


TEST(SignalStateTest, InvalidDurationConstructor) {
    EXPECT_THROW(SignalState(true, 0), std::invalid_argument);
}

TEST(SignalStateTest, InvalidStringConstructor) {
    EXPECT_THROW(SignalState("2"), std::invalid_argument); 
}

TEST(SignalStateTest, InvalidSetLevel) {
    SignalState state;
    EXPECT_THROW(state.setLevel('2'), std::invalid_argument);
    EXPECT_THROW(state.setLevelInt(5), std::invalid_argument);
}

TEST(SignalStateTest, InvalidSetDuration) {
    SignalState state;
    EXPECT_THROW(state.setDuration(0), std::invalid_argument);
}

TEST(SignalStateTest, InvalidSetState) {
    SignalState state;
    EXPECT_THROW(state.setState(true, 0), std::invalid_argument);
}




TEST(SignalStateTest, EmptyString) {
    SignalState state("");
    EXPECT_FALSE(state.getLevel());
    EXPECT_EQ(state.getDuration(), 0);
}

TEST(SignalStateTest, InvalidFirstChar) {
    EXPECT_THROW(SignalState state("2"), std::invalid_argument);
    EXPECT_THROW(SignalState state("a"), std::invalid_argument);
    EXPECT_THROW(SignalState state("-"), std::invalid_argument);
}

TEST(SignalStateTest, SetLevelIntInvalid) {
    SignalState state("1");
    EXPECT_THROW(state.setLevelInt(2), std::invalid_argument);
    EXPECT_THROW(state.setLevelInt(-1), std::invalid_argument);
    EXPECT_THROW(state.setLevelInt(10), std::invalid_argument);
}


TEST(SignalStateTest, InversionOperator) {
    SignalState state1(true, 5);
    SignalState state2;
    
    state2 = ~state1;
    
    EXPECT_FALSE(state2.getLevel()); 

    EXPECT_EQ(state2.getDuration(), 5);
}