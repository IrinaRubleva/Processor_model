#include <gtest/gtest.h>
#include "../include/Vector.hpp"



TEST(VectorBasic, DefaultConstructEmpty) {
    Vector<int> v;
    EXPECT_EQ(v.size(), 0u);
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.capacity(), 0u);
    EXPECT_EQ(v.begin(), v.end());
}

TEST(VectorBasic, FillConstructor) {
    Vector<int> v(5, 42);
    EXPECT_EQ(v.size(), 5u);
    for (size_t i = 0; i < v.size(); ++i) EXPECT_EQ(v[i], 42);
}

TEST(VectorBasic, InitializerList) {
    Vector<int> v{1, 2, 3, 4};
    EXPECT_EQ(v.size(), 4u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[3], 4);
}


TEST(VectorModifiers, AssignCount) {
    Vector<std::string> v;
    v.assign(3, std::string("hello"));
    EXPECT_EQ(v.size(), 3u);
    for (size_t i = 0; i < v.size(); ++i) EXPECT_EQ(v[i], "hello");
}

TEST(VectorModifiers, AssignRange) {
    std::vector<int> src = {10,20,30,40};
    Vector<int> v;
    v.assign(src.begin(), src.end());
    EXPECT_EQ(v.size(), src.size());
    for (size_t i = 0; i < v.size(); ++i) EXPECT_EQ(v[i], src[i]);
}

TEST(VectorModifiers, AssignInitList) {
    Vector<int> v;
    v.assign({7,8,9});
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[1], 8);
}


TEST(VectorModifiers, PushPopBackAndIterate) {
    Vector<int> v;
    for (int i = 0; i < 10; ++i) v.push_back(i);
    EXPECT_EQ(v.size(), 10u);

    int sum = 0;
    for (auto it = v.begin(); it != v.end(); ++it) sum += *it;
    EXPECT_EQ(sum, (9*10)/2);

    v.pop_back();
    EXPECT_EQ(v.size(), 9u);
    EXPECT_EQ(v.back(), 8);
}


TEST(VectorModifiers, InsertEraseBasic) {
    Vector<int> v{1,2,4,5};
    auto it = v.insert(v.begin() + 2, 3);
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(v.size(), 5u);
    for (int i = 0; i < 5; ++i) EXPECT_EQ(v[i], i+1);

    
    auto it2 = v.erase(v.begin() + 2);
    EXPECT_EQ(*it2, 4);
    EXPECT_EQ(v.size(), 4u);
    EXPECT_EQ(v[2], 4);

    
    v.erase(v.begin() + 1, v.begin() + 3);
    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 5);
}


TEST(VectorCapacity, ReserveResize) {
    Vector<int> v;
    v.reserve(20);
    EXPECT_GE(v.capacity(), 20u);
    EXPECT_EQ(v.size(), 0u);

    v.resize(5);
    EXPECT_EQ(v.size(), 5u);
    for (size_t i = 0; i < v.size(); ++i) EXPECT_EQ(v[i], 0);

    v.resize(2);
    EXPECT_EQ(v.size(), 2u);
}


TEST(VectorAccess, AtThrows) {
    Vector<int> v{1,2,3};
    EXPECT_NO_THROW(v.at(0));
    EXPECT_THROW(v.at(3), std::out_of_range);
}


TEST(VectorCopyMove, CopyAndMoveSemantics) {
    Vector<int> v{1,2,3,4,5};
    Vector<int> cpy = v;
    EXPECT_EQ(cpy.size(), v.size());
    for (size_t i=0;i<v.size();++i) EXPECT_EQ(cpy[i], v[i]);

    Vector<int> mv = std::move(v);
    EXPECT_EQ(mv.size(), 5u);
}


TEST(VectorIteratorTest, RandomAccessAndConst) {
    Vector<int> v{10,11,12,13,14};
    auto it = v.begin();
    EXPECT_EQ(it[2], 12);
    it += 3;
    EXPECT_EQ(*it, 13);
    it -= 2;
    EXPECT_EQ(*it, 11);

    const Vector<int> cv = v;
    auto cit = cv.cbegin();
    EXPECT_EQ(cit[4], 14);

    auto diff = cv.cend() - cv.cbegin();
    EXPECT_EQ(diff, static_cast<ptrdiff_t>(cv.size()));
}

TEST(VectorIteratorTest, ReverseIterator) {
    Vector<int> v{1,2,3,4};
    auto rit = v.rbegin();
    EXPECT_EQ(*rit, 4);
    ++rit;
    EXPECT_EQ(*rit, 3);
}

TEST(VectorModifiers, InsertCountValue) {
    Vector<int> v{1,4,5};
    v.insert(v.begin() + 1, 2, 2);
    ASSERT_EQ(v.size(), 5u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 2);
}

TEST(VectorModifiers, AssignLargerSmaller) {
    Vector<int> v{1,2,3};
    v.assign(5, 7);
    EXPECT_EQ(v.size(), 5u);
    for (size_t i=0;i<5;++i) EXPECT_EQ(v[i], 7);

    v.assign(2, 9);
    EXPECT_EQ(v.size(), 2u);
    EXPECT_EQ(v[0], 9);
    EXPECT_EQ(v[1], 9);
}

TEST(VectorIteratorTest, ComparisonsAndDistance) {
    Vector<int> v{0,1,2,3,4,5};
    auto a = v.begin();
    auto b = v.begin() + 3;
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
    EXPECT_EQ(b - a, 3);
    EXPECT_EQ(std::distance(a, b), 3);
}


TEST(VectorBasic, ClearAndReuse) {
    Vector<int> v;
    for (int i=0;i<5;++i) v.push_back(i+1);
    EXPECT_EQ(v.size(), 5u);
    v.clear();
    EXPECT_EQ(v.size(), 0u);
    EXPECT_TRUE(v.empty());

    v.push_back(42);
    EXPECT_EQ(v.size(), 1u);
    EXPECT_EQ(v[0], 42);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}