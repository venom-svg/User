#include <iostream>
#include <vector>
#include <limits>
#include <iomanip>
#include <stdexcept>
#include <climits>  // <-- Add this line to fix the error


using namespace std;

struct Block {
    int size;
    bool allocated;
};

struct Process {
    int size;
    int allocatedBlock;
    int fragmentation;
};

// Function to get valid positive integer input
int getValidInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value <= 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a positive integer.\n";
        } else {
            return value;
        }
    }
}

// First Fit Allocation Method
/*first fit allocate the block which is free from the starting*/
void firstFit(vector<Block>& blocks, vector<Process>& processes) {
    for (auto& process : processes) {
        for (int i = 0; i < blocks.size(); i++) {
            if (!blocks[i].allocated && blocks[i].size >= process.size) {
                process.allocatedBlock = i + 1;
                process.fragmentation = blocks[i].size - process.size;
                blocks[i].allocated = true;
                break;
            }
        }
    }
}

// Best Fit Allocation Method
void bestFit(vector<Block>& blocks, vector<Process>& processes) {
    for (auto& process : processes) {
        int bestIdx = -1, minFragment = INT_MAX;
        for (int i = 0; i < blocks.size(); i++) {
            if (!blocks[i].allocated && blocks[i].size >= process.size) {
                int frag = blocks[i].size - process.size;
                if (frag < minFragment) {
                    minFragment = frag;
                    bestIdx = i;
                }
            }
        }
        if (bestIdx != -1) {
            process.allocatedBlock = bestIdx + 1;
            process.fragmentation = minFragment;
            blocks[bestIdx].allocated = true;
        }
    }
}

// Next Fit Allocation Method
void nextFit(vector<Block>& blocks, vector<Process>& processes) {
    int lastAllocated = 0;
    int n = blocks.size();

    for (auto& process : processes) {
        int i = lastAllocated;
        int count = 0;

        while (count < n) {
            if (!blocks[i].allocated && blocks[i].size >= process.size) {
                process.allocatedBlock = i + 1;
                process.fragmentation = blocks[i].size - process.size;
                blocks[i].allocated = true;
                lastAllocated = i;
                break;
            }
            i = (i + 1) % n;
            count++;
        }
    }
}

// Worst Fit Allocation Method
void worstFit(vector<Block>& blocks, vector<Process>& processes) {
    for (auto& process : processes) {
        int worstIdx = -1;
        int maxFragment = -1;

        for (int i = 0; i < blocks.size(); i++) {
            if (!blocks[i].allocated && blocks[i].size >= process.size) {
                int frag = blocks[i].size - process.size;
                if (frag > maxFragment) {
                    maxFragment = frag;
                    worstIdx = i;
                }
            }
        }

        if (worstIdx != -1) {
            process.allocatedBlock = worstIdx + 1;
            process.fragmentation = blocks[worstIdx].size - process.size;
            blocks[worstIdx].size -= process.size;
        }
    }
}
int calculateTotalFragmentation(const vector<Process>& processes) {
    int total = 0;
    for (const auto& p : processes) {
        if (p.fragmentation != -1) total += p.fragmentation;
    }
    return total;
}

void analyzeAlgorithms(const vector<Block>& blocks, const vector<Process>& processes) {
    vector<string> algoNames = {"First Fit", "Best Fit", "Next Fit", "Worst Fit"};
    vector<int> fragmentations(4);
    
    for (int i = 0; i < 4; ++i) {
        vector<Block> tempBlocks = blocks;
        vector<Process> tempProcesses = processes;

        switch (i) {
            case 0: firstFit(tempBlocks, tempProcesses); break;
            case 1: bestFit(tempBlocks, tempProcesses); break;
            case 2: nextFit(tempBlocks, tempProcesses); break;
            case 3: worstFit(tempBlocks, tempProcesses); break;
        }

        fragmentations[i] = calculateTotalFragmentation(tempProcesses);
    }

    cout << "\n--- Fragmentation Analysis ---\n";
    for (int i = 0; i < 4; i++) {
        cout << algoNames[i] << ": Total Fragmentation = " << fragmentations[i] << "\n";
    }

    int minFrag = fragmentations[0];
    string bestAlgo = algoNames[0];
    for (int i = 1; i < 4; ++i) {
        if (fragmentations[i] < minFrag) {
            minFrag = fragmentations[i];
            bestAlgo = algoNames[i];
        }
    }

    cout << "\nBest Allocation Strategy: " << bestAlgo << " (Minimum Fragmentation = " << minFrag << ")\n";
}

// Function to Print Allocation Results
void printResults(const vector<Process>& processes) {

    cout << "| Process | Size | Block No  | Fragmentation    |\n";
 
    for (int i = 0; i < processes.size(); i++) {
        cout << "|    P" << i + 1 
             << "    | " << setw(5) << processes[i].size 
             << " | " << setw(10);
        if (processes[i].allocatedBlock != -1)
            cout << processes[i].allocatedBlock << "    | " << setw(14) << processes[i].fragmentation << "   |";
        else
            cout << "Not Allocated | " << setw(14) << "-" << "   |";
        cout << "\n";
    }
}

// Reset Block Allocation Status
void resetBlocks(vector<Block>& blocks) {
    for (auto& block : blocks) {
        block.allocated = false;
    }
}

int main() {
    try {
        int numBlocks = getValidInput("Enter number of blocks: ");
        vector<Block> blocks(numBlocks);

        cout << "Enter sizes of blocks: ";
        for (int i = 0; i < numBlocks; i++) {
            cin >> blocks[i].size;
            if (cin.fail() || blocks[i].size <= 0) {
                throw runtime_error("Block sizes must be positive integers.");
            }
            blocks[i].allocated = false;
        }

        int numProcesses = getValidInput("Enter number of processes: ");
        vector<Process> processes(numProcesses);

        cout << "Enter sizes of processes: ";
        for (int i = 0; i < numProcesses; i++) {
            cin >> processes[i].size;
            if (cin.fail() || processes[i].size <= 0) {
                throw runtime_error("Process sizes must be positive integers.");
            }
            processes[i].allocatedBlock = -1;
            processes[i].fragmentation = -1;
        }

        int choice;
do {
    vector<Block> tempBlocks = blocks;
    vector<Process> tempProcesses = processes;

    cout << "\nChoose an Allocation Strategy:\n";
    cout << "1. First Fit\n2. Best Fit\n3. Next Fit\n4. Worst Fit\n5. Analyze All\n6. Exit\n";
    cout << "Enter your choice: ";
    cin >> choice;

    if (cin.fail() || choice < 1 || choice > 6) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid choice! Please enter a number between 1 and 6.\n";
        continue;
    }

    switch (choice) {
        case 1:
            cout << "\nApplying First Fit Allocation...\n";
            firstFit(tempBlocks, tempProcesses);
            printResults(tempProcesses);
            break;
        case 2:
            cout << "\nApplying Best Fit Allocation...\n";
            bestFit(tempBlocks, tempProcesses);
            printResults(tempProcesses);
            break;
        case 3:
            cout << "\nApplying Next Fit Allocation...\n";
            nextFit(tempBlocks, tempProcesses);
            printResults(tempProcesses);
            break;
        case 4:
            cout << "\nApplying Worst Fit Allocation...\n";
            worstFit(tempBlocks, tempProcesses);
            printResults(tempProcesses);
            break;
        case 5:
            analyzeAlgorithms(blocks, processes);
            break;
        case 6:
            cout << "Exiting program...\n";
            break;
    }
} while (choice != 6);

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\nProgram terminated.\n";
    }

    return 0;
}

