# TinySTL
My implementation of the Standard Template Library in C++17
## PLAN
### 1. 空间适配器(Allocator)  
### 2. 迭代器(Iterator)  
### 3. 序列式容器
vector  
list  
deque  
priority_queue
### 4. 关联式容器
rb_tree  
set
hashtable
hashset
### 5. algorithm
### 6. 智能指针
## RUN Test
```
1. git clone
2. cd TinySTL/
3. mkdir build && cd build
4. cmake .. -DENABLE_TinySTL_TEST=ON && make
5. run *Test* in build/test 
```