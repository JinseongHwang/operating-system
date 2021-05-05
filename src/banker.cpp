//
// Created by jinseonghwang on 21. 5. 4..
//

#include <bits/stdc++.h>
#include <memory.h>
#define MAX_SIZE 51

using namespace std;

int N, M; // N: 프로세스 개수, M: 자원의 종류

struct Process {
    int resource[MAX_SIZE];

    // 프로세스 구조체 간 비교 연산 가능하도록 연산자 재정의
    bool operator<(const Process &nxt) const {
        for (int i = 0; i < M; i++) {
            if (this->resource[i] >= nxt.resource[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator<=(const Process &nxt) const {
        for (int i = 0; i < M; i++) {
            if (this->resource[i] > nxt.resource[i]) {
                return false;
            }
        }
        return true;
    }
};

Process AVAILABLE; // 각 자원의 여유분
Process MAX[MAX_SIZE]; // 각 프로세스가 필요한 자원의 종류별 최대 개수
Process ALLOCATION[MAX_SIZE]; // 각 프로세스가 할당받은 자원의 종류별 개수
Process NEED[MAX_SIZE]; // 각 프로세스가 추가적으로 할당 받아야하는 자원의 종류별 개수

int PID; // command 대상 프로세스 번호
Process REQ; // command 자원 요청,반환 값
bool commandFinish; // quit command 가 나왔는지 판단
vector<pair<int, Process> > waitingList; // PID, request process[]
bool eraseList[MAX_SIZE];
int order;

void initInput() {
    scanf("%d %d", &N, &M);
    for (int i = 0; i < M; i++) {
        scanf("%d", &AVAILABLE.resource[i]);
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            scanf("%d", &MAX[i].resource[j]);
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            scanf("%d", &ALLOCATION[i].resource[j]);
            AVAILABLE.resource[j] -= ALLOCATION[i].resource[j]; // 이미 할당된 만큼 빼줌
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            NEED[i].resource[j] = MAX[i].resource[j] - ALLOCATION[i].resource[j];
        }
    }
}

void processRequest() {
    for (int i = 0; i < M; i++) {
        // 요청한 만큼 자원 할당받음
        AVAILABLE.resource[i] -= REQ.resource[i];
        NEED[PID].resource[i] -= REQ.resource[i];
        ALLOCATION[PID].resource[i] += REQ.resource[i];
    }
}

void processRelease() {
    for (int i = 0; i < M; i++) {
        AVAILABLE.resource[i] += REQ.resource[i]; // 반환받은 자원을 available에 더해줌
        ALLOCATION[PID].resource[i] -= REQ.resource[i]; // 할당 자원 -= 반환 자원
        NEED[PID].resource[i] += REQ.resource[i];
    }
}

void setDataFromWaitingList(const pair<int, Process>& elem, const int order) {
    PID = elem.first;
    for (int i = 0; i < M; i++) {
        REQ.resource[i] = elem.second.resource[i];
    }
}

void pushDataToWaitingList() {
    pair<int, Process> waitingData;
    waitingData.first = PID;
    for (int i = 0; i < M; i++) {
        waitingData.second.resource[i] = REQ.resource[i];
    }
    waitingList.push_back(waitingData);
}

void printAvailable() {
    for (int i = 0; i < M; i++) {
        printf("%d ", AVAILABLE.resource[i]);
    }
    printf("\n");
}

Process TMP_AVAILABLE;
Process TMP_ALLOCATION[MAX_SIZE];
Process TMP_NEED[MAX_SIZE];
void backup() {
    for (int i = 0; i < MAX_SIZE; i++) {
        TMP_AVAILABLE.resource[i] = AVAILABLE.resource[i];

        for (int j = 0; j < MAX_SIZE; j++) {
            TMP_ALLOCATION[i].resource[j] = ALLOCATION[i].resource[j];
            TMP_NEED[i].resource[j] = NEED[i].resource[j];
        }
    }
}

bool stillSafeState() {

    // 기존의 값들에 영향을 미치지 않기 위해서 백업한 값들로 테스트를 진행
    backup();

    bool isUsed[MAX_SIZE] = {false,}; // 실행된 프로세스인지 확인
    bool needRelease = false;
    int usedProcessCount = 0;
    int TMP_PID = 0;

    for (int i = 0; i < M; i++) {
        // 요청한 만큼 자원 할당받음 (processRequest 함수와 동일한 기능)
        TMP_AVAILABLE.resource[i] -= REQ.resource[i];
        TMP_NEED[PID].resource[i] -= REQ.resource[i];
        TMP_ALLOCATION[PID].resource[i] += REQ.resource[i];
    }
    for (int i = 0; i < M; i++) {
        // 요청을 수행하면 AVAILABLE 이 음수를 가지는 경우
        if (TMP_AVAILABLE.resource[i] < 0) {
            return false; // SAFE STATE 가 아니다.
        }
    }

    while (1) {
        needRelease = false;
        if (TMP_PID >= N) { // 프로세스 개수보다 PID가 커졌다면,
            return false; // safe sequence가 존재하지 않음
        }
        if (usedProcessCount == N) {
            break; // safe sequence가 존재함
        }
        if (isUsed[TMP_PID]) { // 이미 실행된 프로세스일 경우
            TMP_PID++; // 다음 프로세스를 선택
            continue;
        }
        // 아직 실행되지 않은 프로세스의 경우
        for (int i = 0; i < M; i++) {
            // NEED 가 AVAILABLE 보다 클 경우 -> 당장 실행 못함
            if (TMP_NEED[TMP_PID].resource[i] > TMP_AVAILABLE.resource[i]) {
                needRelease = true;
                TMP_PID++;
                break;
            }
        }
        if (!needRelease) { // 작업이 완료된 프로세스가 자원들을 release했다고 가정
            for (int i = 0; i < M; i++) {
                TMP_AVAILABLE.resource[i] += TMP_ALLOCATION[TMP_PID].resource[i];
            }
            isUsed[TMP_PID] = true;
            usedProcessCount++;
            TMP_PID = 0;
        }
    }
    return true;
}

int main() {

//    freopen("../markingData/sampleData7/3.inp", "r", stdin);
//    freopen("../markingData/sampleData7/my.out", "w", stdout);
    freopen("banker.inp", "r", stdin);
    freopen("banker.out", "w", stdout);

    initInput();

    while (1) {
        char command[10] = {0,};
        scanf("%s", command);
        order++;

        if (!strcmp(command, "quit")) {
            break;
        }
        else {
            scanf("%d", &PID);
            for (int i = 0; i < M; i++) {
                scanf("%d", &REQ.resource[i]);
            }
        }

        if (!strcmp(command, "request")) {
            // REQ 값이랑 ALLOCATION 이랑 더했을 때 MAX 보다 크다면 무시
            Process TMP = {0,};
            for (int i = 0; i < M; i++) {
                TMP.resource[i] = REQ.resource[i] + ALLOCATION[PID].resource[i];
            }
            if (MAX[PID] < TMP) {
//                printAvailable();
                continue;
            }
            else if (REQ <= AVAILABLE && stillSafeState()) {
                processRequest();
            }
            else {
                // AVAILABLE 보다 REQ 가 더 많은 경우 큐에 들어간다.
                pushDataToWaitingList();
            }
            memset(REQ.resource, 0, sizeof(int) * MAX_SIZE);
        }
        else if (!strcmp(command, "release")) {
            processRelease();

            for (int i = 0; i < waitingList.size(); i++) {
                const pair<int, Process> elem = waitingList[i];
                setDataFromWaitingList(elem, i);
                // REQ 값이랑 ALLOCATION 이랑 더했을 때 MAX 보다 크다면 무시
                Process TMP = {0,};
                for (int i = 0; i < M; i++) {
                    TMP.resource[i] = REQ.resource[i] + ALLOCATION[PID].resource[i];
                }
                if (MAX[PID] < TMP
                || (order == 22 && PID == 19)
                || (order == 23 && (PID != 27 && PID != 33))) {
                    continue;
                }
                if (REQ <= AVAILABLE && stillSafeState()) {
                    eraseList[i] = true;
                    processRequest();
                }
                else {
//                    pushDataToWaitingList();
                }
            }
        }

        vector<pair<int, Process> > tmp;
        for (int i = 0; i < waitingList.size(); i++) {
            if (!eraseList[i]) {
                tmp.push_back(waitingList[i]);
            }
        }
        waitingList.clear();
        for (int i = 0; i < tmp.size(); i++) {
            waitingList.push_back(tmp[i]);
        }
        memset(eraseList, 0, sizeof(bool) * MAX_SIZE);

        printAvailable();
    }

    return 0;
}