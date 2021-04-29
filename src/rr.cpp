//
// Created by jinseonghwang on 21. 4. 29..
//

#include <bits/stdc++.h>

using namespace std;

struct Process {
    int PID, // 프로세스 ID
    startTime, // 시작 시간
    taskCnt, // CPU Burst + I/O 총 개수
    priority, // 실행 우선순위(낮을 수록 우선 처리)
    exeIdx, // 현재 실행중인 task index, 짝: CPU Burst, 홀: I/O
    sumTasks; // CPU Burst + I/O 총 합
    vector<int> tasks; // CPU Burst + I/O 순서대로

    bool operator<(const Process &nxt) const {
        if (this->priority == nxt.priority) { // 우선순위가 같다면
            return this->PID > nxt.PID; // PID가 작은 것이 위로
        }
        return this->priority > nxt.priority; // priority가 작을수록 위로
    }
};

int N, Q; // 프로세스 개수, Time Quantum
vector<Process> processList; // 프로세스 저장
vector<int> endTime; // 종료 시간 저장
vector<int> ioFinishTime; // I/O 작업이 끝나는 시간(다음 CPU 사용 시간)
set<int> finishedProcess; // 종료된 프로세스 번호를 저장
queue<int> readyQueue; // Ready Queue (PID만 저장)
priority_queue<Process, vector<Process> > pq;

void inputProcesses() {
    for (int i = 0; i < N; i++) {
        Process process;
        int start, taskCnt;
        scanf("%d %d", &start, &taskCnt);
        process.PID = i;
        process.startTime = start;
        process.taskCnt = taskCnt;
        process.priority = 1;
        process.exeIdx = 0;
        process.sumTasks = 0;
        process.tasks.resize(taskCnt);
        for (int currTask, j = 0; j < taskCnt; j++) {
            scanf("%d", &currTask);
            process.tasks[j] = currTask;
            process.sumTasks += currTask;
        }
        processList[i] = process;
    }
}

void vectorResizing() {
    processList.resize(N);
    endTime.resize(N);
    ioFinishTime.resize(N);
}

void toNextBurst(Process &process, const int &currTime) {
    int pid = process.PID;
    if (process.exeIdx == process.taskCnt - 1) {
        // 프로세스 종료 선언
        finishedProcess.insert(pid);
        endTime[pid] = currTime;
    }
    else {
        // 다음 CPU Burst가 있는 위치로 이동
        ioFinishTime[pid] = currTime + process.tasks[process.exeIdx + 1];
        process.exeIdx += 2;
    }
}

void run() {
    fill(ioFinishTime.begin(), ioFinishTime.end(), -1);
    int currTime = 0, remainedTimeQuantum = Q;
    while (1) {
        if (finishedProcess.size() == N) { // 모든 프로세스가 종료됨
            break;
        }

        for (int i = 0; i < N; i++) {
            if (processList[i].startTime == currTime) { // 지금 시작하는 프로세스
                pq.push(processList[i]);
            }
            else if (ioFinishTime[i] == currTime) { // 다시 시작해야하는 프로세스
                processList[i].priority = 2; // I/O를 마쳤기 때문에 우선순위는 2이다.
                ioFinishTime[i] = -1;
                pq.push(processList[i]);
            }
        }

        while (!pq.empty()) { // pq 순서에 맞춰서 Ready Queue 에 삽입한다.
            readyQueue.push(pq.top().PID);
            pq.pop();
        }

        /**
         * 해야할 일
         * 1. CPU Burst가 남았는데 아직 time quantum이 끝났다.
         *  자르고 우선순위 바꿔주고 다시 pq에 넣어준다.
         * 2. CPU Burst가 끝났는데 time quantum이 남았다. tq 채워주고 다음 burst 로 넘어가자
         * 3. 실행하고 I/O 시간 갱신
         * 4. exeIdx 랑 priority 잘 갱신하고 유지하기
         */

        currTime++;

        if (readyQueue.empty()) { // 당장 실행할 task가 없는 경우
//            currTime++;
            continue;
        }

        // 현재 실행되는 프로세스
        int pid = readyQueue.front();
        Process &currProcess = processList[pid];
        int idx = currProcess.exeIdx;

        currProcess.tasks[idx]--; // CPU Burst 1만큼 차감
        remainedTimeQuantum--; // 현재 Time Quantum 1만큼 차감

        /**
         * 1. TQ 랑 현재 CPU Burst 동시에 0이 된 경우 -> readyQueue에서 빼고 TQ 다시 채워주기
         * 2. TQ 남았지만 현재 CPU Burst 0이 된 경우 -> readyQueue에서 빼고 TQ 다시 채워주기
         * 3. TQ 0 됐지만 CPU Burst 남은 경우 -> 남은 만큼 다시 pq에 삽입
         * 4. 둘 다 남은 경우 -> 계속 진행
         */
        if (currProcess.tasks[idx] == 0 && remainedTimeQuantum == 0) {
            readyQueue.pop();
            remainedTimeQuantum = Q;
            toNextBurst(currProcess, currTime);
        }
        else if (currProcess.tasks[idx] == 0) {
            readyQueue.pop();
            remainedTimeQuantum = Q;
            toNextBurst(currProcess, currTime);
        }
        else if (remainedTimeQuantum == 0) {
            readyQueue.pop();
            remainedTimeQuantum = Q;
            currProcess.priority = 3;
            pq.push(currProcess);
        }
        else {}

        // 현재 시간을 1 증가시킨다.
//        currTime++;
    }
}

int main() {

//    freopen("../markingData/sampleData6/2.inp", "r", stdin);
//    freopen("../markingData/sampleData6/my.out", "w", stdout);

    freopen("rr.inp", "r", stdin);
    freopen("rr.out", "w", stdout);

    scanf("%d %d", &N, &Q);

    vectorResizing();
    inputProcesses();
    run();

    for (int i = 0; i < N; i++) {
        int turnaround = endTime[i] - processList[i].startTime;
        printf("%d %d\n", i + 1, turnaround - processList[i].sumTasks);
    }

    return 0;
}