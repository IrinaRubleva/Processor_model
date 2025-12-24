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



TEST(VectorAllocator, ConstructorWithAllocator) {
    std::allocator<int> custom_alloc;
    Vector<int, std::allocator<int>> v(custom_alloc);
    
    EXPECT_EQ(v.get_allocator(), custom_alloc);
    EXPECT_EQ(v.size(), 0u);
    EXPECT_TRUE(v.empty());
}


TEST(VectorConstructors, RangeConstructor) {
    std::vector<int> source = {1, 2, 3, 4, 5};
    Vector<int> v(source.begin(), source.end());
    EXPECT_EQ(v.size(), source.size());
    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(v[i], source[i]);
    }
}

TEST(VectorConstructors, MoveConstructorWithAllocator) {
    Vector<int> v1 = {1, 2, 3};
    auto alloc = v1.get_allocator();
    Vector<int> v2(std::move(v1), alloc);
    EXPECT_EQ(v2.size(), 3u);
    EXPECT_EQ(v1.size(), 0u); 
    EXPECT_EQ(v2[0], 1);
    EXPECT_EQ(v2[1], 2);
    EXPECT_EQ(v2[2], 3);
}

TEST(VectorConstructors, CopyConstructorWithAllocator) {
    Vector<int> v1 = {1, 2, 3};
    auto alloc = v1.get_allocator();
    Vector<int> v2(v1, alloc);
    EXPECT_EQ(v2.size(), v1.size());
    for (size_t i = 0; i < v1.size(); ++i) {
        EXPECT_EQ(v2[i], v1[i]);
    }
}

TEST(VectorConstructors, EmptyConstructorWithAllocator) {
    std::allocator<int> custom_alloc;
    Vector<int, std::allocator<int>> v(custom_alloc);
    EXPECT_EQ(v.size(), 0u);
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.get_allocator(), custom_alloc);
}


TEST(VectorAssign, AssignToLargerCapacity) {
    Vector<int> v;
    v.reserve(10);
    v.assign(5, 42);
    EXPECT_EQ(v.size(), 5u);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v[i], 42);
    }
    EXPECT_GE(v.capacity(), 10u);
}

TEST(VectorAssign, AssignToSmallerCapacity) {
    Vector<int> v(10, 7);
    v.assign(3, 99);
    EXPECT_EQ(v.size(), 3u);
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(v[i], 99);
    }
}


TEST(VectorInsert, InsertRange) {
    Vector<int> v = {1, 5, 6};
    std::vector<int> to_insert = {2, 3, 4};
    
    auto it = v.insert(v.begin() + 1, to_insert.begin(), to_insert.end());
    
    EXPECT_EQ(v.size(), 6u);
    for (int i = 0; i < 6; ++i) {
        EXPECT_EQ(v[i], i + 1);
    }
    EXPECT_EQ(*it, 2);
}

TEST(VectorInsert, InsertInitializerList) {
    Vector<int> v = {1, 5};
    auto it = v.insert(v.begin() + 1, {2, 3, 4});
    
    EXPECT_EQ(v.size(), 5u);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(v[i], i + 1);
    }
    EXPECT_EQ(*it, 2);
}

TEST(VectorInsert, InsertAtBegin) {
    Vector<int> v = {2, 3, 4};
    auto it = v.insert(v.begin(), 1);
    
    EXPECT_EQ(v.size(), 4u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(*it, 1);
}


TEST(VectorIterator, AllComparisonOperators) {
    Vector<int> v = {1, 2, 3, 4, 5};
    auto it1 = v.begin();
    auto it2 = v.begin() + 2;
    auto it3 = v.begin() + 2;
    
    EXPECT_TRUE(it1 < it2);
    EXPECT_TRUE(it2 > it1);
    EXPECT_TRUE(it1 <= it2);
    EXPECT_TRUE(it2 >= it1);
    EXPECT_TRUE(it2 <= it3);
    EXPECT_TRUE(it2 >= it3);
    EXPECT_FALSE(it1 > it2);
    EXPECT_FALSE(it2 < it1);
}

TEST(VectorIterator, RandomAccessOperations) {
    Vector<int> v = {0, 10, 20, 30, 40, 50};
    auto it = v.begin() + 2;
    
    EXPECT_EQ(it[0], 20);
    EXPECT_EQ(it[1], 30);
    EXPECT_EQ(it[-1], 10);
    EXPECT_EQ(it[-2], 0);
    
    it += 2;
    EXPECT_EQ(*it, 40);
    
    it -= 3;
    EXPECT_EQ(*it, 10);
    
    auto it2 = it + 4;
    EXPECT_EQ(*it2, 50);
    
    auto it3 = it2 - 5;
    EXPECT_EQ(*it3, 0);
}


TEST(VectorReverseIterator, Operations) {
    Vector<int> v = {1, 2, 3, 4};
    auto rit = v.rbegin();
    
    EXPECT_EQ(*rit, 4);
    ++rit;
    EXPECT_EQ(*rit, 3);
    rit++;
    EXPECT_EQ(*rit, 2);
    --rit;
    EXPECT_EQ(*rit, 3);
    rit--;
    EXPECT_EQ(*rit, 4);
    
    auto rit2 = rit + 2;
    EXPECT_EQ(*rit2, 2);
    
    auto rit3 = rit2 - 1;
    EXPECT_EQ(*rit3, 3);
}


TEST(VectorEdgeCases, EraseAllElements) {
    Vector<int> v = {1, 2, 3, 4, 5};
    
    v.erase(v.begin(), v.end());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_TRUE(v.empty());
}

TEST(VectorNoexcept, MoveOperations) {
    Vector<int> v1 = {1, 2, 3};
    EXPECT_NO_THROW({
        Vector<int> v2(std::move(v1));
    });
    Vector<int> v3 = {4, 5, 6};
    Vector<int> v4;
    EXPECT_NO_THROW({
        v4 = std::move(v3);
    });
}

TEST(VectorIterators, AllIteratorTypes) {
    Vector<int> v = {1, 2, 3, 4, 5};
    const Vector<int> cv = v;
    
    EXPECT_NE(v.begin(), v.end());
    EXPECT_NE(v.cbegin(), v.cend());
    EXPECT_NE(v.rbegin(), v.rend());
    EXPECT_NE(v.crbegin(), v.crend());
    
    EXPECT_NE(cv.begin(), cv.end());
    EXPECT_NE(cv.cbegin(), cv.cend());
    EXPECT_NE(cv.rbegin(), cv.rend());
    EXPECT_NE(cv.crbegin(), cv.crend());
}


TEST(VectorAccess, ConstAt) {
    const Vector<int> v = {1, 2, 3};
    
    EXPECT_EQ(v.at(0), 1);
    EXPECT_EQ(v.at(2), 3);
    EXPECT_THROW(v.at(3), std::out_of_range);
}


TEST(VectorAccess, ConstFrontBack) {
    const Vector<int> v = {10, 20, 30};
    
    EXPECT_EQ(v.front(), 10);
    EXPECT_EQ(v.back(), 30);
}

TEST(VectorAccess, ConstIndexOperator) {
    const Vector<int> v = {100, 200, 300};
    
    EXPECT_EQ(v[0], 100);
    EXPECT_EQ(v[1], 200);
    EXPECT_EQ(v[2], 300);
}


TEST(VectorAssignment, SelfAssignment) {
    Vector<int> v = {1, 2, 3};
    v = v;
    EXPECT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}


TEST(VectorStudentTest, EmplaceBasic) {
    Vector<int> v = {1, 2, 4, 5};

    auto it = v.emplace(v.begin() + 2, 3);
    
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3); 
    EXPECT_EQ(v[3], 4);
    EXPECT_EQ(v[4], 5);
    EXPECT_EQ(*it, 3);
}


TEST(VectorStudentTest, EmplaceInEmpty) {
    Vector<std::string> v;
    auto it = v.emplace(v.begin(), "hello");
    
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v[0], "hello");
    EXPECT_EQ(*it, "hello");
}


TEST(VectorStudentTest, ResizeWithValueSmaller) {
    Vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    v.resize(3, 999);
    
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
}


TEST(VectorStudentTest, ResizeWithValueBigger) {
    Vector<int> v = {1, 2, 3};
    v.resize(6, 99);
    
    EXPECT_EQ(v.size(), 6);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(v[3], 99);
    EXPECT_EQ(v[4], 99);
    EXPECT_EQ(v[5], 99);
}


TEST(VectorStudentTest, InsertCopyBasic) {
    Vector<int> v = {1, 3, 4};
    int value = 2;
    auto it = v.insert(v.begin() + 1, value);
    
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2); 
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(v[3], 4);
    EXPECT_EQ(*it, 2);
    EXPECT_EQ(value, 2);  
}

TEST(VectorStudentTest, InsertCopyAtEnd) {
    Vector<int> v = {1, 2, 3};
    int value = 4;
    
    auto it = v.insert(v.end(), value);
    
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(v[3], 4);
    EXPECT_EQ(*it, 4);
}


TEST(VectorStudentTest, MoveConstructorSameAllocator) {
    Vector<int> v1;
    v1.push_back(10);
    v1.push_back(20);
    v1.push_back(30);
    Vector<int> v2(std::move(v1), v1.get_allocator());
    
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2[0], 10);
    EXPECT_EQ(v2[1], 20);
    EXPECT_EQ(v2[2], 30);

    EXPECT_TRUE(v1.empty());
}

TEST(VectorStudentTest, MoveConstructorDifferentAllocators) {
    Vector<int> v1;
    v1.push_back(100);
    v1.push_back(200);
    
    Vector<int> v2(std::move(v1), std::allocator<int>());
    EXPECT_EQ(v2.size(), 2);
    EXPECT_EQ(v2[0], 100);
    EXPECT_EQ(v2[1], 200);
}


TEST(VectorTest, MoveWithDifferentAllocator) {
    Vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);
    
    size_t old_size = v1.size();

    Vector<int> v2(std::move(v1), std::allocator<int>());
    
    EXPECT_EQ(v2.size(), old_size);
    EXPECT_EQ(v2[0], 1);
    EXPECT_EQ(v2[1], 2);
    EXPECT_EQ(v2[2], 3);
}

TEST(VectorTest, ClearCallsDestroyRange) {
    Vector<int> v;
    for (int i = 0; i < 10; i++) {
        v.push_back(i * 10);
    }
    
    v.clear();
    
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
}


TEST(VectorCoverage, EmptyVectorConstructors) {
    Vector<int> v1{};
    EXPECT_TRUE(v1.empty());
    
    Vector<int> v2(0, 42);
    EXPECT_TRUE(v2.empty());
    
    Vector<int> v3;
    Vector<int> v4(v3);
    EXPECT_TRUE(v4.empty());
    
    Vector<int> v5(v3, std::allocator<int>());
    EXPECT_TRUE(v5.empty());
}

TEST(VectorCoverage, AssignWithinCapacity) {
    Vector<int> v;
    v.reserve(10);
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    
    v.assign(2, 99);
    
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], 99);
    EXPECT_EQ(v[1], 99);
}

TEST(VectorCoverage, PopBackOnEmpty) {
    Vector<int> v;
    EXPECT_NO_THROW(v.pop_back());
    EXPECT_TRUE(v.empty());
}

TEST(VectorCoverage, ResizeWithReallocation) {
    Vector<int> v = {1, 2, 3};
    v.reserve(4); 
    
    v.resize(10, 99); 
    
    EXPECT_EQ(v.size(), 10);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    for (int i = 3; i < 10; i++) {
        EXPECT_EQ(v[i], 99);
    }
}

TEST(VectorCoverage, InsertZeroElements) {
    Vector<int> v = {1, 2, 3};
    
    auto it = v.insert(v.begin() + 1, 0, 99);
    
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    EXPECT_EQ(*it, 2);
}

TEST(VectorCoverage, AssignRangeWithinCapacity) {
    Vector<int> v;
    v.reserve(10);
    v.push_back(100);
    v.push_back(200);
    v.push_back(300);
    
    std::vector<int> src = {1, 2, 3, 4};
    
    v.assign(src.begin(), src.end());
    
    EXPECT_EQ(v.size(), 4);
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(v[i], i + 1);
    }
}

TEST(VectorCoverage, MoveWithDifferentAllocSimple) {
    Vector<int> v1 = {1, 2, 3};
    
    Vector<int> v2(std::move(v1), std::allocator<int>());
    
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2[0], 1);
    EXPECT_EQ(v2[1], 2);
    EXPECT_EQ(v2[2], 3);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}