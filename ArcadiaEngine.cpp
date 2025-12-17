// ArcadiaEngine.cpp - STUDENT TEMPLATE


#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <random>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable {
private:
   struct PlayerData{
     int playerID;
     string name;
     bool isEmpty;

     PlayerData(){
       playerID = -1;
       isEmpty = true;
       name = "";
     }
   };

    int size;
    int count;
    PlayerData PlayerTable[101];

   int h1(int key){
     return key % size;
   }

   int h2(int key){
     return 97 - (key % 26);
   }

public:
    ConcretePlayerTable() {
        size = 101;
        count = 0;
    }

    void insert(int playerID, string name) override {
        int hash1 = h1(playerID);
        int hash2 = h2(playerID);

        for(int i = 0; i < size; i++) {
            int index = (hash1 + i * hash2) % size;

            if (PlayerTable[index].isEmpty) {
                PlayerTable[index].playerID = playerID;
                PlayerTable[index].name = name;
                PlayerTable[index].isEmpty = false;
                count++;
                return;
            }
        }
        cout << "Table is Full" << endl;
    }

    string search(int playerID) override {
        int hash1 = h1(playerID);
        int hash2 = h2(playerID);

        for(int i = 0; i < size; i++) {
            int index = (hash1 + i * hash2) % size;
            if (PlayerTable[index].isEmpty) {
                return "";
            }
            if (PlayerTable[index].playerID == playerID) {
                return PlayerTable[index].name;
            }
        }
        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers
    struct Node {
        int playerID;
        int score;
        vector<Node*> forward;
        Node(int id, int sc, int level) : playerID(id), score(sc), forward(level, nullptr) {}
    };
    int MAX_LEVEL = 16;
    float P = 0.5;
    Node* head;
    int level; // max height we have rn
    int randomLevel() {
        int lvl = 1;
        while ((rand()%2) == 1 && lvl < MAX_LEVEL) {
            lvl++;
        }
        return lvl;
    }
public:
    ConcreteLeaderboard() {
        // TODO: Initialize your skip list
        this->level = 0;
        this->head = new Node(-1,-1,MAX_LEVEL);
    }

    void addScore(int playerID, int score) override {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
        vector<Node*> update(MAX_LEVEL, nullptr);
        Node* current = head;
        for (int i = level; i >= 0; i--) {
            while (current->forward[i] != nullptr &&
                   (current->forward[i]->score > score ||
                    (current->forward[i]->score == score && current->forward[i]->playerID < playerID))) {
                current = current->forward[i];
            }
            update[i] = current;
        }
        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level + 1; i < newLevel; i++) {
                update[i] = head;
            }
            level = newLevel;
        }
        Node* newNode = new Node(playerID, score, newLevel);
        for (int i = 0; i < newLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }

    }

    void removePlayer(int playerID) override {
        // TODO: Implement skip list deletion
        //linear search for the player using id
        Node* t = head->forward[0];
        while (t != nullptr && t->playerID != playerID) {
            t = t->forward[0];
        }
        if (t == nullptr) return;
        int tScore = t->score;
        //predecessors
        vector<Node*> update(MAX_LEVEL, nullptr);
        Node* current = head;
        for (int i = level; i >= 0; i--) {
            while (current->forward[i] != nullptr &&
                   current->forward[i]->score > tScore) {
                current = current->forward[i];
            }
            //handle ties
            while (current->forward[i] != nullptr &&
                   current->forward[i]->score == tScore &&
                   current->forward[i] != t) {
                current = current->forward[i];
            }
            update[i] = current;
        }
        for (int i = 0; i <= level; i++) {
            if (update[i]->forward[i] == t) {
                update[i]->forward[i] = t->forward[i];
            }
        }
        delete t;
        //in case the node was an only node for some levels
        while (level > 0 && head->forward[level] == nullptr) {
            level--;
        }
    }

    vector<int> getTopN(int n) override {
        // TODO: Return top N player IDs in descending score order
        int c = 0;
        Node* temp = head->forward[0];

        while (temp != nullptr && c < n) {
            ++c;
            temp = temp->forward[0];
        }
        vector<int> result(c);
        temp = head->forward[0];
        for (int i = 0; i < c; i++) {
            result[i] = temp->playerID;
            temp = temp->forward[0];
        }
        return result;
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
    // TODO: Define your Red-Black Tree node structure
    // Hint: Each node needs: id, price, color, left, right, parent pointers
    enum Color{
        RED,
        BLACK,
        DBLACK
    };
    class Node {
    public:
        int id;
        int price;
        Node* left;
        Node* right;
        Node* parent;
        Color color;
        Node(int p_id,int p_price,  Color p_color = Color::RED) :
                id{p_id}, price{p_price}, left{nullptr}, right{nullptr}, parent{nullptr}, color{p_color}
        {}
    };
    Node* root;
    Node* insert_node(int itemId, int price) {
        Node* node = new Node(itemId, price);
        Node* parent = root;
        Node* current = root;
        while(current != nullptr) {
            parent = current;
            if(current->price > node->price) {
                current = current->left;
            } else if(current->price == node->price && current->id > node->id) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        if (root) {
            if (parent->price > node->price) {
                parent->left = node;
            } else if(parent->price == node->price && parent->id > node->id) {
                parent->left = node;
            } else {
                parent->right = node;
            }
            node->parent = parent;
        } else {
            root = node;
        }
        return node;
    }
    Node* get_uncle(Node* node) {
        Node* grandparent = node->parent->parent;
        if(grandparent->left == node->parent) {
            return grandparent->right;
        }
        return grandparent->left;
    }
    void rotate_left(Node* node) {
        Node* parent = node->parent;
        Node* grandparent = parent->parent;
        // Step 1: Adopt node's left child as parent's right child
        parent->right = node->left;
        if(node->left)
            node->left->parent = parent;

        // Step 2: Make parent the left child of node
        node->left = parent;
        parent->parent = node;

        // Step 3: Replace parent with node in the tree structure
        node->parent = grandparent;
        if(grandparent != nullptr) {
            if(grandparent->left == parent)
                grandparent->left = node;
            else
                grandparent->right = node;
        } else {
            root = node;
        }
    }
    void rotate_right(Node* node) {
        Node* parent = node->parent;
        Node* grandparent = parent->parent;
        // Step 1: Adopt node's right child as parent's left child
        parent->left = node->right;
        if(node->right)
            node->right->parent = parent;
        // Step 2: Make parent the right child of node
        node->right = parent;
        parent->parent = node;

        // Step 3: Replace parent with node in the tree structure
        node->parent = grandparent;
        if(grandparent != nullptr) {
            if(grandparent->left == parent)
                grandparent->left = node;
            else
                grandparent->right = node;
        } else {
            root = node;
        }
    }
    void fix_insertion_violation(Node* node) {
        while(node != root and node->parent->color == RED) {
            Node* grandparent = node->parent->parent;
            Node* parent = node->parent;
            Node* uncle = get_uncle(node);
            bool parent_is_left = (grandparent->left == parent);
            bool is_triangle = parent_is_left ? (node == parent->right) : (node == parent->left);
            if(uncle != nullptr && uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                node = grandparent;
            } else {
                if (is_triangle) {
                    if (parent_is_left) {
                        rotate_left(node);
                    } else {
                        rotate_right(node);
                    }
                    std::swap(node,parent);
                }
                if(parent_is_left) {
                    rotate_right(parent);
                } else {
                    rotate_left(parent);
                }
                std::swap(grandparent, parent);
                grandparent->color = BLACK;
                parent->color = RED;
                break;
            }
        }
        root->color = BLACK;
    }
    Node* find_successor(Node* node) {
        while(node->left != nullptr) {
            node = node->left;
        }
        return node;
    }
    Color get_color(Node* node) {
        return (node == nullptr) ? BLACK : node->color;
    }
    void fix_double_black(Node* node, Node* parent) {
        while(node != this->root ) {
            Node* sibling = (parent->left == node) ? parent->right : parent->left;
            bool x_is_left = (parent->left == node);

            if(sibling == nullptr ) {
                cout << "Error: Tree is invalid\n";
                break;
            }
            // Case 1
            if(sibling->color == RED) {
                sibling->color = BLACK;
                parent->color = RED;
                if(x_is_left) {
                    rotate_left(sibling);
                    sibling = parent->right;
                } else {
                    rotate_right(sibling);
                    sibling = parent->left;
                }
            }
            Node* near_child = x_is_left ? sibling->left : sibling->right;
            Node* far_child = x_is_left ? sibling->right : sibling->left;
            // Case 2
            if(get_color(near_child) == BLACK && get_color(far_child) == BLACK) {
                sibling->color = RED;
                if(parent->color == RED) {
                    parent->color = BLACK;
                    break;
                }
                node = parent;
                parent = node->parent;
                continue;
            }
            // Case 3
            if(get_color(far_child) == BLACK) {
                if(x_is_left) {
                    rotate_right(near_child);
                } else {
                    rotate_left(near_child);
                }
                near_child->color = BLACK;
                sibling->color = RED;
                sibling = near_child;
                far_child = x_is_left ? sibling->right : sibling->left;
            }
            // Case 4
            if(x_is_left) {
                rotate_left(sibling);
            } else {
                rotate_right(sibling);
            }
            sibling->color = parent->color;
            parent->color = BLACK;
            far_child->color = BLACK;
            break;
        }

        this->root->color = BLACK;
    }
    Node* search(Node* current, int id, bool& found) {
        if(current != nullptr && current->id == id) {
            found = true;
            return current;
        }
        search(current->left, id, found);
        if(!found) search(current->right, id, found);
        return nullptr;
    }
public:
    ConcreteAuctionTree() {
        root = nullptr;
    }

    void insertItem(int itemID, int price) override {
        // Remember to maintain RB-Tree properties with rotations and recoloring
        Node* node = insert_node(itemID, price);
        fix_insertion_violation(node);
    }

    void deleteItem(int itemID) override {
        // This is complex - handle all cases carefully
        bool found = false;
        Node* node_to_delete = search(this->root, itemID, found);
        if(!found) {
            std::cout << "Item with ID: " << itemID << " doesn't exist";
            return;
        }
        Node* actual_node = nullptr; // node with at most 1 child
        Node* replacement = nullptr; // child that replaces it
        if (node_to_delete->left == nullptr || node_to_delete->right == nullptr) {
            actual_node = node_to_delete;
        } else {
            actual_node = find_successor(node_to_delete->right);
            node_to_delete->id = actual_node->id;
        }
        replacement = (actual_node->left != nullptr) ? actual_node->left : actual_node->right;
        if(replacement != nullptr) {
            replacement->parent = actual_node->parent;
        }
        if(actual_node->parent == nullptr) {
            root = replacement;
        } else if (actual_node == actual_node->parent->left) {
            actual_node->parent->left = replacement;
        } else {
            actual_node->parent->right = replacement;
        }
        if(actual_node->color == BLACK) {
            if(replacement != nullptr && replacement->color ==  RED) {
                replacement->color = BLACK;
            } else {
                fix_double_black(replacement, actual_node->parent);
            }
        }
        delete actual_node;
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    int totalSum = 0;

    // Calculate total sum
    for (int coin : coins) {
        totalSum += coin;
    }

    // DP array: dp[j] = can we make sum j?
    vector<bool> dp(totalSum + 1, false);
    dp[0] = true;  // Base case: we can always make sum 0

    // For each number in the array
    for (int i = 0; i < n; i++) {
        // Traverse from right to left to avoid using same element twice
        for (int j = totalSum; j >= coins[i]; j--) {
            if (dp[j - coins[i]]) {
                dp[j] = true;
            }
        }
    }

    // Find the largest sum <= totalSum/2 that is achievable
    int minDiff = totalSum;
    for (int j = totalSum / 2; j >= 0; j--) {
        if (dp[j]) {
            // One subset has sum j, other has sum (totalSum - j)
            minDiff = totalSum - 2 * j;
            break;
        }
    }

    return minDiff;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity
    return 0;
}

long long InventorySystem::countStringPossibilities(string s) {
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings
    return 0;
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    // edges are bidirectional
    if(source == dest) return true;
    vector<vector<int>> adj_list(n);
    for(const auto& edge : edges) {
        adj_list[edge[0]].push_back(edge[1]);
        adj_list[edge[1]].push_back(edge[0]);
    }
    vector<bool> discovered(n, false);
    queue<int> q;
    int v, path_exist;
    q.push(source);
    discovered[source] = true;
    while(!q.empty()) {
        v = q.front();
        q.pop();
        for(int y : adj_list[v]) {
            if(!discovered[y]) {
                discovered[y] = true;
                q.push(y);
            }
        }
    }
    return discovered[dest];
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    return -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

// Helper function to find the best task to schedule next
int findBestTask(const vector<int>& frequencies, const vector<int>& nextAvailableTime, int currentTime) {
    int bestTaskIndex = -1;
    int maxFreq= -1;

    for (int i = 0; i < frequencies.size(); i++) {
        if (frequencies[i] > 0) {
            if (nextAvailableTime[i] <= currentTime) {
                if (frequencies[i] > maxFreq) {
                    maxFreq = frequencies[i];
                    bestTaskIndex = i;
                }
            }
        }
    }
    return bestTaskIndex;
}

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    vector<int> frequencies(26, 0);
    vector<int> nextAvailableTime(26, 0);
    int currentTime = 0;
    int tasksRemaining = tasks.size();

    for(int i = 0; i < tasks.size(); i++) {
        frequencies[tasks[i] - 'A']++;
    }

    while(tasksRemaining > 0) {
        int bestTaskIndex = findBestTask(frequencies, nextAvailableTime, currentTime);
        if (bestTaskIndex != -1) {
            frequencies[bestTaskIndex]--;
            nextAvailableTime[bestTaskIndex] = currentTime + n + 1;
            tasksRemaining--;
        }
        currentTime++;
    }
    return currentTime;
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
    PlayerTable* createPlayerTable() { 
        return new ConcretePlayerTable(); 
    }

    Leaderboard* createLeaderboard() { 
        return new ConcreteLeaderboard(); 
    }

    AuctionTree* createAuctionTree() { 
        return new ConcreteAuctionTree(); 
    }
}
