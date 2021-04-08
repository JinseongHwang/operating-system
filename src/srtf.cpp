//
// Created by jinseonghwang on 21. 4. 7..
//

#include <bits/stdc++.h>

using namespace std;

const int MAX_LEN = 1001;

struct Process {
    int pid, arrival, usage;

    // CPU Burst 작은 값, 같다면 도착 시간 작은 값이 힙의 top에 저장되도록 정의
    bool operator<(const Process& nxt) const {
        if (this->usage == nxt.usage) {
            return this->arrival > nxt.arrival;
        }
        return this->usage > nxt.usage;
    }
};

priority_queue<Process, vector<Process> > pq; // running processes
int endTime[MAX_LEN], cpuBurst[MAX_LEN], currTime;

inline void usageMinusOne() {
    Process currTop = pq.top();
    pq.pop();
    currTop.usage--;
    pq.push(currTop);
}

int main() {
    /**
     * 각 프로세스의 남은 시간이 0이 되는 시간을 기록, 에서 arrival을 뺀다 -> turnaround time을 알아냄
     * turnaround time 에서 CPU Burst time 을 빼면 각 프로세스의 waiting time 을 알 수 있다.
     */

//    freopen("../markingData/sampleData4/1.inp", "r", stdin);
    freopen("srtf.inp", "r", stdin);
    freopen("srtf.out", "w", stdout);

    int N;
    scanf("%d", &N);

    vector<Process> input(N);
    for (int i = 0; i < N; i++) {
        Process inputProcess;
        scanf("%d %d", &inputProcess.arrival, &inputProcess.usage);
        inputProcess.pid = i;
        input[i] = inputProcess;
    }

    sort(input.begin(), input.end(), [](const Process &a, const Process &b) {
        if (a.arrival == b.arrival) {
            if (a.usage == b.usage) {
                return a.pid < b.pid;
            }
            return a.usage < b.usage;
        }
        return a.arrival < b.arrival;
    });

    for (int i = 0; i < N; i++) {
        cpuBurst[i] = input[i].usage;
    }

    Process firstProcess = input.front();
    pq.push(firstProcess);

    currTime = firstProcess.arrival;
    endTime[0] = firstProcess.arrival + firstProcess.usage;

    for (int i = 1; i < N; i++) {
        Process newProcess = input[i];

        while (!pq.empty() && currTime < newProcess.arrival) {
            currTime++;
            usageMinusOne();

            if (!pq.top().usage) {
                endTime[pq.top().pid] = currTime;
                pq.pop();
            }
        }
        pq.push(newProcess);
        currTime = newProcess.arrival;
    }
    while (!pq.empty()) {
        currTime++;
        usageMinusOne();

        if (!pq.top().usage) {
            endTime[pq.top().pid] = currTime;
            pq.pop();
        }
    }

    int result = 0;
    for (int i = 0; i < N; i++) {
//        printf("%d ~ %d\n", input[i].arrival, endTime[i]);
        int turnaround = endTime[i] - input[i].arrival;
        int waiting = turnaround - cpuBurst[i];
        result += waiting;
    }

    printf("%d", result);

    return 0;
}