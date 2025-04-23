#include <iostream>
#include <vector>
using namespace std;

int tr = 0;

class Process
{
public:
    int processID;
    int *allocatedResources;
    int *max;

    Process()
    {
        allocatedResources = new int[tr];
        max = new int[tr];
    }
};
class BankersAlgorithm
{
public:
    int n;
    Process *p;
    int *totalResources;

    void takeInput()
    {
        totalResources = new int[tr];

        cout << "Enter the total resources of each type (A, B, C, ....) : ";
        for (int i = 0; i < tr; i++)
        {
            cin >> totalResources[i];
        }

        cout << "Enter number of processes : ";
        cin >> n;

        p = new Process[n];

        cout << "Enter process information." << endl;

        for (int i = 0; i < n; i++)
        {
            cout << "Process " << i + 1 << ": " << endl;
            cout << "Allocated Resources : ";
            for (int j = 0; j < tr; j++)
            {
                cin >> p[i].allocatedResources[j];
            }
            cout << "Maximum Resources : ";
            for (int j = 0; j < tr; j++)
            {
                cin >> p[i].max[j];
            }
        }
    }

    void findSafeSequences(int *available, bool *visited, vector<int> &sequence, int **need)
    {
        bool found = false;

        for (int i = 0; i < n; i++)
        {
            if (!visited[i])
            {
                bool canExecute = true;
                for (int j = 0; j < tr; j++)
                {
                    if (need[i][j] > available[j])
                    {
                        canExecute = false;
                        break;
                    }
                }

                if (canExecute)
                {
                    // Simulate allocation
                    for (int j = 0; j < tr; j++)
                    {
                        available[j] += p[i].allocatedResources[j];
                    }

                    visited[i] = true;
                    sequence.push_back(i + 1);

                    findSafeSequences(available, visited, sequence, need);

                    // Backtrack
                    for (int j = 0; j < tr; j++)
                    {
                        available[j] -= p[i].allocatedResources[j];
                    }
                    visited[i] = false;
                    sequence.pop_back();

                    found = true;
                }
            }
        }

        if (!found && sequence.size() == n)
        {
            // All processes included, print safe sequence
            cout << "Safe sequence: ";
            for (int i = 0; i < n; i++)
            {
                cout << "P" << sequence[i] << "\t";
            }
            cout << endl;
        }
    }

    void algo()
    {
        // Calculate available resources
        int *available = new int[tr];
        for (int i = 0; i < tr; i++)
        {
            available[i] = totalResources[i];
        }

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < tr; j++)
            {
                available[j] -= p[i].allocatedResources[j];
            }
        }

        // Calculate need matrix
        int **need = new int *[n];      // Create 2D array dynamically (need[n][tr])
        for (int i = 0; i < n; i++)
        {
            need[i] = new int[tr];
            for (int j = 0; j < tr; j++)
            {
                need[i][j] = p[i].max[j] - p[i].allocatedResources[j];
            }
        }

        // Setup
        bool *visited = new bool[n]{false};
        vector<int> sequence;

        // Find all safe sequences
        findSafeSequences(available, visited, sequence, need);

    }
};
int main()
{
    cout << "Enter total number of resources : ";
    cin >> tr;

    BankersAlgorithm b;
    b.takeInput();
    cout << "----------------------------------" << endl;
    b.algo();
    return 0;
}