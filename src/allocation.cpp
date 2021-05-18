//
// Created by jinseonghwang on 21. 5. 17..
//

#include <bits/stdc++.h>

using namespace std;

#define MAX_MEMORY 1000

struct Process {
    int pid, // Process ID
    reqTime, // 요청 시간
    needTime, // 실행 시간
    size; // 크기
};

struct Segment {
    int address, // 시작 주소
    pid, // 적재된 프로세스의 id (없다면 -1 : hole)
    size; // 적재된 프로세스가 차지하고 있는 메모리 크기
};

int N; // 프로세스 총 개수
vector<Process> input; // 입력받은 프로세스를 저장
vector<Segment> memory; // 메모리 공간
vector<Process> waitingList; // 메모리 공간 부족으로 인한 대기 프로세스 저장
int TYPE; // 적용할 할당 알고리즘 타입


/**
 * 사용자(파일)로부터 입력받는다.
 */
void inputData() {
    scanf("%d", &N);
    input.resize(N);
    for (int i = 0; i < N; i++) {
        input[i].pid = i;
        scanf("%d", &input[i].reqTime);
        scanf("%d", &input[i].needTime);
        scanf("%d", &input[i].size);
    }
}

/**
 * 메모리 공간을 초기화한다.
 */
void initMemory() {
    memory.clear();
    memory.push_back({0, -1, MAX_MEMORY});
}

/**
 * hole (pid == -1) 인 메모리 내 세그먼트들을 하나의 세그먼트로 합친다.
 * 단, 프로세스 종료 후 pid를 -1로 바꿔주고 이 함수를 실행해야 함.
 */
void assembleHole() {
    queue<pair<int, Segment> > tmpQ; // index, Segment

    for (int i = 0; i < memory.size(); i++) {
        const Segment &currSegment = memory[i];
        if (currSegment.pid == -1) { // hole 을 발견했다.
            tmpQ.push(make_pair(i, currSegment));
        }
        else {
            if (tmpQ.size() >= 2) { // 2개 이상일 때 합친다.
                int frontIndex = tmpQ.front().first;
                int frontAddress = tmpQ.front().second.address;
                int sumSize = 0;
                while (!tmpQ.empty()) {
                    memory.erase(memory.begin() + frontIndex); // hole 제거
                    sumSize += tmpQ.front().second.size; // 사용 가능한 메모리 크기의 합을 구한다.
                    tmpQ.pop();
                }

                Segment assembled = {frontAddress, -1, sumSize}; // 합쳐진 세그먼트 생성
                memory.insert(memory.begin() + frontIndex, assembled); // 삽입

                // 삭제되면서 줄어든 벡터에서 index를 맞춰주기 위함
                i = frontIndex + 1;
            }
            else { // 1개일 때는 그냥 넘어간다.
                if (!tmpQ.empty()) tmpQ.pop();
            }
        }
    }
    // 큐에 2개 이상 남아있으면 다시 합친다. (위 코드와 동일)
    if (tmpQ.size() >= 2) { // 2개 이상일 때 합친다.
        int frontIndex = tmpQ.front().first;
        int frontAddress = tmpQ.front().second.address;
        int sumSize = 0;
        while (!tmpQ.empty()) {
            memory.erase(memory.begin() + frontIndex); // hole 제거
            sumSize += tmpQ.front().second.size; // 사용 가능한 메모리 크기의 합을 구한다.
            tmpQ.pop();
        }

        Segment assembled = {frontAddress, -1, sumSize}; // 합쳐진 세그먼트 생성
        memory.insert(memory.begin() + frontIndex, assembled); // 삽입
    }
}

/**
 * fit 함수로부터 세그먼트를 삽입할 위치와 프로세스 정보를 받아와서 직접 삽입하는 함수이다.
 */
void insertThat(const int &index, const Process &process) {
    if (memory[index].size == process.size) { // 알맞게 들어가는 경우
        memory[index].pid = process.pid;
    }
    else { // 적재된 프로세스와 나머지 공간으로 쪼개지는 경우
        memory[index].pid = process.pid;
        int need = process.size;
        memory.insert(
                memory.begin() + index + 1,
                {memory[index].address + need, -1, memory[index].size - need}
        );
        memory[index].size = need;
    }
}

/**
 * fit 함수들은 메모리 공간에서 pid 가 -1인 곳(hole)을 찾아서 프로세스를 삽입한다.
 * 프로세스를 삽입하면 Segment 가 하나 추가된다.
 * hole 을 찾아가는 방식은 각 fit 함수에 따라 상이하다.
 */
bool firstFit(const Process &process) {
    int insertIndex = -1;
    for (int i = 0; i < memory.size(); i++) {
        Segment segment = memory[i];
        if (segment.pid == -1 && process.size <= segment.size) {
            insertIndex = i; // 처음 하나 찾고
            break; // break
        }
    }
    if (insertIndex < 0) {
        return false;
    }
    else {
        insertThat(insertIndex, process);
        return true;
    }
}

bool bestFit(const Process &process) {
    int insertIndex = -1;
    int minDiff = MAX_MEMORY;
    for (int i = 0; i < memory.size(); i++) {
        Segment segment = memory[i];
        if (segment.pid == -1 && process.size <= segment.size) {
            int diff = segment.size - process.size;
            if (minDiff > diff) { // 가장 작은 차이를 가지는 index 를 구한다.
                minDiff = diff;
                insertIndex = i;
            }
        }
    }
    if (insertIndex < 0) {
        return false;
    }
    else {
        insertThat(insertIndex, process);
        return true;
    }
}

bool worstFit(const Process &process) {
    int insertIndex = -1;
    int maxDiff = -1;
    for (int i = 0; i < memory.size(); i++) {
        Segment segment = memory[i];
        if (segment.pid == -1 && process.size <= segment.size) {
            int diff = segment.size - process.size;
            if (maxDiff < diff) { // 가장 큰 차이를 가지는 index 를 구한다.
                maxDiff = diff;
                insertIndex = i;
            }
        }
    }
    if (insertIndex < 0) {
        return false;
    }
    else {
        insertThat(insertIndex, process);
        return true;
    }
}

bool fit(const Process &process) {
    if (TYPE == 0) return firstFit(process);
    else if (TYPE == 1) return bestFit(process);
    else if (TYPE == 2) return worstFit(process);
}

void run() {
    vector<Process> originInput(input);

    for (int type = 0; type < 3; type++) {
        /* 다음 할당 알고리즘 실행을 위한 준비 */
        TYPE = type;
        input = originInput;
        initMemory();
        waitingList.clear();

        /** === 메인 로직 ===
         * 1. 반복 1번에 단위 시간(currTime) 1이 흐른다. (0부터 시작한다.)
         * 2. pid == N-1 인 프로세스가 메모리에 적재되면, 시작 주소를 출력하고 종료한다.
         * 3. 현재 시간에 종료되는 프로세스가 있는지 확인한 후 메모리의 세그먼트 pid 에 -1 값으로 변경한다.
         * 4. 발생한 hole 들을 합쳐서, 다른 프로세스를 받을 준비를 한다.
         * 5. waitingList 에서 적재 가능한 프로세스들을 적재 시도한다.
         * 6. 적재 성공하면 적재 후 waitingList 에서 제거하고, 실패하면 그대로 둔다.
         * 7. 현재 시간에 적재할 수 있는 프로세스가 있는지 확인한 후 적재를 시도한다.
         * 8. 적재 성공하면 적재하고, 실패하면 waitingList 에 삽입한다.
         * 9. 1~8 과정을 2번 조건이 만족할 때 까지 계속 반복한다.
         */
        int currTime = 0;
        int idx = 0;
        while (1) {
            // 3
            for (Segment &segment : memory) {
                const int currPid = segment.pid; // 적재된 프로세스들의 id
                if (currPid < 0) continue;
                const Process &runningProcess = input[currPid];

                if (runningProcess.reqTime + runningProcess.needTime <= currTime) { // 종료시간 <= 현재시간
                    segment.pid = -1;
                }
            }

            // 4
            assembleHole();

            // 5
            bool isFinished = false;
            vector<int> deleteProcessIndex;
            for (int i = 0; i < waitingList.size(); i++) {
                const Process waitingProcess = waitingList[i];
                if (fit(waitingProcess)) { // 적재 성공
                    // 적재 성공한 시간에 요청이 들어왔다고 가정한다. 그러면 종료 시간을 예측하기 쉽다.
                    input[waitingProcess.pid].reqTime = currTime;
                    deleteProcessIndex.push_back(i);
                    if (waitingProcess.pid == N - 1) {
                        isFinished = true;
                        break;
                    }
                }
            }
            if (isFinished) break; // pid == N-1 을 메모리에 적재시킴

            // 6
            while (!deleteProcessIndex.empty()) {
                const int currIndex = deleteProcessIndex.back();
                deleteProcessIndex.pop_back();
                waitingList.erase(waitingList.begin() + currIndex);
            }

            if (idx >= N) {
                currTime++; // 1
                continue;
            }
            while (idx < N) {
                const Process currProcess = input[idx];
                // 7, 8
                if (currProcess.reqTime <= currTime) {
                    if (fit(currProcess)) { // 적재 성공
                        // 적재 성공한 시간에 요청이 들어왔다고 가정한다. 그러면 종료 시간을 예측하기 쉽다.
                        input[currProcess.pid].reqTime = currTime;
                        if (currProcess.pid == N - 1) {
                            isFinished = true;
                            break; // 2
                        }
                    }
                    else { // 적재 실패
                        waitingList.push_back(currProcess);
                    }
                    idx++; // 다음 프로세스 가져오기
                }
                else {
                    break; // 현재 시간의 모든 프로세스 적재 완료
                }
            }
            if (isFinished) break; // pid == N-1 을 메모리에 적재시킴

            currTime++; // 1
        }

        // pid == N-1 인 프로세스의 시작 주소를 출력한다.
        for (const Segment &segment : memory) {
            if (segment.pid == N - 1) {
                printf("%d\n", segment.address);
            }
        }
    }
}

int main() {

//    freopen("../markingData/sampleData8/1.inp", "r", stdin);
    freopen("allocation.inp", "r", stdin);
    freopen("allocation.out", "w", stdout);

    inputData();
    initMemory();
    run();

    return 0;
}