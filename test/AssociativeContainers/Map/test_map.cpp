#include "AssociativeContainers/Map/stl_map.h"
#include <gtest/gtest.h>

using namespace ::TinySTL;

class MapTest : public testing::Test {
 protected:
  void SetUp() override {
  }
};

// test cases from https://sourceforge.net/projects/stlport/
TEST_F(MapTest, map1) {
  typedef map<char, int, less<char>> maptype;
  maptype m;
  // Store mappings between roman numerals and decimals.
  m['l'] = 50;
  m['x'] = 20;// Deliberate mistake.
  m['v'] = 5;
  m['i'] = 1;
  //  cout << "m['x'] = " << m['x'] << endl;
  ASSERT_TRUE(m['x'] == 20);
  m['x'] = 10;// Correct mistake.
  ASSERT_TRUE(m['x'] == 10);
  ASSERT_TRUE(m['z'] == 0);
  //cout << "m['z'] = " << m['z'] << endl; // Note default value is added.
  ASSERT_TRUE(m.count('z') == 1);
  //cout << "m.count('z') = " << m.count('z') << endl;
  pair<maptype::iterator, bool> p = m.insert(pair<const char, int>('c', 100));
  ASSERT_TRUE(p.second);
  ASSERT_TRUE(p.first != m.end());
  ASSERT_TRUE((*p.first).first == 'c');
  ASSERT_TRUE((*p.first).second == 100);

  p = m.insert(pair<const char, int>('c', 100));
  ASSERT_TRUE(!p.second);// already existing pair
  ASSERT_TRUE(p.first != m.end());
  ASSERT_TRUE((*p.first).first == 'c');
  ASSERT_TRUE((*p.first).second == 100);
}

TEST_F(MapTest, equal_range) {
  typedef map<char, int, less<char>> maptype;
  {
    maptype m;
    m['x'] = 10;

    pair<maptype::iterator, maptype::iterator> ret;
    ret = m.equal_range('x');
    ASSERT_TRUE(ret.first != ret.second);
    ASSERT_TRUE((*(ret.first)).first == 'x');
    ASSERT_TRUE((*(ret.first)).second == 10);
    ASSERT_TRUE(++(ret.first) == ret.second);
  }
  {
    {
      maptype m;

      maptype::iterator i = m.lower_bound('x');
      ASSERT_TRUE(i == m.end());

      i = m.upper_bound('x');
      ASSERT_TRUE(i == m.end());

      pair<maptype::iterator, maptype::iterator> ret;
      ret = m.equal_range('x');
      ASSERT_TRUE(ret.first == ret.second);
      ASSERT_TRUE(ret.first == m.end());
    }

    {
      const maptype m;
      pair<maptype::const_iterator, maptype::const_iterator> ret;
      ret = m.equal_range('x');
      ASSERT_TRUE(ret.first == ret.second);
      ASSERT_TRUE(ret.first == m.end());
    }
  }
}

TEST_F(MapTest, template_methods) {
  struct Key {
    Key() : m_data(0) {}
    Key(int data) : m_data(data) {}

    int m_data;
  };

  struct KeyCmp {
    bool operator()(Key lhs, Key rhs) const { return lhs.m_data < rhs.m_data; }

    bool operator()(Key lhs, int rhs) const { return lhs.m_data < rhs; }

    bool operator()(int lhs, Key rhs) const { return lhs < rhs.m_data; }
  };
  {
    typedef map<Key, int, KeyCmp> Container;
    typedef Container::value_type value;
    Container cont;
    cont.insert(value(Key(1), 1));
    cont.insert(value(Key(2), 2));
    cont.insert(value(Key(3), 3));
    cont.insert(value(Key(4), 4));

    ASSERT_TRUE(cont.count(Key(1)) == 1);
    ASSERT_TRUE(cont.count(1) == 1);
    ASSERT_TRUE(cont.count(5) == 0);

    ASSERT_TRUE(cont.find(2) != cont.end());
    ASSERT_TRUE(cont.lower_bound(2) != cont.end());
    ASSERT_TRUE(cont.upper_bound(2) != cont.end());
    ASSERT_TRUE(cont.equal_range(2) != make_pair(cont.begin(), cont.end()));

    Container const &ccont = cont;
    ASSERT_TRUE(ccont.find(2) != ccont.end());
    ASSERT_TRUE(ccont.lower_bound(2) != ccont.end());
    ASSERT_TRUE(ccont.upper_bound(2) != ccont.end());
    ASSERT_TRUE(ccont.equal_range(2) != make_pair(ccont.end(), ccont.end()));
  }
}