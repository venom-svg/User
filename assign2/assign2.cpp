#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <queue>
using namespace std;

struct Task {
    int id, arrival, burst, completion, waiting, turnaround, priority, remaining;
};

// Function declarations
void FCFS(vector<Task>& tasks, int n);
void SJF(vector<Task>& tasks, int n);
void SRTF(vector<Task>& tasks, int n);
void roundRobin(vector<Task>& tasks, int n, int quantum);
void priorityScheduling(vector<Task>& tasks, int n);
void preemptivePriorityScheduling(vector<Task>& tasks, int n);
void calculateTimes(vector<Task>& tasks, int n);
void printGanttChart(vector<pair<int, int>> &timeline);
void displayReadyQueue(queue<int> readyQueue);

int main() {
    int n, choice, quantum;
    cout << "Enter number of Processes: ";
    cin >> n;
    vector<Task> tasks(n);

    for (int i = 0; i < n; i++) {
        tasks[i].id = i + 1;
        cout << "Enter arrival time for task " << i + 1 << ": ";
        cin >> tasks[i].arrival;
        cout << "Enter burst time for task " << i + 1 << ": ";
        cin >> tasks[i].burst;
        tasks[i].remaining = tasks[i].burst;
        tasks[i].waiting = tasks[i].turnaround = tasks[i].completion = 0;
    }
    while(true)
    {
    cout << "\nSelect Scheduling Algorithm:\n";
    cout << "1. FCFS\n2. SJF\n3. SRTF (Preemptive SJF)\n4. Round Robin\n5. Priority Scheduling\n6. Preemptive Priority Scheduling\n";
    cin >> choice;

    switch (choice) {
        case 1: FCFS(tasks, n); break;
        case 2: SJF(tasks, n); break;
        case 3: SRTF(tasks, n); break;
        case 4:
            cout << "Enter time quantum for Round Robin: ";
            cin >> quantum;
            roundRobin(tasks, n, quantum);
            break;
        case 5:
            for (int i = 0; i < n; i++) {
                cout << "Enter priority for task " << i + 1 << ": ";
                cin >> tasks[i].priority;
            }
            priorityScheduling(tasks, n);
            break;
        case 6:
            for (int i = 0; i < n; i++) {
                cout << "Enter priority for task " << i + 1 << ": ";
                cin >> tasks[i].priority;
            }
            preemptivePriorityScheduling(tasks, n);
            break;
        default:
            cout << "Invalid choice!\n";
            break;
    }
    }
    return 0;
}
void displayReadyQueue(queue<int> readyQueue) {
    cout << "Ready Queue: ";
    if (readyQueue.empty()) {
        cout << "Empty";
    } else {
        queue<int> temp = readyQueue;
        while (!temp.empty()) {
            cout << "P" << temp.front() << " ";
            temp.pop();
        }
    }
    cout << endl;
}

void FCFS(vector<Task>& tasks, int n) {
    sort(tasks.begin(), tasks.end(), [](Task a, Task b) { return a.arrival < b.arrival; });
    vector<pair<int, int>> timeline;
    queue<int> readyQueue;
    int time = 0;

    for (auto &task : tasks) {
        while (!readyQueue.empty() && time < task.arrival) {
            displayReadyQueue(readyQueue);
            time++;
        }

        if (time < task.arrival) time = task.arrival;
        readyQueue.push(task.id);
        displayReadyQueue(readyQueue);

        timeline.push_back({task.id, time});
        time += task.burst;
        task.completion = time;
        task.turnaround = task.completion - task.arrival;
        task.waiting = task.turnaround - task.burst;
        readyQueue.pop();
    }
    timeline.push_back({-1, time});
    printGanttChart(timeline);
    calculateTimes(tasks, n);
}


void SJF(vector<Task>& tasks, int n) {
    int time = 0, completed = 0;
    vector<bool> done(n, false);
    vector<pair<int, int>> timeline;

    while (completed < n) {
        int idx = -1;
        int shortest = INT_MAX;

        // Find task with minimum burst among arrived & not completed
        for (int i = 0; i < n; ++i) {
            if (!done[i] && tasks[i].arrival <= time && tasks[i].burst < shortest) {
                shortest = tasks[i].burst;
                idx = i;
            }
        }

        if (idx == -1) {
            // No process has arrived yet
            time++;
        } else {
            timeline.push_back({tasks[idx].id, time});
            time += tasks[idx].burst;
            tasks[idx].completion = time;
            tasks[idx].turnaround = tasks[idx].completion - tasks[idx].arrival;
            tasks[idx].waiting = tasks[idx].turnaround - tasks[idx].burst;
            done[idx] = true;
            completed++;
        }
    }

    timeline.push_back({-1, time});
    printGanttChart(timeline);
    calculateTimes(tasks, n);
}

void SRTF(vector<Task>& tasks, int n) {
    int time = 0, completed = 0;
    vector<pair<int, int>> timeline;
    queue<int> readyQueue;

    while (completed < n) {
        int idx = -1, shortest = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (tasks[i].arrival <= time && tasks[i].remaining > 0 && tasks[i].remaining < shortest) {
                shortest = tasks[i].remaining;
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }

        readyQueue.push(tasks[idx].id);
        displayReadyQueue(readyQueue);
        timeline.push_back({tasks[idx].id, time});
        tasks[idx].remaining--;

        if (tasks[idx].remaining == 0) {
            tasks[idx].completion = time + 1;
            tasks[idx].turnaround = tasks[idx].completion - tasks[idx].arrival;
            tasks[idx].waiting = tasks[idx].turnaround - tasks[idx].burst;
            completed++;
            readyQueue.pop();
        }
        time++;
    }
    timeline.push_back({-1, time});
    printGanttChart(timeline);
    calculateTimes(tasks, n);
}


void roundRobin(vector<Task>& tasks, int n, int quantum) {
    vector<pair<int, int>> timeline;
    queue<int> readyQueue;
    int time = 0, completed = 0;
    vector<bool> visited(n, false);

    // Sort by arrival time
    sort(tasks.begin(), tasks.end(), [](Task a, Task b) {
        return a.arrival < b.arrival;
    });

    // Push first task (if arrived)
    if (tasks[0].arrival == 0) {
        readyQueue.push(0);
        visited[0] = true;
    }

    while (completed < n) {
        if (!readyQueue.empty()) {
            int idx = readyQueue.front();
            readyQueue.pop();

            int execTime = min(quantum, tasks[idx].remaining);
            timeline.push_back({tasks[idx].id, time});
            time += execTime;
            tasks[idx].remaining -= execTime;

            // Enqueue newly arrived tasks during this execution
            for (int i = 0; i < n; ++i) {
                if (!visited[i] && tasks[i].arrival <= time) {
                    readyQueue.push(i);
                    visited[i] = true;
                }
            }

            if (tasks[idx].remaining > 0) {
                readyQueue.push(idx);  // Put back in queue
            } else {
                tasks[idx].completion = time;
                tasks[idx].turnaround = tasks[idx].completion - tasks[idx].arrival;
                tasks[idx].waiting = tasks[idx].turnaround - tasks[idx].burst;
                completed++;
            }

           // Print Ready Queue state (show up to 3, excluding running process)
    queue<int> tempQueue = readyQueue;
    vector<int> displayed;
    while (!tempQueue.empty()) {
        int id = tasks[tempQueue.front()].id;
        if (find(displayed.begin(), displayed.end(), id) == displayed.end()) {
            displayed.push_back(id);
        }
        tempQueue.pop();
}

cout << "Ready Queue: ";
if (displayed.empty()) {
    cout << "Empty";
} else {
    for (size_t i = 0; i < min(displayed.size(), size_t(3)); ++i) {
        cout << "P" << displayed[i] << " ";
    }
}
cout << endl;


        } else {
            // CPU idle, increment time and check if any task arrives
            time++;
            for (int i = 0; i < n; ++i) {
                if (!visited[i] && tasks[i].arrival <= time) {
                    readyQueue.push(i);
                    visited[i] = true;
                }
            }
        }
    }

    timeline.push_back({-1, time});
    printGanttChart(timeline);
    calculateTimes(tasks, n);
}


void priorityScheduling(vector<Task>& tasks, int n) {
    sort(tasks.begin(), tasks.end(), [](Task a, Task b) {
        return a.priority < b.priority;
    });
    FCFS(tasks, n);
}

void preemptivePriorityScheduling(vector<Task>& tasks, int n) {
    int time = 0, completed = 0;
    vector<pair<int, int>> timeline;
    queue<int> readyQueue;
    
    while (completed < n) {
        int idx = -1, highestPriority = INT_MAX;

        // Find the process with the highest priority (lowest priority number)
        for (int i = 0; i < n; i++) {
            if (tasks[i].arrival <= time && tasks[i].remaining > 0 && tasks[i].priority < highestPriority) {
                highestPriority = tasks[i].priority;
                idx = i;
            }
        }

        // If no process is available, move time forward
        if (idx == -1) {
            time++;
            continue;
        }

        // Add the selected process to the ready queue and display it
        readyQueue.push(tasks[idx].id);
        displayReadyQueue(readyQueue);

        // Execute the selected process
        timeline.push_back({tasks[idx].id, time});
        tasks[idx].remaining--;

        // If the process is completed, update its completion time
        if (tasks[idx].remaining == 0) {
            tasks[idx].completion = time + 1;
            tasks[idx].turnaround = tasks[idx].completion - tasks[idx].arrival;
            tasks[idx].waiting = tasks[idx].turnaround - tasks[idx].burst;
            completed++;
            readyQueue.pop();  // Remove from the ready queue after execution
        }

        time++;
    }

    timeline.push_back({-1, time});
    printGanttChart(timeline);
    calculateTimes(tasks, n);
}


void calculateTimes(vector<Task>& tasks, int n) {
    double totalWaiting = 0, totalTurnaround = 0;
    cout << "\nTask ID\tArrival Time\tBurst Time\tCompletion Time\tWaiting Time\tTurnaround Time\n";
    for (auto &task : tasks) {
        cout << task.id << "\t" << task.arrival << "\t\t" << task.burst << "\t\t" 
             << task.completion << "\t\t" << task.waiting << "\t\t" << task.turnaround << endl;
        totalWaiting += task.waiting;
        totalTurnaround += task.turnaround;
    }
    cout << "Average Waiting Time: " << totalWaiting / n << endl;
    cout << "Average Turnaround Time: " << totalTurnaround / n << endl;
}

void printGanttChart(vector<pair<int, int>> &timeline) {
    cout << "\nGantt Chart:\n";
    for (auto t : timeline) {
        if (t.first != -1) cout << "| P" << t.first << " ";
    }
    cout << "|\n";
    
    for (auto t : timeline) {
        cout << t.second << "    ";
        }
}
