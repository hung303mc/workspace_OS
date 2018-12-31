// C++ program to implement Shortest Job first 
#include<bits/stdc++.h> 
using namespace std; 

struct Process 
{ 
  int pid; // Process ID 
  int bt; // Burst Time 
}; 

// Function to find the waiting time for all 
// processes 
void findWaitingTime(const vector<Process>& proc, int n, vector<int>& wt) 
{ 
	// waiting time for first process is 0 
	wt[0] = 0; 

	// calculating waiting time 
	for (int i = 1; i < n ; i++ ) 
		wt[i] = proc[i-1].bt + wt[i-1] ; 
} 

// Function to calculate turn around time 
void findTurnAroundTime(const vector<Process> &proc, int n, 
						const vector<int>& wt, vector<int>& tat) 
{ 
	// calculating turnaround time by adding 
	// bt[i] + wt[i] 
	for (int i = 0; i < n ; i++) 
		tat[i] = proc[i].bt + wt[i]; 
} 

//Function to calculate average time 
void findavgTime(vector<Process> proc, int n) 
{ 
	vector<int> wt(n);
  vector<int> tat(n);
  int total_wt = 0;
  int total_tat = 0; 

	// Function to find waiting time of all processes 
	findWaitingTime(proc, n, wt);

	// Function to find turn around time for all processes 
	findTurnAroundTime(proc, n, wt, tat); 

	// Calculate total waiting time and total turn 
	// around time 
	for (int i = 0; i < n; i++) 
	{ 
		total_wt += wt[i]; 
		total_tat += tat[i]; 
	} 
  total_wt /= n;
  total_tat /= n;

	cout << "Average waiting time = " << total_wt << endl; 
	cout << "Average turn around time = " << total_tat << endl; 
} 

// Driver code 
int main() 
{ 
	vector<Process> proc = {{1, 6}, {2, 8}, {3, 7}, {4, 3}}; 
	int n = proc.size();

	// Sorting processes by burst time. 
	sort(proc.begin(), proc.end(), [](Process a, Process b){
    return a.bt < b.bt;
  }); 

	cout << "Order in which process gets executed\n"; 
	for (auto it: proc) 
    cout << it.pid <<" "; 
  cout << endl;

	findavgTime(proc, n); 
	return 0; 
} 
