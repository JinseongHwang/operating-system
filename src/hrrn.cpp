//
// Created by jinseonghwang on 21. 4. 7..
//

#include <bits/stdc++.h>

using namespace std;

const int MAX_PROCESS = 1001;

struct Process {
    int PID, arrival, usage;
    double priority;

    inline bool operator<(const Process& nxt) const {
        if (this->priority == nxt.priority) {
            return this->PID > nxt.PID;
        }
        return this->priority < nxt.priority;
    }
};

int N, currTime, result;
vector<Process> input;
vector<bool> isUsed(MAX_PROCESS, false);
priority_queue<Process, vector<Process> > pq;

void calcPriority() {
    queue<Process> readyToCalc;
    while (!pq.empty()) {
        readyToCalc.push(pq.top());
        pq.pop();
    }

    while (!readyToCalc.empty()) {
        Process currProcess = readyToCalc.front();
        readyToCalc.pop();

        int waiting = currTime - currProcess.arrival;
        currProcess.priority = double(waiting) / double(currProcess.usage);

        pq.push(currProcess);
    }
}

void getProcesses() {
    for (int i = 1; i < N; i++) {
        Process &currProcess = input[i];
        if (!isUsed[currProcess.PID]) {
            if (currProcess.arrival <= currTime) {
                pq.push(currProcess);
                isUsed[currProcess.PID] = true;
            }
            else {
                break;
            }
        }
    }

    for (int i = 1; pq.empty() && i < N; i++) {
        Process &currProcess = input[i];
        if (!isUsed[currProcess.PID]) {
            pq.push(currProcess);
            isUsed[currProcess.PID] = true;
        }
    }
}

int main() {

//    freopen("../markingData/sampleData5/5.inp", "r", stdin);
    freopen("hrrn.inp", "r", stdin);
    freopen("hrrn.out", "w", stdout);

    scanf("%d", &N);

    input.resize(N);
    for (int i = 0; i < N; i++) {
        Process inputProcess;
        scanf("%d %d", &inputProcess.arrival, &inputProcess.usage);
        inputProcess.PID = i;
        input[i] = inputProcess;
    }

    sort(input.begin(), input.end(), [](const Process &a, const Process &b) {
        if (a.arrival == b.arrival) {
            return a.PID < b.PID;
        }
        return a.arrival < b.arrival;
    });

    /* 첫 프로세스는 바로 실행한다. */
    Process firstProcess = input.front();
    currTime = firstProcess.arrival + firstProcess.usage;
    getProcesses();
    calcPriority();

    /* 이후 프로세스에 대한 처리 */
    while (!pq.empty()) {

        calcPriority();

        Process currProcess = pq.top();
        pq.pop();

//        printf("%d %d\n", currProcess.arrival, currProcess.usage);

        // 시간 점프가 발생한 경우
        if (currTime < currProcess.arrival) {
            currTime = currProcess.arrival + currProcess.usage;
        }
        else {
            result += (currTime - currProcess.arrival);
            currTime += currProcess.usage;
        }

        getProcesses();
    }

    printf("%d", result);

    return 0;
}
