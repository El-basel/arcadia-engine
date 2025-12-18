/**
 * main.cpp
 * Extended Happy + Edge Case Test Suite for ArcadiaEngine
 * Covers Parts A–D thoroughly
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <functional>
#include "ArcadiaEngine.h"

using namespace std;

// ==========================================
// FACTORY FUNCTIONS (LINKING)
// ==========================================
extern "C" {
PlayerTable* createPlayerTable();
Leaderboard* createLeaderboard();
AuctionTree* createAuctionTree();
}

// ==========================================
// TEST UTILITIES
// ==========================================
class StudentTestRunner {
    int count = 0;
    int passed = 0;
    int failed = 0;

public:
    void runTest(const string& testName, bool condition) {
        count++;
        cout << "TEST: " << left << setw(55) << testName;
        if (condition) {
            cout << "[ PASS ]";
            passed++;
        } else {
            cout << "[ FAIL ]";
            failed++;
        }
        cout << endl;
    }

    void printSummary() {
        cout << "\n==========================================" << endl;
        cout << "SUMMARY: Passed: " << passed << " | Failed: " << failed << endl;
        cout << "==========================================" << endl;
        cout << "TOTAL TESTS: " << count << endl;
    }
};

StudentTestRunner runner;

// ==========================================
// PART A: DATA STRUCTURES
// ==========================================

void test_PartA_DataStructures() {
    cout << "\n--- Part A: Data Structures ---" << endl;

    // ---------- PlayerTable ----------
    runner.runTest("PlayerTable: Insert & Search", [&]() {
        PlayerTable* t = createPlayerTable();
        t->insert(1, "Alice");
        bool ok = (t->search(1) == "Alice");
        delete t;
        return ok;
    }());

    runner.runTest("PlayerTable: Search non-existing", [&]() {
        PlayerTable* t = createPlayerTable();
        bool ok = (t->search(999) == "");
        delete t;
        return ok;
    }());

    runner.runTest("PlayerTable: Update existing key", [&]() {
        PlayerTable* t = createPlayerTable();
        t->insert(5, "A");
        t->insert(5, "B");
        bool ok = (t->search(5) == "B");
        delete t;
        return ok;
    }());

    runner.runTest("PlayerTable: Collision handling", [&]() {
        PlayerTable* t = createPlayerTable();
        t->insert(101, "A");
        t->insert(202, "B");
        t->insert(303, "C");
        bool ok = t->search(101) == "A" &&
                  t->search(202) == "B" &&
                  t->search(303) == "C";
        delete t;
        return ok;
    }());

    // ---------- Leaderboard ----------
    runner.runTest("Leaderboard: Basic ordering", [&]() {
        Leaderboard* b = createLeaderboard();
        b->addScore(1, 100);
        b->addScore(2, 200);
        auto top = b->getTopN(1);
        delete b;
        return (!top.empty() && top[0] == 2);
    }());

    runner.runTest("Leaderboard: Tie-break by ID", [&]() {
        Leaderboard* b = createLeaderboard();
        b->addScore(10, 500);
        b->addScore(20, 500);
        auto top = b->getTopN(2);
        delete b;
        return top.size() == 2 && top[0] == 10 && top[1] == 20;
    }());

    runner.runTest("Leaderboard: Remove player", [&]() {
        Leaderboard* b = createLeaderboard();
        b->addScore(1, 100);
        b->addScore(2, 200);
        b->removePlayer(2);
        auto top = b->getTopN(2);
        delete b;
        return top.size() == 1 && top[0] == 1;
    }());

    // ---------- AuctionTree ----------
    runner.runTest("AuctionTree: Insert items", [&]() {
        AuctionTree* t = createAuctionTree();
        t->insertItem(1, 100);
        t->insertItem(2, 50);
        delete t;
        return true;
    }());

    runner.runTest("AuctionTree: Delete non-existing", [&]() {
        AuctionTree* t = createAuctionTree();
        t->insertItem(1, 100);
        t->deleteItem(999);
        delete t;
        return true;
    }());
}

// ==========================================
// PART B: INVENTORY SYSTEM
// ==========================================

void test_PartB_Inventory() {
    cout << "\n--- Part B: Inventory System ---" << endl;

    runner.runTest("LootSplit: {1,2,4} -> 1", [&]() {
        vector<int> coins = {1,2,4};
        return InventorySystem::optimizeLootSplit(3, coins) == 1;
    }());

    runner.runTest("LootSplit: Perfect split", [&]() {
        vector<int> coins = {1,2,3};
        return InventorySystem::optimizeLootSplit(3, coins) == 0;
    }());

    runner.runTest("Knapsack: All fit", [&]() {
        vector<pair<int,int>> items = {{1,10},{2,20},{3,30}};
        return InventorySystem::maximizeCarryValue(10, items) == 60;
    }());

    runner.runTest("Knapsack: Zero capacity", [&]() {
        vector<pair<int,int>> items = {{1,10}};
        return InventorySystem::maximizeCarryValue(0, items) == 0;
    }());

    runner.runTest("ChatDecoder: 'uu' -> 2", [&]() {
        return InventorySystem::countStringPossibilities("uu") == 2;
    }());

    runner.runTest("ChatDecoder: 'uuuu' -> 5", [&]() {
        return InventorySystem::countStringPossibilities("uuuu") == 5;
    }());
}

// ==========================================
// PART C: WORLD NAVIGATOR
// ==========================================

void test_PartC_Navigator() {
    cout << "\n--- Part C: World Navigator ---" << endl;

    runner.runTest("PathExists: simple path", [&]() {
        vector<vector<int>> edges = {{0,1},{1,2}};
        return WorldNavigator::pathExists(3, edges, 0, 2);
    }());

    runner.runTest("PathExists: disconnected", [&]() {
        vector<vector<int>> edges = {{0,1}};
        return !WorldNavigator::pathExists(3, edges, 0, 2);
    }());

    runner.runTest("MinBribeCost: triangle", [&]() {
        vector<vector<int>> roads = {{0,1,10,0},{1,2,5,0},{0,2,20,0}};
        return WorldNavigator::minBribeCost(3,3,1,1,roads) == 15;
    }());

    runner.runTest("MinBribeCost: disconnected", [&]() {
        vector<vector<int>> roads = {{0,1,10,0}};
        return WorldNavigator::minBribeCost(3,1,1,1,roads) == -1;
    }());

    runner.runTest("BinarySum: line graph", [&]() {
        vector<vector<int>> roads = {{0,1,1},{1,2,2}};
        return WorldNavigator::sumMinDistancesBinary(3, roads) == "110";
    }());
    runner.runTest("BinarySum: Disconnected/Directed Check", [&]() {
        vector<vector<int>> roads = {{0,1,2}, {0,2,8}};
        string result = WorldNavigator::sumMinDistancesBinary(3, roads);

        // Debug output if it fails
        if (result != "1010") cout << "    -> Got: " << result << " (Expected 1010)" << endl;

        return result == "1010";
    }());
    

}

// ==========================================
// PART D: SERVER KERNEL
// ==========================================

void test_PartD_Kernel() {
    cout << "\n--- Part D: Server Kernel ---" << endl;

    runner.runTest("Scheduler: {A,A,B}, n=2", [&]() {
        vector<char> tasks = {'A','A','B'};
        return ServerKernel::minIntervals(tasks, 2) == 4;
    }());

    runner.runTest("Scheduler: no cooling", [&]() {
        vector<char> tasks = {'A','A','A'};
        return ServerKernel::minIntervals(tasks, 0) == 3;
    }());

    runner.runTest("Scheduler: all same", [&]() {
        vector<char> tasks = {'A','A','A'};
        return ServerKernel::minIntervals(tasks, 2) == 7;
    }());
}

// ==========================================
// MAIN
// ==========================================

int main() {
    cout << "Arcadia Engine - Extended Test Suite" << endl;
    cout << "--------------------------------" << endl;

    test_PartA_DataStructures();
    test_PartB_Inventory();
    test_PartC_Navigator();
    test_PartD_Kernel();

    runner.printSummary();
    return 0;
}