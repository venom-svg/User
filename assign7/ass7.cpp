#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <limits>

using namespace std;

// Modified printFrames to accept an annotation message.
void printFrames(const vector<int>& frames, const string& note = "") {
    cout << "[ ";
    for (int frame : frames) {
        if (frame == -1)
            cout << "- ";
        else
            cout << frame << " ";
    }
    cout << "]";
    if (!note.empty()) {
        cout << " (" << note << ")";
    }
    cout << endl;
}

int simulateFCFS(const vector<int>& pages, int capacity, bool ifprint) {
    vector<int> frames(capacity, -1);
    int pageFaults = 0, index = 0;
    if (ifprint)
        cout << "\n--- FCFS Simulation ---\n";
    for (int page : pages) {
        bool found = false;
        for (int f : frames) {
            if (f == page) {
                found = true;
                break;
            }
        }
        string note;
        if (!found) {
            frames[index] = page;
            index = (index + 1) % capacity;
            pageFaults++;
            note = "Page fault";
        } else {
            note = "Hit";
        }
        if (ifprint)
            printFrames(frames, note);
    }
    if (ifprint)
        cout << "Total Page Faults (FCFS): " << pageFaults << endl;
    return pageFaults;
}

int simulateLRU(const vector<int>& pages, int capacity, bool ifprint) {
    vector<int> frames;
    unordered_map<int, int> recent;
    int pageFaults = 0;
    if (ifprint)
        cout << "\n--- LRU Simulation ---\n";
    for (int i = 0; i < pages.size(); ++i) {
        int page = pages[i];
        auto it = find(frames.begin(), frames.end(), page);
        string note;
        if (it == frames.end()) {
            pageFaults++;
            note = "Page fault";
            if (frames.size() < capacity)
                frames.push_back(page);
            else {
                int lru_index = 0, min_time = i;
                for (int j = 0; j < frames.size(); ++j) {
                    if (recent[frames[j]] < min_time) {
                        min_time = recent[frames[j]];
                        lru_index = j;
                    }
                }
                frames[lru_index] = page;
            }
        } else {
            note = "Hit";
        }
        // Update the time stamp for the current page
        recent[page] = i;
        // Build a display vector that has size = capacity
        vector<int> display(capacity, -1);
        copy(frames.begin(), frames.end(), display.begin());
        if (ifprint)
            printFrames(display, note);
    }
    if (ifprint)
        cout << "Total Page Faults (LRU): " << pageFaults << endl;
    return pageFaults;
}

int simulateOptimal(const vector<int>& pages, int capacity, bool ifprint) {
    vector<int> frames;
    int pageFaults = 0;
    if (ifprint)
        cout << "\n--- Optimal Simulation ---\n";
    for (int i = 0; i < pages.size(); ++i) {
        int page = pages[i];
        string note;
        if (find(frames.begin(), frames.end(), page) == frames.end()) {
            pageFaults++;
            note = "Page fault";
            if (frames.size() < capacity) {
                frames.push_back(page);
            } else {
                int farthest = i + 1, index_to_replace = -1;
                for (int j = 0; j < frames.size(); ++j) {
                    int k;
                    for (k = i + 1; k < pages.size(); ++k) {
                        if (pages[k] == frames[j])
                            break;
                    }
                    if (k > farthest) {
                        farthest = k;
                        index_to_replace = j;
                    }
                    if (k == pages.size()) {
                        index_to_replace = j;
                        break;
                    }
                }
                if (index_to_replace == -1) index_to_replace = 0;
                frames[index_to_replace] = page;
            }
        } else {
            note = "Hit";
        }
        // Build a display vector that has size = capacity
        vector<int> display(capacity, -1);
        copy(frames.begin(), frames.end(), display.begin());
        if (ifprint)
            printFrames(display, note);
    }
    if (ifprint)
        cout << "Total Page Faults (Optimal): " << pageFaults << endl;
    return pageFaults;
}

void findBest(int fcfs, int lru, int opt) {
    cout << "\n--- Performance Summary ---\n";
    cout << "FCFS:    " << fcfs << " faults\n";
    cout << "LRU:     " << lru  << " faults\n";
    cout << "Optimal: " << opt  << " faults\n";

    int minFaults = min({fcfs, lru, opt});
    cout << "Best Algorithm: ";
    if (minFaults == opt) cout << "Optimal\n";
    else if (minFaults == lru) cout << "LRU\n";
    else cout << "FCFS\n";
}

int main() {
    int n, capacity;
    vector<int> pages;

    do {
        cout << "Enter number of pages (must be > 0): ";
        cin >> n;
        if (cin.fail() || n <= 0) {
            cout << "Invalid input! Please enter a positive integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (n <= 0);

    pages.resize(n);
    cout << "Enter page sequence: ";
    for (int i = 0; i < n; ++i) {
        do {
            cin >> pages[i];
            if (cin.fail() || pages[i] < 0) {
                cout << "Invalid input! Please enter a non-negative integer.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        } while (pages[i] < 0);
    }

    do {
        cout << "Enter number of frames (must be > 0): ";
        cin >> capacity;
        if (cin.fail() || capacity <= 0) {
            cout << "Invalid input! Please enter a positive integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (capacity <= 0);

    int fcfsFaults = simulateFCFS(pages, capacity, false);
    int lruFaults = simulateLRU(pages, capacity, false);
    int optFaults = simulateOptimal(pages, capacity, false);

    findBest(fcfsFaults, lruFaults, optFaults);

    while (true) {
        cout << "\n--- Menu ---\n";
        cout << "1. FCFS\n2. LRU\n3. Optimal\n4. Exit\n";
        cout << "Enter choice: ";
        int choice;
        cin >> choice;

        if (cin.fail() || choice < 1 || choice > 4) {
            cout << "Invalid choice! Please enter a number between 1 and 4.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1:
                simulateFCFS(pages, capacity, true);
                break;
            case 2:
                simulateLRU(pages, capacity, true);
                break;
            case 3:
                simulateOptimal(pages, capacity, true);
                break;
            case 4:
                cout << "Exiting...\n";
                return 0;
        }
    }

    return 0;
}
