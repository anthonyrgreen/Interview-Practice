#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <sstream>
#include <ctime>
#include <string>
#include <utility>

#define ULLONG unsigned long long int

using namespace std;

template <typename T>
struct Node {
  Node(T elt, int height) : elt(elt), left(0), right(0), height(height) {}
  T elt;
  Node<T> * left;
  Node<T> * right;
  int height; // Invariant: abs(x.left->height - x.right->height) <= 1
  void updateHeight();
};
template<typename T> 
int getHeight(Node<T> * root) 
  { return root ? root->height : -1; }
template<typename T> 
string printNode(Node<T> * root) {
  stringstream ss;
  if (root) {
    ss << root->elt;
    string out = ss.str();
    return out;
  }
  else {
    return "X";
  }
  //  return root ? itoa(root->elt) : "X"; }
}
template<typename T> 
int balance(Node<T> * root) 
  { return getHeight(root->right) - getHeight(root->left); }
template<typename T> 
void Node<T>::updateHeight() 
  { height = max(getHeight(left), getHeight(right)) + 1; }

template<typename T> Node<T> * leftRotate(Node<T> * root);
template<typename T> Node<T> * rightRotate(Node<T> * root);
template<typename T> Node<T> * mapUp(Node<T> * root, function<void(T)> fun);

/* An AVL tree. Allows element multiplicity. */
template <class T>
class AVL {
  private:
    Node<T> * root;
  public:
    AVL() : root(0) {}
    AVL(T elt) { root = new Node<T>(elt, 0); }
    AVL(std::vector<T> in);
    ~AVL();
    void insert(T elt);
    void remove(T elt);
    bool search(T const &elt);
    bool empty();
    std::vector<T> inOrder() const;
    std::vector<T> preOrder() const;
    std::vector<T> postOrder() const;
    char ** printState();
};

template <typename T>
AVL<T>::AVL(std::vector<T> in) : root(0) {
  //cout << "starting constructor loop..." << endl;
  for(auto it = in.begin(); it != in.end(); it++) {
    //cout << "inserting element " << it - in.begin() << endl;
    insert(*it);
  }
  //cout << "ending constructor loop..." << endl;
}

template <typename T>
AVL<T>::~AVL<T>() {
  mapUp(root, [](Node<T> * root) -> void { if(root) cout << "deleting " << printNode(root) << endl; if(root) delete root; root = 0; });
  root = 0;
}
  //insertPath.push_back(root);
  //while (insertPath.back())
  //  insertPath.push_back(cursor->elt < elt ? cursor->left : cursor->right)
  //// We've inserted a null pointer into the last index of insertPath
  //insertPath.pop_back();

template <typename T>
void AVL<T>::insert(T const elt) {
  //cout << "inserting " << elt ;
  if (empty()) {
    //cout << "...into root." << endl;
    root = new Node<T>(elt, 0);
    return;
  }
  vector<Node<T> *> insertPath;
  Node<T> * cursor = root;
  Node<T> * newCursor = root;

  // Build the insert path, root-down
  do {
    insertPath.push_back(cursor);
    //if (elt < cursor->elt)
      //cout << "L";
    //else
      //cout << "R";
    cursor = elt < cursor->elt ? cursor->left : cursor->right;
  } while (cursor);
  //cout << "_X_" << endl;

  // Insert the new element
  Node<T> * insertNode = new Node<T>(elt, 0);
  Node<T> * parent = insertPath.back();
  if (elt < parent->elt) {
    //cout << "inserting " << elt << " into " << printNode(parent) << " left.";
    parent->left = insertNode;
  }
  else {
    //cout << "inserting " << elt << " into " << printNode(parent) << " right.";
    parent->right = insertNode;
  }
  parent->updateHeight();
  //cout << printNode(parent) << " height: " << getHeight(parent) << endl;
  insertPath.pop_back();

  //vector<int> outOfOrderNumbers  { 5, 2, 8, 3, 386, 22, 23, 1, 53 };

  // Update the heights, going upwards to the root, rotating as we need
  while (!insertPath.empty()) {
    cursor = insertPath.back();
    insertPath.pop_back();
    //cout << "Inspecting " << printNode(cursor) << "..." << endl;

    if (balance(cursor) > 1) {
      assert(balance(cursor->right) != 0);
      //cout << "\tright balanced..." << endl;
      if (balance(cursor->right) < 0) {
        //cout << "\t right-left balanced..." << endl;
        // WILL THIS EVER RESULT IN A SEG FAULT?
        //cout << "\tRight of " << printNode(cursor);
        //cout << " rightRotate : " << printNode(cursor->right);
        cursor->right = rightRotate<T>(cursor->right);
        //cout << " now equals " << printNode(cursor->right) << "..." << endl;
      }
      ////cout << printNode(cursor);
      ////cout << " leftRotate : " << printNode(cursor);
      ////cout << " leftRotate : " << printNode(cursor->right);
      //cursor->right = rightRotate<T>(cursor->right);
      ////cout << " now equals " << printNode(cursor) << "..." << endl;
      
      //cout << " leftRotate : " << printNode(cursor);
      cursor = leftRotate(cursor);
      //cout << " now equals " << printNode(cursor) << "..." << endl;
      //cursor = leftRotate(cursor->right);
    } else if (balance(cursor) < -1) {
      assert(balance(cursor->left) != 0);
      //cout << "\tright balanced..." << endl;
      if (balance(cursor->left) > 0) {
        //cout << "\t left-right balanced..." << endl;
        //cout << "\tLeft of " << printNode(cursor);
        //cout << " leftRotate : " << printNode(cursor->left);
        cursor->left = leftRotate(cursor->left);
        ////cout << " now equals " << printNode(cursor->left) << "..." << endl;
      }
      ////cout << printNode(cursor);

      //cout << " rightRotate : " << printNode(cursor);
      cursor = rightRotate(cursor);
      //cout << " now equals " << printNode(cursor) << "..." << endl;
      //cursor = rightRotate(cursor->right);
    }

    cursor->updateHeight();
    if (!insertPath.empty()) {
      if (cursor->elt < insertPath.back()->elt)
        insertPath.back()->left = cursor;
      else
        insertPath.back()->right = cursor;
    }
    else
      root = cursor;
  }
}

template <typename T>
void AVL<T>::remove(T const elt) {

}

template <typename T>
bool _search(Node<T> * root, T const &elt) {
  return (root && root->elt == elt)
         || _search(root->left, elt) 
         || _search(root->right, elt);
}

template <typename T>
bool AVL<T>::search(T const &elt) {
  return _search(root, elt);
}

template <typename T>
bool AVL<T>::empty() {
  return !root;
}

template <typename T>
void _inOrder(vector<T> &list, Node<T> const * root) {
  if (!root) 
    return;

  _inOrder(list, root->left);
  list.push_back(root->elt);
  _inOrder(list, root->right);
  return;
}

template <typename T>
vector<T> AVL<T>::inOrder() const {
  vector<T> list;
  _inOrder(list, root);
  return list;
}

template <typename T>
void _preOrder(vector<T> &list, Node<T> const * root) {
  if (!root) 
    return;

  list.push_back(root->elt);
  _inOrder(list, root->left);
  _inOrder(list, root->right);
  return;
}

template <typename T>
vector<T> AVL<T>::preOrder() const {
  vector<T> list;
  _preOrder(list, root);
  return list;
}

template <typename T>
void _postOrder(vector<T> &list, Node<T> const * root) {
  if (!root) 
    return;

  _inOrder(list, root->left);
  _inOrder(list, root->right);
  list.push_back(root->elt);
  return;
}

template <typename T>
vector<T> AVL<T>:: postOrder() const {
  vector<T> list;
  _postOrder(list, root);
  return list;
  
}

template <typename T>
char ** AVL<T>::printState() {
  // first determine width and height of tree.
  return 0;
}

template <typename T>
Node<T> * rightRotate(Node<T> * root) {
  // root and left must be non-empty
  assert(root && root->left);
  Node<T> * left = root->left;
  root->left = left->right;
  left->right = root;
  root->updateHeight();
  left->updateHeight();
  return left;
}

template <typename T>
Node<T> * leftRotate(Node<T> * root) {
  // root and right must be non-empty
  assert(root && root->right);
  Node<T> * right = root->right;
  root->right = right->left;
  right->left = root;
  root->updateHeight();
  right->updateHeight();
  return right;
}

template<typename T, typename Func>
Node<T> * _mapInOrder(Node<T> * root, Func fun) {
  // recurse along left and right subtrees
  if (root->left)
    _mapInOrder(root->left, fun);
  fun(root);
  if (root->right)
    _mapInOrder(root->right, fun);
  return root;
}
template<typename T, typename Func>
Node<T> * mapInOrder(Node<T> * root, Func fun) {
  if (root)
    return _mapInOrder(root, fun);
  else
    return root;
}

template<typename T, typename Func>
Node<T> * _mapUp(Node<T> * root, Func fun) {
  // recurse along left and right subtrees
  if (root->left)
    _mapUp(root->left, fun);
  if (root->right)
    _mapUp(root->right, fun);
  fun(root);
  return root;
}
template<typename T, typename Func>
Node<T> * mapUp(Node<T> * root, Func fun) {
  if (root)
    return _mapUp(root, fun);
  else
    return root;
}

template<typename T>
void printArray(vector<T> &in) {
  for (auto it = in.begin(); it != in.end(); it++)
    cout << *it << " ";
  //cout << endl;
}

template<typename T>
bool isInOrder(AVL<T> const &in) {
  vector<T> list = in.inOrder();
  for (auto it = list.begin() + 1; it < list.end(); it++) {
    if (*it < *(it - 1))
      return false;
  }
  return true;
}

int main() {
  vector<int> inOrderNumbers { 2, 6, 7, 9, 10, 23, 245, 25443, 234235 };
  vector<int> outOfOrderNumbers  { 5, 2, 8, 3, 386, 22, 23, 1, 53 };

  //cout << "Construction 1..." << endl;
  AVL<int> inOrderTree(inOrderNumbers);
  //cout << "Construction 2..." << endl;
  //AVL<int> outOfOrderTree(outOfOrderNumbers);

  //vector<int> inOrderTreeInOrder = inOrderTree.inOrder();
  //vector<int> outOfOrderTreeInOrder = outOfOrderTree.inOrder();
  
  //assert(isInOrder<int>(inOrderTree));
  //assert(isInOrder<int>(outOfOrderTree));
  
  //AVL<int> randomTree;
  //for (int i = 0; i < 100000; i++)
  //  randomTree.insert(rand());
  //assert(isInOrder<int>(randomTree));

//  AVL<ULLONG> timeTest;
//  for (ULLONG i = 10; i < 1000000000; i *= 10) {
//    clock_t start = clock();
//    for (ULLONG j = 0; j < i; j++) {
//      timeTest.insert(rand());
//    }
//    clock_t end = clock();
//    pair<ULLONG, double> test(i, (double)(end - start) / (double(CLOCKS_PER_SEC)));
//    cout << "(" << test.first << ":\t" << test.second << "):\t";
//    cout << double(test.first) / test.second << " inserts/sec" << endl;
//  }
  return 0;
}
