#include "SequenceContainers/Vector/stl_vector.h"
#include <gtest/gtest.h>
using namespace ::TinySTL;

class VectorTest : public testing::Test {
 protected:
  void SetUp() override {}
};

// test cases from https://sourceforge.net/projects/stlport/
TEST_F(VectorTest, vec_test_1) {
  vector<int> v1;// Empty vector of integers.

  ASSERT_TRUE(v1.empty() == true);
  ASSERT_TRUE(v1.size() == 0);

  v1.push_back(42);// Add an integer to the vector.

  ASSERT_TRUE(v1.size() == 1);

  ASSERT_TRUE(v1[0] == 42);

  {
    vector<vector<int>> vect(10);
    vector<vector<int>>::iterator it(vect.begin()), end(vect.end());
    for (; it != end; ++it) {
      ASSERT_TRUE((*it).empty());
      ASSERT_TRUE((*it).size() == 0);
      ASSERT_TRUE((*it).capacity() == 0);
      ASSERT_TRUE((*it).begin() == (*it).end());
    }
  }
}

TEST_F(VectorTest, vec_test_2) {
  vector<double> v1;// Empty vector of doubles.
  v1.push_back(32.1);
  v1.push_back(40.5);
  vector<double> v2;// Another empty vector of doubles.
  v2.push_back(3.56);
  ASSERT_TRUE(v1.size() == 2);
  ASSERT_TRUE(v1[0] == 32.1);
  ASSERT_TRUE(v1[1] == 40.5);

  ASSERT_TRUE(v2.size() == 1);
  ASSERT_TRUE(v2[0] == 3.56);
  size_t v1Cap = v1.capacity();
  size_t v2Cap = v2.capacity();

  v1.swap(v2);// Swap the vector's contents.

  ASSERT_TRUE(v1.size() == 1);
  ASSERT_TRUE(v1.capacity() == v2Cap);
  ASSERT_TRUE(v1[0] == 3.56);

  ASSERT_TRUE(v2.size() == 2);
  ASSERT_TRUE(v2.capacity() == v1Cap);
  ASSERT_TRUE(v2[0] == 32.1);
  ASSERT_TRUE(v2[1] == 40.5);

  v2 = v1;// Assign one vector to another.

  ASSERT_TRUE(v2.size() == 1);
  ASSERT_TRUE(v2[0] == 3.56);
}

TEST_F(VectorTest, vec_test_3) {
  typedef vector<char> vec_type;

  vec_type v1;// Empty vector of characters.
  v1.push_back('h');
  v1.push_back('i');

  ASSERT_TRUE(v1.size() == 2);
  ASSERT_TRUE(v1[0] == 'h');
  ASSERT_TRUE(v1[1] == 'i');

  vec_type v2(v1.begin(), v1.end());
  v2[1] = 'o';// Replace second character.

  ASSERT_TRUE(v2.size() == 2);
  ASSERT_TRUE(v2[0] == 'h');
  ASSERT_TRUE(v2[1] == 'o');

  ASSERT_TRUE((v1 == v2) == false);
}

TEST_F(VectorTest, vec_test_4) {
  vector<int> v(4);

  v[0] = 1;
  v[1] = 4;
  v[2] = 9;
  v[3] = 16;

  ASSERT_TRUE(v.front() == 1);
  ASSERT_TRUE(v.back() == 16);

  v.push_back(25);

  ASSERT_TRUE(v.back() == 25);
  ASSERT_TRUE(v.size() == 5);

  v.pop_back();

  ASSERT_TRUE(v.back() == 16);
  ASSERT_TRUE(v.size() == 4);
}

TEST_F(VectorTest, vec_test_5) {
  int array[] = {1, 4, 9, 16};

  vector<int> v(array, array + 4);

  ASSERT_TRUE(v.size() == 4);

  ASSERT_TRUE(v[0] == 1);
  ASSERT_TRUE(v[1] == 4);
  ASSERT_TRUE(v[2] == 9);
  ASSERT_TRUE(v[3] == 16);
}

TEST_F(VectorTest, vec_test_6) {
  int array[] = {1, 4, 9, 16, 25, 36};

  vector<int> v(array, array + 6);
  vector<int>::iterator vit;

  ASSERT_TRUE(v.size() == 6);
  ASSERT_TRUE(v[0] == 1);
  ASSERT_TRUE(v[1] == 4);
  ASSERT_TRUE(v[2] == 9);
  ASSERT_TRUE(v[3] == 16);
  ASSERT_TRUE(v[4] == 25);
  ASSERT_TRUE(v[5] == 36);

  vit = v.erase(v.begin());// Erase first element.
  ASSERT_TRUE(*vit == 4);

  ASSERT_TRUE(v.size() == 5);
  ASSERT_TRUE(v[0] == 4);
  ASSERT_TRUE(v[1] == 9);
  ASSERT_TRUE(v[2] == 16);
  ASSERT_TRUE(v[3] == 25);
  ASSERT_TRUE(v[4] == 36);

  vit = v.erase(v.end() - 1);// Erase last element.
  ASSERT_TRUE(vit == v.end());

  ASSERT_TRUE(v.size() == 4);
  ASSERT_TRUE(v[0] == 4);
  ASSERT_TRUE(v[1] == 9);
  ASSERT_TRUE(v[2] == 16);
  ASSERT_TRUE(v[3] == 25);

  v.erase(v.begin() + 1, v.end() - 1);// Erase all but first and last.

  ASSERT_TRUE(v.size() == 2);
  ASSERT_TRUE(v[0] == 4);
  ASSERT_TRUE(v[1] == 25);
}

TEST_F(VectorTest, vec_test_7) {
  int array1[] = {1, 4, 25};
  int array2[] = {9, 16};

  vector<int> v(array1, array1 + 3);
  vector<int>::iterator vit;
  vit = v.insert(v.begin(), 0);// Insert before first element.
  ASSERT_TRUE(*vit == 0);

  vit = v.insert(v.end(), 36);// Insert after last element.
  ASSERT_TRUE(*vit == 36);

  ASSERT_TRUE(v.size() == 5);
  ASSERT_TRUE(v[0] == 0);
  ASSERT_TRUE(v[1] == 1);
  ASSERT_TRUE(v[2] == 4);
  ASSERT_TRUE(v[3] == 25);
  ASSERT_TRUE(v[4] == 36);
}

TEST_F(VectorTest, capacity) {
  struct TestStruct {
    unsigned int a[3];
  };

  {
    vector<int> v;

    ASSERT_TRUE(v.capacity() == 0);
    v.push_back(42);
    ASSERT_TRUE(v.capacity() >= 1);
    v.reserve(5000);
    ASSERT_TRUE(v.capacity() >= 5000);
  }

  {
    //Test that used to generate an assertion when using __debug_alloc.
    vector<TestStruct> va;
    va.reserve(1);
    va.reserve(2);
  }
}

TEST_F(VectorTest, auto_ref) {
  vector<int> ref;
  for (int i = 0; i < 5; ++i) {
    ref.push_back(i);
  }

  vector<vector<int>> v_v_int(1, ref);
  v_v_int.push_back(v_v_int[0]);
  v_v_int.push_back(ref);
  v_v_int.push_back(v_v_int[0]);
  v_v_int.push_back(v_v_int[0]);
  v_v_int.push_back(ref);

  vector<vector<int>>::iterator vvit(v_v_int.begin()), vvitEnd(v_v_int.end());
  for (; vvit != vvitEnd; ++vvit) {
    ASSERT_TRUE(*vvit == ref);
  }
}

//This test check that vector implementation do not over optimize
//operation as PointEx copy constructor is trivial
TEST_F(VectorTest, optimizations_check) {
  struct Point {
    int x, y;
  };

  struct PointEx : public Point {
    PointEx() : Point(), builtFromBase(false) {}
    PointEx(const Point &) : Point(), builtFromBase(true) {}

    bool builtFromBase;
  };
  vector<Point> v1(1);
  ASSERT_TRUE(v1.size() == 1);

  vector<PointEx> v2(v1.begin(), v1.end());
  ASSERT_TRUE(v2.size() == 1);
  ASSERT_TRUE(v2[0].builtFromBase == true);
}

TEST_F(VectorTest, iterators) {
  vector<int> vint(10, 0);
  vector<int> const &crvint = vint;

  ASSERT_TRUE(vint.begin() == vint.begin());
  ASSERT_TRUE(crvint.begin() == vint.begin());
  ASSERT_TRUE(vint.begin() == crvint.begin());
  ASSERT_TRUE(crvint.begin() == crvint.begin());

  ASSERT_TRUE(vint.begin() != vint.end());
  ASSERT_TRUE(crvint.begin() != vint.end());
  ASSERT_TRUE(vint.begin() != crvint.end());
  ASSERT_TRUE(crvint.begin() != crvint.end());
}
