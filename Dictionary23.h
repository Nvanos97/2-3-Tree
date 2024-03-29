// Dictionary23.h
// Programmers: Nathan Vanos, Jackson Paris
// CPSC 223 Programming Assignment 5
// Date: December 1st, 2017

#ifndef Dictionary23_h
#define Dictionary23_h

#include<iostream>
#include<cassert>
#include<memory>
#include "Dictionary.h"

using namespace std;

template<class K, class V>
struct Node23{
    //unique_ptr<Node23<K, V>> children[3];
    unique_ptr<Node23<K, V>> left;
    unique_ptr<Node23<K, V>> leftMid;
    unique_ptr<Node23<K, V>> mid;
    unique_ptr<Node23<K, V>> right;
    Node23<K, V> *parent; // Only keep if useful
    
    bool isTwoNode = true;
    bool isOneNode = false;
    
    K keys[3];
    V values[3];
};

template<class K, class V>
class Dictionary23: public Dictionary<K, V> {
public:
    // Dictionary23 Constructor - no code because of smart pointer use
    // Time: O(1)
    Dictionary23(){}
    
    // Dictionary23 Copy Constructor
    // Time: O(n)
    Dictionary23(const Dictionary23<K, V> &other)
    {
        copy(other.root.get());
    }
    
    // Dictionary23 Assignment Operator
    // Time: O(n)
    Dictionary23& operator=(const Dictionary23<K, V> &other)
    {
        if(this == &other)
            return *this;
        root = nullptr;
        copy(other -> root.get());
        return *this;
    }
    
    // Dictionary23 Destructor - no code because of smart pointer use
    // Time: O(1)
    ~Dictionary23(){}
    
    // Adds a node to the 2-3 tree dictionary
    // Time: O(logn)
    void add(K key, V value) override
    {
        if(isEmpty())
        {
            root = createTwoNode(key, value);
            root -> parent = nullptr;
        }else{
            addHelper(root.get(), key, value);
        }
    }
    
    // Retrieves the value associated with the given key
    // Time: O(logn)
    V getValue(K key) const override
    {
        return findValue(key, root.get());
    }
    
    // Retrieves the key associated with the given value
    // Time: O(n)
    K getKey(V value) const override
    {
        auto helperPtr = unique_ptr<Node23<K, V>>(new Node23<K, V>);
        helperPtr = findKey(value, root.get());
        if(helperPtr -> values[0] == value)
            return helperPtr -> keys[0];
        else
            throw DictionaryException("The key could not be found in the dictionary");
    }
    
    // Removes a key-value pair from the dictionary
    // Time: O(logn)
    void remove(K key) override
    {
        if(!keyExists(key)) // Case 0: The program using the class tried to remove a key that was never added in the first place
            throw DictionaryException("Cannot remove key. Key does not exist.");
        else if(size() == 1) // Case 1: The node being removed is the only key-value pair in the tree
        {
            root = nullptr;
        } else{
            deleteNode(root.get(), key);
        }
    }
    
    // Displays the dictionary in sorted order
    // Time: O(n)
    void display() const override
    {
        if(isEmpty())
        {
            cout << "The dictionary is empty." << endl;
        } else{
            cout << "Items in 2-3 Tree Dictionary: ";
            displayHelper(root.get());
            cout << endl;
        }
    }
    
    // Returns the number of key-value pairs in the current 2-3 tree dictionary
    // Time: O(n)
    int size() const override
    {
        return sizeHelper(root.get());
    }
    
    // Returns the height of the tree
    // Time: O(logn)
    int height() const override
    {
        return heightHelper(root.get());
    }
    
    // Finds whether or not there is a key in the dictionary
    // Time: O(logn)
    bool keyExists(K key) const override
    {
        return keyExistsHelper(key, root.get());
    }
    
    // Returns true if there are no nodes in the dictionary
    // Time: O(1)
    bool isEmpty() const override
    {
        if(root == nullptr)
            return true;
        else
            return false;
    }
    
private:
    // Unique pointer to the root of the 2-3 tree dictionary
    unique_ptr<Node23<K, V>> root;
    
    // Creates a new node while maintaining the 2-3 tree properties
    // Time: O(logn)
    void addHelper(Node23<K, V> *subTreePtr, K key, V value)
    {
        //if(subTreePtr -> parent)
        //cout << "Current node's parent: " << subTreePtr -> parent -> keys[0] << endl;
        if(key == subTreePtr -> keys[0]){
            cout << "Updating a value " << endl;
            subTreePtr -> values[0] = value;
        }
        else if(key == subTreePtr -> keys[1]){
            cout << "Updating a value " << endl;
            subTreePtr -> values[1] = value;
        }
        else if(subTreePtr -> isTwoNode){ // Case 0: The current node is a 2 node
            if(!subTreePtr -> left && !subTreePtr -> right) // Case A: The node is a two node and a leaf
            {
                if(key < subTreePtr -> keys[0]) // Case i: The new key is smaller than the existing key
                {
                    subTreePtr -> keys[1] = subTreePtr -> keys[0];
                    subTreePtr -> values[1] = subTreePtr -> values[0];
                    subTreePtr -> keys[0] = key;
                    subTreePtr -> values[0] = value;
                    subTreePtr -> isTwoNode = false;
                } else{ // Case ii: The new key is larger than the existing key
                    subTreePtr -> keys[1] = key;
                    subTreePtr -> values[1] = value;
                    subTreePtr -> isTwoNode = false;
                }
            } else{ // Case B: The node is a two node but not a leaf
                if(key < subTreePtr -> keys[0]) {// Case i: Key is smaller than the key in the current node (traverse left)
                    addHelper(subTreePtr -> left.get(), key, value);
                }
                else{ // Case ii: Key is larger than the key in the current node (traverse right)
                    addHelper(subTreePtr -> right.get(), key, value);
                }
            }
        }else{ // Case 1: The current node is a 3 node
            if(!subTreePtr -> left && !subTreePtr -> mid && !subTreePtr -> right) // Case A: The node is a leaf (split the node)
            {
                createFourNode(subTreePtr, key, value);
                splitHelper(subTreePtr);
            }else{ // Case B: Current node is 3 node but not a leaf
                if(key < subTreePtr -> keys[0]){
                    addHelper(subTreePtr -> left.get(), key, value);
                }
                else if(key > subTreePtr -> keys[1]){
                    addHelper(subTreePtr -> right.get(), key, value);
                }
                else{
                    addHelper(subTreePtr -> mid.get(), key, value);
                }
            }
        }
    }
    
    // Splits a 4-node
    // Time: O(logn)
    void splitHelper(Node23<K, V> *it) // returns responsibility
    {
        if(it == root.get()) // Case 0: The node is the root (splitting a 3 node into a 2 node with children)
        {
            auto leftChild = createTwoNode(it -> keys[0], it -> values[0]);
            auto rightChild = createTwoNode(it -> keys[2], it -> values[2]);
            
            leftChild -> left = move(it -> left);
            leftChild -> right = move(it -> leftMid);
            rightChild -> left = move(it -> mid);
            rightChild -> right = move(it -> right);
            
            if (leftChild->left) {
                leftChild->left->parent = leftChild.get();
                leftChild->right->parent = leftChild.get();
                rightChild->left->parent = rightChild.get();
                rightChild->right->parent = rightChild.get();
            }
            
            it -> keys[0] = it -> keys[1];
            it -> values[0] = it -> values[1];
            it -> left = move(leftChild);
            it -> right = move(rightChild);
            it -> isTwoNode = true;
            
            it -> left -> parent = it;
            it -> right -> parent = it;
        }
        else if(it -> parent -> isTwoNode) // Case 1: The node above the current leaf is a two node
        {
            if(it -> keys[1] < it -> parent -> keys[0]) // Case A: This node is the left child of its parent
            {
                it -> parent -> keys[1] = it -> parent -> keys[0];
                it -> parent -> values[1] = it -> parent -> values[0];
                it -> parent -> keys[0] = it -> keys[1];
                it -> parent -> values[0] = it -> values[1];
                it -> parent -> isTwoNode = false;
                
                auto midChild = createTwoNode(it -> keys[2], it -> values[2]);
                midChild -> left = move(it -> mid);
                midChild -> right = move(it -> right);
                if(midChild -> left)
                {
                    midChild -> left -> parent = midChild.get();
                    midChild -> right -> parent = midChild.get();
                }
                
                it -> parent -> mid = move(midChild);
                it -> parent -> mid -> parent = it -> parent;
                it -> left = move(it -> left);
                it -> right = move(it -> leftMid);
                it -> isTwoNode = true;
            } else{ // Case B: This node is the right child of its parent
                it -> parent -> keys[1] = it -> keys[1];
                it -> parent -> values[1] = it -> values[1];
                it -> parent -> isTwoNode = false;
                
                auto midChild = createTwoNode(it -> keys[0], it -> values[0]);
                midChild -> left = move(it -> left);
                midChild -> right = move(it -> leftMid);
                if(midChild -> left)
                {
                    midChild -> left -> parent = midChild.get();
                    midChild -> right -> parent = midChild.get();
                }
                
                it -> parent -> mid = move(midChild);
                it -> parent -> mid -> parent = it -> parent;
                it -> keys[0] = it -> keys[2];
                it -> values[0] = it -> values[2];
                it -> left = move(it -> mid);
                it -> right = move(it -> right);
                it -> isTwoNode = true;
            }
        } else{ // Case 2: This node's parent is a 3 node
            if(it -> keys[1] < it -> parent -> keys[0]) // Case A: This node is the left child of its parent
            {
                it -> parent -> keys[2] = it -> parent -> keys[1];
                it -> parent -> keys[1] = it -> parent -> keys[0];
                it -> parent -> keys[0] = it -> keys[1];
                it -> parent -> values[2] = it -> parent -> values[1];
                it -> parent -> values[1] = it -> parent -> values[0];
                it -> parent -> values[0] = it -> values[1];
                
                auto leftMiddleChild = createTwoNode(it -> keys[2], it -> values[2]);
                leftMiddleChild -> left = move(it -> mid);
                leftMiddleChild -> right = move(it -> right);
                if(leftMiddleChild -> left)
                {
                    leftMiddleChild -> left -> parent = leftMiddleChild.get();
                    leftMiddleChild -> right -> parent = leftMiddleChild.get();
                }
                
                it -> parent -> leftMid = move(leftMiddleChild);
                it -> parent -> leftMid -> parent = it -> parent;
                it -> left = move(it -> left);
                it -> right = move(it -> leftMid);
                it -> isTwoNode = true;
            }
            else if(it -> keys[1] > it -> parent -> keys[1]) // Case B: This node is the right child of its parent
            {
                it -> parent -> keys[2] = it -> keys[1];
                it -> parent -> values[2] = it -> values[1];
                
                auto rightMiddleChild = createTwoNode(it -> keys[0], it -> values[0]);
                rightMiddleChild -> left = move(it -> left);
                rightMiddleChild -> right = move(it -> leftMid);
                if(rightMiddleChild -> left)
                {
                    rightMiddleChild -> left -> parent = rightMiddleChild.get();
                    rightMiddleChild -> right -> parent = rightMiddleChild.get();
                }
                
                it -> parent -> leftMid = move(it -> parent -> mid);
                it -> parent -> leftMid -> parent = it -> parent;
                it -> parent -> mid = move(rightMiddleChild);
                it -> parent -> mid -> parent = it -> parent;
                it -> keys[0] = it -> keys[2];
                it -> values[0] = it -> values[2];
                it -> left = move(it -> mid);
                it -> right = move(it -> right);
                it -> isTwoNode = true;
            } else{ // Case C: This node is the middle child of its parent
                it -> parent -> keys[2] = it -> parent -> keys[1];
                it -> parent -> keys[1] = it -> keys[1];
                it -> parent -> values[2] = it -> parent -> values[1];
                it -> parent -> values[1] = it -> values[1];
                
                auto leftMiddleChild = createTwoNode(it -> keys[0], it -> values[0]);
                leftMiddleChild -> left = move(it -> left);
                leftMiddleChild -> right = move(it -> leftMid);
                if(leftMiddleChild -> left)
                {
                    leftMiddleChild -> left -> parent = leftMiddleChild.get();
                    leftMiddleChild -> right -> parent = leftMiddleChild.get();
                }
                
                it -> parent -> leftMid = move(leftMiddleChild);
                it -> parent -> leftMid -> parent = it -> parent;
                it -> keys[0] = it -> keys[2];
                it -> values[0] = it -> values[2];
                it -> left = move(it -> mid);
                it -> isTwoNode = true;
            }
            splitHelper(it -> parent);
        }
    }
    
    // Returns the value; throws exception if it isn't found
    // Time: O(logn)
    V findValue(K target, Node23<K, V> *it) const
    {
        if(it == nullptr)
        {
            throw DictionaryException("The value was not found in the dictionary");
        }
        else if(it -> isTwoNode)
        {
            if(target == it -> keys[0])
                return it -> values[0];
            else if(target < it -> keys[0])
                return findValue(target, it -> left.get());
            else
                return findValue(target, it -> right.get());
        } else{
            if(target == it -> keys[0])
                return it -> values[0];
            else if(target == it -> keys[1])
                return it -> values[1];
            else if(target < it -> keys[0])
                return findValue(target, it -> left.get());
            else if(target > it -> keys[1])
                return findValue(target, it -> right.get());
            else
                return findValue(target, it -> mid.get());
        }
    }
    
    // Finds the desired key
    // Time: O(n)
    unique_ptr<Node23<K, V>> findKey(V target, Node23<K, V> *it) const
    {
        auto helperPtr = unique_ptr<Node23<K, V>>(new Node23<K, V>);
        if(it == nullptr) // Case 0: The current node is empty
        {
            helperPtr = createTwoNode(root -> keys[0], root -> values[0]);
            return helperPtr;
        }
        else if(it -> isTwoNode) // Case 1: The current node is a two node
        {
            if(target == it -> values[0])
            {
                helperPtr = createTwoNode(it -> keys[0], it -> values[0]);
                return helperPtr;
            }
            else{
                helperPtr = findKey(target, it -> left.get());
                if(helperPtr && helperPtr -> values[0] == target)
                    return helperPtr;
                helperPtr = findKey(target, it -> right.get());
                if(helperPtr && helperPtr -> values[0] == target)
                    return helperPtr;
                helperPtr = createTwoNode(root -> keys[0], root -> values[0]);
                return helperPtr;
            }
        } else{ // Case 2: The current node is a 3 node
            if(target == it -> values[0])
            {
                helperPtr = createTwoNode(it -> keys[0], it -> values[0]);
                return helperPtr;
            }
            else if(target == it -> values[1])
            {
                helperPtr = createTwoNode(it -> keys[1], it -> values[1]);
                return helperPtr;
            }
            else{
                helperPtr = findKey(target, it -> left.get());
                if(helperPtr && helperPtr -> values[0] == target)
                    return helperPtr;
                helperPtr = findKey(target, it -> mid.get());
                if(helperPtr && helperPtr -> values[0] == target)
                    return helperPtr;
                helperPtr = findKey(target, it -> right.get());
                if(helperPtr && helperPtr -> values[0] == target)
                    return helperPtr;
                helperPtr = createTwoNode(root -> keys[0], root -> values[0]);
                return helperPtr;
            }
        }
    }
    
    // Finds and removes a node from the 2-3 tree
    // Time: O(logn)
    void deleteNode(Node23<K, V> *subTreePtr, K target)
    {
        if(keyExists(target) && subTreePtr) // Case that the current node exists
        {
            if(subTreePtr -> isTwoNode) // Case 1: The current node is a 2-node
            {
                if(subTreePtr -> keys[0] == target) // Case A: The current key is the target key
                {
                    if(!subTreePtr -> left && subTreePtr -> parent)
                    {
                        subTreePtr -> isOneNode = true;
                        subTreePtr -> isTwoNode = false;
                        pushUp(subTreePtr);
                    }
                    else{ // Node is not a leaf
                        subTreePtr -> keys[0] = findMaxKey(subTreePtr -> left.get());
                        subTreePtr -> values[0] = findMaxVal(subTreePtr -> left.get());
                        deleteNode(subTreePtr -> left.get(), subTreePtr -> keys[0]);
                    }
                } else{ // Case B: The current key is not the target key
                    if(target < subTreePtr -> keys[0]) // Case I: The target key is less than the current key
                        deleteNode(subTreePtr -> left.get(), target);
                    else // Case II: The target key is greater than the current key
                        deleteNode(subTreePtr -> right.get(), target);
                }
            } else{ // Case 2: The current node is a 3-node
                if(subTreePtr -> keys[0] == target) // Case A: The small key is the target key
                {
                    if(!subTreePtr -> left) // Case I: The node is a leaf
                    {
                        subTreePtr -> keys[0] = subTreePtr -> keys[1];
                        subTreePtr -> values[0] = subTreePtr -> values[1];
                        subTreePtr -> isTwoNode = true;
                    } else{ // Case II: The node is not a leaf
                        subTreePtr -> keys[0] = findMaxKey(subTreePtr -> left.get());
                        subTreePtr -> values[0] = findMaxVal(subTreePtr -> left.get());
                        deleteNode(subTreePtr -> left.get(), subTreePtr -> keys[0]);
                    }
                }
                else if(subTreePtr -> keys[1] ==  target) // Case B: The large key is the target key
                {
                    if(!subTreePtr -> left) // Case I: The node is a leaf
                    {
                        subTreePtr -> isTwoNode = true;
                    } else{ // Case II: The node is not a leaf
                        subTreePtr -> keys[1] = findMaxKey(subTreePtr -> mid.get());
                        subTreePtr -> values[1] = findMaxVal(subTreePtr -> mid.get());
                        deleteNode(subTreePtr -> mid.get(), subTreePtr -> keys[1]);
                    }
                } else{ // Case C: Neither key is the target key
                    if(target < subTreePtr -> keys[0]) // Case I: The target key is less than the small key
                        deleteNode(subTreePtr -> left.get(), target);
                    else if(target > subTreePtr -> keys[1]) // Case II: The target key is greater than the large key
                        deleteNode(subTreePtr -> right.get(), target);
                    else // Case III: The target key is greater than the small key but is less than the large key
                        deleteNode(subTreePtr -> mid.get(), target);
                }
            }
        } else{ // Case that the current node does not exist
            throw DictionaryException("Cannot remove node, the node does not exist");
        }
    }
    
    // Finds the largest key in the subtree and returns it
    // Time: O(logn)
    K findMaxKey(Node23<K, V> *it) const
    {
        if(!it)
            throw DictionaryException("Search for max node failed due to invalid subtree");
        else{
            if(it -> right)
                return findMaxKey(it -> right.get());
            else{
                if(it -> isTwoNode)
                    return it -> keys[0];
                else
                    return it -> keys[1];
            }
        }
    }
    
    // Finds the largest value in the tree and returns it
    // Time: O(logn)
    V findMaxVal(Node23<K, V> *it) const
    {
        if(!it)
            throw DictionaryException("Search for max node failed due to invalid subtree");
        else{
            if(it -> right)
                return findMaxVal(it -> right.get());
            else{
                if(it -> isTwoNode)
                    return it -> values[0];
                else
                    return it -> values[1];
            }
        }
    }
    
    // Finds the smallest key in the subtree and returns the node it's located in
    // Time: O(logn)
    K findMinKey(Node23<K, V> *it) const
    {
        if(!it)
            throw DictionaryException("Search for min node failed due to invalid subtree");
        else{
            if(it -> left)
                return findMinKey(it -> left.get());
            else{
                return it -> keys[0];
            }
        }
    }
    
    // Finds the smallest value in the subtree and returns the node it's located in
    // Time: O(logn)
    V findMinVal(Node23<K, V> *it) const
    {
        if(!it){
            cout << "Inside the case that a key does not exist. " << endl;
            throw DictionaryException("Search for min node failed due to invalid subtree");
        }
        else{
            if(it -> left)
                return findMinVal(it -> left.get());
            else{
                return it -> values[0];
            }
        }
    }
    
    // Fixes the tree after the removal of a leaf two node
    // Time: O(logn)
    void pushUp(Node23<K, V> *it)
    {
        assert(it -> isOneNode);
        if(it == root.get())
        {
            root = move(it -> left);
            root -> parent = nullptr;
        } else{
            if(it -> parent -> isTwoNode) // Case where the parent cannot be borrowed from (is a 2 node)
            {
                if(it -> keys[0] <= it -> parent -> left -> keys[0] && !it -> parent -> right -> isTwoNode) // Case where this is the left child and it's sibling is a 3 node
                {
                    it -> keys[0] = it -> parent -> keys[0];
                    it -> values[0] = it -> parent -> values[0];
                    it -> parent -> keys[0] = it -> parent -> right -> keys[0];
                    it -> parent -> values[0] = it -> parent -> right -> values[0];
                    
                    it -> right = move(it -> parent -> right -> left);
                    if(it -> left)
                        it -> right -> parent = it;
                    it -> isOneNode = false;
                    it -> isTwoNode = true;
                    
                    it -> parent -> right -> keys[0] = it -> parent -> right -> keys[1];
                    it -> parent -> right -> values[0] = it -> parent -> right -> values[1];
                    it -> parent -> right -> left = move(it -> parent -> right -> mid);
                    it -> parent -> right -> isTwoNode = true;
                }
                else if(it -> keys[0] >= it -> parent -> right -> keys[0] && !it -> parent -> left -> isTwoNode) // Case where this is the right child and it's sibling is a 3 node
                {
                    it -> keys[0] = it -> parent -> keys[0];
                    it -> parent -> keys[0] = it -> parent -> left -> keys[1];
                    it -> values[0] = it -> parent -> values[0];
                    it -> parent -> values[0] = it -> parent -> left -> values[1];
                    
                    it -> isOneNode = false;
                    it -> right = move(it -> left);
                    it -> left = move(it -> parent -> left -> right);
                    if(it -> left)
                        it -> left -> parent = it;
                    it -> isTwoNode = true;
                    
                    it -> parent -> left -> right = move(it -> parent -> left -> mid);
                    it -> parent -> left -> isTwoNode = true;
                } else{ // Case where sibling is a 2 node
                    it -> isOneNode = false;
                    it -> isTwoNode = false;
                    if(it -> keys[0] <= it -> parent -> keys[0]) // Case where this is the left child
                    {
                        it -> keys[0] = it -> parent -> keys[0];
                        it -> keys[1] = it -> parent -> right -> keys[0];
                        it -> values[0] = it -> parent -> values[0];
                        it -> values[1] = it -> parent -> right -> values[0];
                        
                        it -> mid = move(it -> parent -> right -> left);
                        it -> right = move(it -> parent -> right -> right);
                        if(it -> left)
                        {
                            it -> mid -> parent = it;
                            it -> right -> parent = it;
                        }
                        it -> parent -> isOneNode = true;
                        it -> parent -> isTwoNode = false;
                    } else{ // Case where this is the right child
                        it -> keys[0] = it -> parent -> left -> keys[0];
                        it -> keys[1] = it -> parent -> keys[0];
                        it -> values[0] = it -> parent -> left -> values[0];
                        it -> values[1] = it -> parent -> values[0];
                        
                        it -> right = move(it -> left);
                        it -> left = move(it -> parent -> left -> left);
                        it -> mid = move(it -> parent -> left -> right);
                        if(it -> left)
                        {
                            it -> left -> parent = it;
                            it -> mid -> parent = it;
                        }
                        it -> parent -> isOneNode = true;
                        it -> parent -> isTwoNode = false;
                        it -> parent -> left = move(it -> parent -> right);
                    }
                    pushUp(it -> parent);
                }
            } else{ // Case where the parent is a 3 node
                it -> isOneNode = false;
                it -> isTwoNode = false;
                if(it -> parent -> keys[0] >= it -> keys[0]) // Left child of a 3 node
                {
                    if(it -> parent -> mid -> isTwoNode)
                    {
                        it -> keys[0] = it -> parent -> keys[0];
                        it -> keys[1] = it -> parent -> mid -> keys[0];
                        it -> values[0] = it -> parent -> values[0];
                        it -> values[1] = it -> parent -> mid -> values[0];
                        it -> mid = move(it -> parent -> mid -> left);
                        it -> right = move(it -> parent -> mid -> right);
                        if(it -> left)
                        {
                            it -> mid -> parent = it;
                            it -> right -> parent = it;
                        }
                        it -> parent -> keys[0] = it -> parent -> keys[1];
                        it -> parent -> values[0] = it -> parent -> values[1];
                        it -> parent -> isTwoNode = true;
                    } else{ // Middle sibling is a 3 node
                        it -> isTwoNode = true;
                        it -> keys[0] = it -> parent -> keys[0];
                        it -> values[0] = it -> parent -> values[0];
                        it -> parent -> keys[0] = it -> parent -> mid -> keys[0];
                        it -> parent -> values[0] = it -> parent -> mid -> values[0];
                        it -> parent -> mid -> keys[0] = it -> parent -> mid -> keys[1];
                        it -> parent -> mid -> values[0] = it -> parent -> mid -> values[1];
                        it -> parent -> mid -> isTwoNode = true;
                        
                        it -> right = move(it -> parent -> mid -> left);
                        if(it -> left)
                            it -> right -> parent = it;
                        it -> parent -> mid -> left = move(it -> parent -> mid -> mid);
                    }
                }
                else if(it -> parent -> keys[1] < it -> keys[0]) // Right child of a 3 node
                {
                    if(it -> parent -> mid -> isTwoNode)
                    {
                        it -> keys[0] = it -> parent -> mid -> keys[0];
                        it -> values[0] = it -> parent -> mid -> values[0];
                        it -> keys[1] = it -> parent -> keys[1];
                        it -> values[1] = it -> parent -> values[1];
                        it -> right = move(it -> left);
                        it -> mid = move(it -> parent -> mid -> right);
                        it -> left = move(it -> parent -> mid -> left);
                        if(it -> left)
                        {
                            it -> left -> parent = it;
                            it -> mid -> parent = it;
                        }
                        it -> parent -> isTwoNode = true;
                    } else{
                        it -> isTwoNode = true;
                        it -> keys[0] = it -> parent -> keys[1];
                        it -> values[0] = it -> parent -> values[1];
                        it -> parent -> keys[1] = it -> parent -> mid -> keys[1];
                        it -> parent -> values[1] = it -> parent -> mid -> values[1];
                        it -> parent -> mid -> isTwoNode = true;
                        
                        it -> right = move(it -> left);
                        it -> left = move(it -> parent -> mid -> right);
                        if(it -> left)
                            it -> left -> parent = it;
                        it -> parent -> mid -> right = move(it -> parent -> mid -> mid);
                    }
                } else{ // Middle child of a 3 node
                    if(it -> parent -> left -> isTwoNode && it -> parent -> right -> isTwoNode)
                    {
                        it -> keys[0] = it -> parent -> left -> keys[0];
                        it -> keys[1] = it -> parent -> keys[0];
                        it -> values[0] = it -> parent -> left -> values[0];
                        it -> values[1] = it -> parent -> values[0];
                        it -> right = move(it -> left);
                        it -> mid = move(it -> parent -> left -> right);
                        it -> left = move(it -> parent -> left -> left);
                        if(it -> left)
                        {
                            it -> mid -> parent = it;
                            it -> left -> parent = it;
                        }
                        it -> parent -> keys[0] = it -> parent -> keys[1];
                        it -> parent -> values[0] = it -> parent -> values[1];
                        it -> parent -> isTwoNode = true;
                        it -> parent -> left = move(it -> parent -> mid);
                    }
                    else if(it -> parent -> left -> isTwoNode && !it -> parent -> right -> isTwoNode)
                    {
                        it -> isTwoNode = true;
                        it -> keys[0] = it -> parent -> keys[1];
                        it -> parent -> keys[1] = it -> parent -> right -> keys[0];
                        it -> values[0] = it -> parent -> values[1];
                        it -> parent -> values[1] = it -> parent -> right -> values[0];
                        it -> parent -> right -> keys[0] = it -> parent -> right -> keys[1];
                        it -> parent -> right -> values[0] = it -> parent -> right -> values[1];
                        it -> parent -> right -> isTwoNode = true;
                        
                        it -> right = move(it -> parent -> right -> left);
                        if(it -> left)
                            it -> right -> parent = it;
                        it  -> parent -> right -> left = move(it -> parent -> right -> mid);
                    } else{
                        it -> isTwoNode = true;
                        it -> keys[0] = it -> parent -> keys[0];
                        it -> values[0] = it -> parent -> values[0];
                        it -> parent -> keys[0] = it -> parent -> left -> keys[1];
                        it -> parent -> values[0] = it -> parent -> left -> values[1];
                        it -> parent -> left -> isTwoNode = true;
                        
                        it -> right = move(it -> left);
                        it -> left = move(it -> parent -> left -> right);
                        if(it -> left)
                            it -> left -> parent = it;
                        it -> parent -> left -> right = move(it -> parent -> left -> mid);
                    }
                }
            }
        }
        
    }
    
    // Uses 2-3 tree style traversal to display the dictionary
    // Time: O(n)
    void displayHelper(Node23<K, V> *it) const
    {
        if(it)
        {
            if(it -> isTwoNode)
            {
                displayHelper(it -> left.get());
                cout << it -> keys[0] << "->" << it -> values[0] << " ";
                displayHelper(it -> right.get());
            } else{
                displayHelper(it -> left.get());
                cout << it -> keys[0] << "->" << it -> values[0] << " ";
                displayHelper(it -> mid.get());
                cout << it -> keys[1] << "->" << it -> values[1] << " ";
                displayHelper(it -> right.get());
            }
        }
    }
    
    int sizeHelper(Node23<K, V> *it) const
    {
        if(!it)
            return 0;
        else{
            if(it -> isTwoNode)
                return 1 + sizeHelper(it -> left.get()) + sizeHelper(it -> right.get());
            else
                return 2 + sizeHelper(it -> left.get()) + sizeHelper(it -> mid.get()) + sizeHelper(it -> right.get());
        }
    }
    
    int heightHelper(Node23<K, V> *it) const
    {
        if(!it)
            return 0;
        else
            return 1 + heightHelper(it -> left.get());
    }
    
    // Finds the node of the desired key and returns true if it is found
    // Time: O(logn)
    bool keyExistsHelper(K target, Node23<K, V> *it) const
    {
        if(it == nullptr)
        {
            return false;
        }
        else if(it -> isTwoNode)
        {
            if(target == it -> keys[0])
                return true;
            else if(target < it -> keys[0])
                return keyExistsHelper(target, it -> left.get());
            else
                return keyExistsHelper(target, it -> right.get());
        } else{
            if(target == it -> keys[0])
                return true;
            else if(target == it -> keys[1])
                return true;
            else if(target < it -> keys[0])
                return keyExistsHelper(target, it -> left.get());
            else if(target > it -> keys[1])
                return keyExistsHelper(target, it -> right.get());
            else
                return keyExistsHelper(target, it -> mid.get());
        }
    }
    
    void copy(Node23<K, V> *copyPtr)
    {
        if(copyPtr == nullptr)
            copyPtr = nullptr;
        else{
            if(copyPtr -> isTwoNode)
            {
                copy(copyPtr -> left.get());
                add(copyPtr -> keys[0], copyPtr -> values[0]);
                copy(copyPtr -> right.get());
            } else{
                copy(copyPtr -> left.get());
                add(copyPtr -> keys[0], copyPtr -> values[0]);
                copy(copyPtr -> mid.get());
                add(copyPtr -> keys[1], copyPtr -> values[1]);
                copy(copyPtr -> right.get());
            }
        }
    }
    
    // Creates a temporary 4 node
    // Time: O(1)
    void createFourNode(Node23<K, V> *it, K key, V value) const
    {
        if(key < it -> keys[0])
        {
            it -> keys[2] = it -> keys[1];
            it -> keys[1] = it -> keys[0];
            it -> keys[0] = key;
            it -> values[2] = it -> values[1];
            it -> values[1] = it -> values[0];
            it -> values[0] = value;
        }
        else if(key > it -> keys[1])
        {
            it -> keys[2] = key;
            it -> values[2] = value;
        } else{
            it -> keys[2] = it -> keys[1];
            it -> keys[1] = key;
            it -> values[2] = it -> values[1];
            it -> values[1] = value;
        }
    }
    
    // Creates a 2 node
    // Time: O(1)
    unique_ptr<Node23<K, V>> createTwoNode(K key, V value) const
    {
        auto leaf = unique_ptr<Node23<K, V>>(new Node23<K, V>);
        leaf -> keys[0] = key;
        leaf -> values[0] = value;
        leaf -> isTwoNode = true;
        return leaf;
    }
};

#endif

