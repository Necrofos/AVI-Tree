#include "AVL.h"
#include <iostream>
#include <cassert>
#include <utility>
#include <cstdint>
#include <string>
#include <stack>
#include <exception>
#include <stdexcept>
#include <iterator>
BinarySearchTree::BinarySearchTree(const BinarySearchTree& other){
  _size = other._size;
  if (other._root != nullptr){
    _root = new Node(*other._root);
  }
  else{
    _root = nullptr;
  }
}

BinarySearchTree& BinarySearchTree::operator=(const BinarySearchTree &other) {
    if (this != &other) {
        _size = other._size; 
        if (other._root != nullptr) { 
            _root = new Node(*other._root);
        } else {
            _root = nullptr;
        }
    }
    return *this;
}

BinarySearchTree::BinarySearchTree(BinarySearchTree&& other) noexcept{
  _root = other._root;
  _size = other._size;
  other._root = nullptr;
  other._size = 0;
}

BinarySearchTree& BinarySearchTree::operator=(BinarySearchTree&& other) noexcept {
    if (this != &other) {
        _size = other._size;
        _root = other._root;
        other._root = nullptr;
        other._size = 0;
    }
    return *this;
}


void BinarySearchTree::Node::Delete_Nodes(Node* top){
    if (top!= nullptr){
        Delete_Nodes(top->left);
        delete top->left;
        Delete_Nodes(top->right);
        delete top->right;
    }
}

BinarySearchTree::~BinarySearchTree() {
    if (_root != nullptr) {
        _root->Delete_Nodes(_root);
        delete _root;
    }
}
void BinarySearchTree::Node::small_left_rotation() {
    std::pair<Key, Value> copy = right->keyValuePair;
    right->keyValuePair = keyValuePair;
    keyValuePair = copy;
    Node *copy_right = right;
    right = right->right;
    if (right != nullptr) {
        right->parent = this;
    }
    copy_right->right = copy_right->left;
    copy_right->left = left;
    if (left != nullptr) {
        left->parent = copy_right;
    }
    left = copy_right;
    left->parent = this;

    update_height();
}

void BinarySearchTree::Node::small_right_rotation() {
    std::pair<Key, Value> copy = left->keyValuePair;
    left->keyValuePair = keyValuePair;
    keyValuePair = copy;
    Node *copy_left = left;
    left = left->left;
    if (left != nullptr) {
        left->parent = this;
    }
    copy_left->left = copy_left->right;
    copy_left->right = right;
    if (right != nullptr) {
        right->parent = copy_left;
    }
    right = copy_left;
    right->parent = this;

    update_height();

}

void BinarySearchTree::Node::big_left_rotation() {
    right->small_right_rotation();
    small_left_rotation();
}

void BinarySearchTree::Node::big_right_rotation() {
    left->small_left_rotation();
    small_right_rotation();
}

void BinarySearchTree::Node::update_height() {
    m_height = 1 + std::max(
            left == nullptr ? -1 : left->m_height,
            right == nullptr ? -1 : right->m_height);
    if (left != nullptr) {
        left->m_height = 1 + std::max(
                left->left == nullptr ? 0 : left->left->m_height,
                left->right == nullptr ? 0 : left->right->m_height
        );
    }
    if (right != nullptr) {
        right->m_height = 1 + std::max(
                right->left == nullptr ? 0 : right->left->m_height,
                right->right == nullptr ? 0 : right->right->m_height
        );
    }
}

void BinarySearchTree::Node::balance() {
    update_height();

    int left_height = left == nullptr ? 0 : left->m_height;
    int right_height = right == nullptr ? 0 : right->m_height;
    if (left_height - right_height > 1) {
        left->update_height();
        int left_left_height = left->left == nullptr ? 0 : left->left->m_height;
        int left_right_height = left->right == nullptr ? 0 : left->right->m_height;
        if (left_left_height - left_right_height > 0) {
            small_right_rotation();
        } else {
            big_right_rotation();
        }
    } else if (right_height - left_height > 1) {
        right->update_height();
        int right_left_height = right->left == nullptr ? 0 : right->left->m_height;
        int right_right_height = right->right == nullptr ? 0 : right->right->m_height;
        if (right_right_height - right_left_height > 0) {
            small_left_rotation();
        } else {
            big_left_rotation();
        }
    }
}


BinarySearchTree::ConstIterator BinarySearchTree::find(const Key &key) const {
    Node *current = _root;

    while (current != nullptr) { 

        if (key > current->keyValuePair.first) {
            current = current->right;
        } else if (key < current->keyValuePair.first) {
            current = current->left;
        } else if (key == current->keyValuePair.first) {
            return ConstIterator(current);
        } else {
            return cend();
        }
    }
    return ConstIterator(nullptr);
}

BinarySearchTree::Iterator BinarySearchTree::find(const Key &key) {
    Node *current = _root;

    while (current != nullptr) {

        if (current->right && key > current->keyValuePair.first ) {
            current = current->right;
        } else if (current->left && key < current->keyValuePair.first) {
            current = current->left;
        } else if (key == current->keyValuePair.first) {
            return Iterator(current);
        } else
            return end();
    }
    return end();
}

size_t BinarySearchTree::size() const{
  return _size;
}
    
BinarySearchTree::ConstIterator BinarySearchTree::min() const{
  Node *node = _root;
  while (node->left != nullptr){
    node = node->left;
  }
  return ConstIterator(node);
}

BinarySearchTree::ConstIterator BinarySearchTree::max() const {
  Node *node = _root; 
  while (node && node->right && !node->right->flag) {
    node = node->right; 
  }
  return ConstIterator(node);
}


BinarySearchTree::ConstIterator BinarySearchTree::min(const Key &key) const {
  Node *current = _root;
  Node *res = nullptr;
  Value min;
  int f = 1;

  while (current != nullptr)
  {
    if (key > current->keyValuePair.first)
    {
      current = current->right;
    } else if (key < current->keyValuePair.first) 
    {
      current = current->left;
    } else
    {
      if (current->keyValuePair.second < min || f)
      {
        min = current->keyValuePair.second;
        res = current;
        f = 0;         
      }
      current = current->right;
    }
  }
  return ConstIterator(res);
}

BinarySearchTree::ConstIterator BinarySearchTree::max(const Key &key) const {
  
 Node *current = _root;
  Node *res = nullptr;
  Value max;
  int f = 1;

  while (current != nullptr)
  {
    if (key > current->keyValuePair.first)
    {
      current = current->right;
    } else if (key <
               current->keyValuePair.first)
    {
      current = current->left;
    } else
    {
      if (current->keyValuePair.second > max ||f)
      {
        max = current->keyValuePair.second;
        res = current;
        f = 0;         
      }
      current = current->right;
    }
  }
  return ConstIterator(res);
}

BinarySearchTree::Iterator BinarySearchTree::begin(){
  Node* node = _root;
  if (node == nullptr){
    return Iterator(nullptr);
  }
  while (node->left != nullptr){
    node = node->left;
  }
  return Iterator(node);
}

BinarySearchTree::Iterator BinarySearchTree::end(){
  Node* node = _root;
  if (node == nullptr){
    return Iterator(nullptr);
  }
  while (node->right != nullptr){
    node = node->right;
  }
  return Iterator(node);
}

BinarySearchTree::ConstIterator BinarySearchTree::cbegin() const{
  Node* node = _root;
  if (node == nullptr){
    return ConstIterator(nullptr);
  }
  while (node->left != nullptr){
    node = node->left;
  }
  return ConstIterator(node);
}

BinarySearchTree::ConstIterator BinarySearchTree::cend() const {
    Node *current = _root;
    while (current && current->right) {
        current = current->right;
    }
    return ConstIterator(current);
}

BinarySearchTree::Node::Node(Key key, Value value, Node *parent, Node *left, Node *right, bool flag)
        : keyValuePair(std::make_pair(key, value)), parent(parent), left(left), right(right), flag(flag) {}

BinarySearchTree::Node::Node(const Node &other) {
    keyValuePair.first = other.keyValuePair.first;
    keyValuePair.second = other.keyValuePair.second;
    flag = other.flag; 
    if (other.left == nullptr) {
        left = nullptr;
    } else {
        left = new Node(*other.left); 
        left->parent = this;
    }
    if (other.right == nullptr) {
        right = nullptr;
    } else {
        right = new Node(*other.right);
        parent = other.parent;
        right->parent = this;

    }
}

bool BinarySearchTree::Node::operator==(const Node& other) const{
  if (keyValuePair == other.keyValuePair && parent == other.parent && left == other.left && right == other.right){
    return true;
  }
  return false;
}

BinarySearchTree::Iterator::Iterator(Node *node){
  _node = node;
}

std::pair<Key, Value> &BinarySearchTree::Iterator::operator*(){
  return _node->keyValuePair;
}

const std::pair<Key, Value> &BinarySearchTree::Iterator::operator*() const{
  return _node->keyValuePair;
}

std::pair<Key, Value> *BinarySearchTree::Iterator::operator->(){
  return &_node->keyValuePair;
}

const std::pair<Key, Value> *BinarySearchTree::Iterator::operator->() const{
  return &_node->keyValuePair;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++(){
  if (_node == nullptr){
    return Iterator(nullptr);
  }
  if (_node->right != nullptr){
    _node = _node->right;
    while(_node->left != nullptr){
      _node = _node->left;
    }
  }
  else{
    while(_node->parent != nullptr && _node == _node->parent->right){
      _node = _node->parent;
    }
    _node = _node->parent;
  }
  return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++(int){
  BinarySearchTree::Iterator *tmp = this;
  ++(*this);
  return *tmp;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--(){
  if (_node == nullptr){
    return Iterator(nullptr);
  }
  if (_node->left != nullptr){
    _node = _node->left;
    while(_node->right != nullptr){
      _node = _node->right;
    }
  }
  else{
    while(_node->parent != nullptr && _node == _node->parent->left){
      _node = _node->parent;
    }
    _node = _node->parent;
  }
  return *this;
}

BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--(int){
  BinarySearchTree::Iterator *tmp = this;
  --(*this);
  return *tmp;
}

bool BinarySearchTree::Iterator::operator==(const Iterator& other) const {
    return _node == other._node;
}

bool BinarySearchTree::Iterator::operator!=(const Iterator& other) const {
    return !(*this == other);
}

BinarySearchTree::ConstIterator::ConstIterator(const Node* node) {
    _node = node;
}
const std::pair<Key, Value>& BinarySearchTree::ConstIterator::operator*() const {
    return (*_node).keyValuePair;
}

const std::pair<Key, Value>* BinarySearchTree::ConstIterator::operator->() const {
    return &_node->keyValuePair;
}

BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++() {
    if (_node->right != nullptr) {
        _node = _node->right;
        while (_node->left != nullptr) {
            _node = _node->left;
        }
    }
    else {
        while (_node->parent != nullptr && _node == _node->parent->right) {
            _node = _node->parent;
        }
        _node = _node->parent;
    }
    return *this;
}
BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++(int) {
    BinarySearchTree::ConstIterator t(*this);
    ++(*this);
    return t;
}
BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--() {
    if (_node->left != nullptr) {
        _node = _node->left;
        while (_node->right != nullptr) {
            _node = _node->right;
        }
    }
    else {
        while (_node->parent != nullptr && _node == _node->parent->left) {
            _node = _node->parent;
        }
        _node = _node->parent;
    }
    return *this;
}
BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--(int) {
    BinarySearchTree::ConstIterator p = *this;
    --(*this);
    return p;
}
bool BinarySearchTree::ConstIterator::operator==(const ConstIterator& other) const {
    return _node == other._node;
}
bool BinarySearchTree::ConstIterator::operator!=(const ConstIterator& other) const {
    return !(*this == other);
}

std::pair<BinarySearchTree::Iterator, BinarySearchTree::Iterator>
BinarySearchTree::equalRange(const Key &key) {
   Iterator it_start = find(key);
    if (it_start == end()) {
        return std::make_pair(it_start, it_start);
    }
    Iterator it_tmp = it_start;
    Iterator it_end = it_start;
    it_tmp--;
    Iterator begin_it = begin();
    while (it_tmp->first == key && it_tmp != begin_it) {
        it_start--;
        it_tmp--;
    }
    it_tmp = it_end;
    it_tmp++;
    Iterator end_it = end();
    while (it_tmp->first == key && it_tmp != end_it) {
        it_end++;
        it_tmp++;
    }
    it_end++;
    return std::make_pair(it_start, it_end);
}

std::pair<BinarySearchTree::ConstIterator, BinarySearchTree::ConstIterator> BinarySearchTree::equalRange(const Key &key) const {
    ConstIterator begin = find(key);
    ConstIterator end = begin;
    if (begin != cend()) {
        while (end != cend() && end->first == key) {
            ++end;
        }
    }
    return std::make_pair(begin, end);
}

void BinarySearchTree::Node::insert(const Key &key, const Value &value) {
  if (key < keyValuePair.first)
  {
    if (left)
    {
      left->insert(key, value);
    } else
    {
      Node *node = new Node(key, value, this, nullptr, nullptr);
      this->left = node;
    }
  }
  if (key >= keyValuePair.first)
  {
    if (right)
    {
      right->insert(key, value);
    } 
    else
    {
      Node *node = new Node(key, value, this, nullptr, nullptr);
      this->right = node;
    }
  }
  balance();
}
void BinarySearchTree::insert(const Key &key, const Value &value) {
  if (_root) {
    delete_nill();
    _root->insert(key, value);
  } 
  else {
    _root = new Node(key, value);
  }
  insert_nill();
  _size++;
}


BinarySearchTree::Node *BinarySearchTree::Node::erase_node(Node *node,
                                                           const Key &key) {
  if (node == nullptr)
    return node;

  if (key < node->keyValuePair.first)
  {
    node->left = erase_node(node->left, key);
    node->balance();
    return node;
  } else if (key > node->keyValuePair.first)
  {
    node->right = erase_node(node->right, key);
    node->balance();
    return node;
  }

  if (node->left == nullptr) {
    Node *temp = node->right;
    delete node;
    return temp;
  } else if (node->right == nullptr) {
    Node *temp = node->left;
    delete node;
    return temp;
  }

  Node *temp = node->right;
  Node *parent = node;

  while (temp->left != nullptr) {
    parent = temp;
    temp = temp->left;
  }

  node->keyValuePair = temp->keyValuePair;

  if (parent->left == temp)
    parent->left = temp->right;
  else
    parent->right = temp->right;

  delete temp;
  node->balance();
  return node;
}

void BinarySearchTree::erase(const Key &key) {
  if (_root) {
    delete_nill();
    _root = _root->erase_node(_root, key);
    _size--;
    insert_nill();
  }
  Iterator f = find(key);
  if (f != end()) {
    erase(key);
  }
}
void BinarySearchTree::delete_nill() {
  Node *current = _root;

  if (!current)
    return;

  while (current->right) {
    current = current->right;
  }

  if (current != nullptr && current->flag) {
    current->parent->right = nullptr;
    delete current;
  }
}

void BinarySearchTree::insert_nill() {
  if (_root) {
    Node *nil = new Node(0, 0);
    nil->flag = true;

    Node *current = _root;

    while (current->right) {
      current = current->right;
    }

    if (!current->flag) {
      current->right = nil;
      nil->parent = current;
    }
  }
}
