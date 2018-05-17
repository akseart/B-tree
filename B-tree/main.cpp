//
//  main.cpp
//  B-tree
//
//  Created by Артем on 17.05.2018.
//  Copyright © 2018 Артем. All rights reserved.
//

#include <iostream>
#include <vector>

using namespace std;

#define typeOfNode int
class BTree;
class Node {
    bool leaf;
    vector<typeOfNode> key;
    vector<Node*> children;
    Node* parent;
    friend BTree;
public:
    Node(){};
};
class BTree{
private:
    int maxDegreeTree;
    Node *root;
    void deletedNode(Node *);
    void restruct(Node *node);
    void insert_to_node(int, Node*);
    bool searchKey(int key, Node*);
    bool printing(Node *);
public:
    BTree(int maxDegree);
    ~BTree();
    void insert(int insertedKey);
    void delKey(int deletedKey);
    bool search(int key);
    void print();
};
BTree::BTree(int maxDegree){
    maxDegreeTree = maxDegree;
    root = nullptr;
};
BTree::~BTree(){
    if (root != nullptr){
        deletedNode(root);
    }
}
void BTree::deletedNode(Node *node){
    if (node!=nullptr){
        for (int i=0; i<=node->children.size(); i++){
            try {
                if (node->children.at(i) != nullptr){
                    deletedNode(node->children[i]);
                } else{
                    delete(node);
                    break;
                }
            } catch (out_of_range) {
                
            }
           
        }
    }
}

void BTree::insert_to_node(int key, Node *node){
    node->key.push_back(key);
    node->children.push_back(nullptr);
    sort(node->key.begin(),node->key.end());
}
void BTree::insert(int insertedKey){
    if (root==nullptr) {
        Node *newRoot = new Node;
        newRoot->key.push_back(insertedKey);
        newRoot->children.push_back(nullptr);
        newRoot->children.push_back(nullptr);
        newRoot->leaf=true;
        newRoot->parent=nullptr;
        root=newRoot;
    } else {
        Node *ptr=root;
        while (ptr->leaf==false){ //выбор ребенка до тех пор, пока узел не будет являться листом
            for (int i=0; i<=ptr->key.size(); i++){ //перебираем все ключи
                    if (insertedKey < ptr->key[i]) {
                        ptr=ptr->children[i];
                        break;
                    }
                    if (insertedKey > ptr->key[i]) {
                        ptr=ptr->children[i+1]; //перенаправляем к самому последнему ребенку, если цикл не "сломался"
                        break;
                    }
            }
        }
        insert_to_node(insertedKey, ptr);
        
        while (ptr->key.size() == 2*maxDegreeTree){
            if (ptr == root){
                restruct(ptr);
                break;
            } else {
                restruct(ptr);
                ptr = ptr->parent;
            }
        }
    }
};
void BTree::restruct(Node *node){
    if (node->key.size() < (2*maxDegreeTree-1))
        return;
    //первый сын
    Node *child1 = new Node;
    int j;
    for (j=0; j<=maxDegreeTree-2; j++)
        child1->key.push_back(node->key.at(j));
    if(!node->leaf){ //Узел не лист
        for (int i=0; i<=(maxDegreeTree-1); i++){
            child1->children.push_back(node->children[i]);
            child1->children[i]->parent=child1;
        }
        child1->leaf=false;
    } else {
        child1->leaf=true;
    }
    
    //второй сын
    Node *child2 = new Node;
    for (int j=0; j<=(maxDegreeTree-1); j++)
        child2->key.push_back(node->key.at(j+maxDegreeTree));
    if(!node->leaf) {
        for (int i=0; i<=(maxDegreeTree); i++){
            child2->children.push_back(node->children[i+maxDegreeTree]);
            child2->children[i]->parent=child2;
        }
        child2->leaf=false;
    } else {
        child2->leaf=true;
    }
    
    //родитель
    if (node->parent==nullptr){ //если родителя нет, то это корень
        node->key[0]=node->key[maxDegreeTree-1];
        node->key.erase(node->key.begin()+1, node->key.end());
        node->children.erase(node->children.begin()+2, node->children.end());
        node->children[0]=child1;
        node->children[1]=child2;
        node->parent=nullptr;
        node->leaf=false;
        child1->parent=node;
        child2->parent=node;
    } else {
        insert_to_node(node->key[maxDegreeTree-1], node->parent);

        for (int i=0; i<=(2*maxDegreeTree); i++){
            if (node->parent->children[i] == node)
                node->parent->children[i] = nullptr;
        }
        for (int i=0; i<=(2*maxDegreeTree); i++){
            if (node->parent->children[i] == nullptr) {
                for (int j=(2*maxDegreeTree); j>(i+1); j--)
                    node->parent->children[j] = node->parent->children[j-1];
                node->parent->children[i+1]=child2;
                node->parent->children[i]=child1;
                break;
            }
        }
        child1->parent=node->parent;
        child2->parent=node->parent;
        node->parent->leaf=false;
        delete node;
    }
}
bool BTree::search(int Key){
    return searchKey(Key, this->root);
}
bool BTree::searchKey(int Key, Node *node){
    if (node!=nullptr){
        if (node->leaf==false){
            int i;
            for (i=0; i<=node->key.size(); i++){
                    if(Key == node->key[i])
                        return true;
                    if ((Key < node->key[i])){
                        return searchKey(Key, node->children[i]);
                        break;
                    }
            }
            return searchKey(Key, node->children[i]);
        } else {
            for(int j=0; j<=node->key.size(); j++)
                if (Key == node->key[j]) return true;
            return false;
        }
    } else return false;
}
void BTree::print(){
    printing(root);
}
bool BTree::printing(Node * node){
    if (node != nullptr){
        cout<<"|";
        if (node->leaf) {
            for (int i=0; i<node->key.size(); i++) {
                cout<<node->key[i]<<" ";
            }
            return true;
        }
        
        for (int i=0; i<node->key.size(); i++) {
            try {
                if(printing(node->children.at(i))){
                    cout<<"|";
                };
            } catch (out_of_range) {
                
            }
            cout.setf(ios::left);
            cout.width(4);
            cout<<node->key[i];
            cout.width();
            cout.unsetf(ios::left);
        }
        try {
            printing(node->children.back());
        } catch (out_of_range) {
            
        }
        
    }else{
        return false;
    }
    return true;
}

int main(int argc, const char * argv[]) {
//    srand(time(0));
    cout << "Введите размерность дерева\n";
    int t = 0;
    cin>>t;
    BTree tree(t);
    for (int i=0; i<15; i++) {
        int tmp =rand()%100;
        tree.insert(tmp);
        cout.setf(ios::left);
        cout.width(4);
        cout<<i;
        cout<<tmp;
        cout.width();
        cout.unsetf(ios::left);
        tree.print();
        cout<<endl;
    }
    tree.~BTree();
    
    return 0;
}
