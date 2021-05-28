//
// Created by jinseonghwang on 21. 5. 28..
//

#include <bits/stdc++.h>

#define MAX_PROCESS 100001

using namespace std;


struct Reference {
    int pid, page; // <PID, Page number>
};

struct Segment {
    int frame, pid, page; // <Frame number, PID, Page number>
    int loadTime; // 메모리에 적재된 시간을 기록

    bool operator==(const Reference &reference) const {
        if (this->pid == reference.pid && this->page == reference.page) return true;
        return false;
    }
};

// Optimal 알고리즘에서 임시로 사용할 데이터 타입
struct TempProcess {
    int page, position, loadTime;

    bool operator<(const TempProcess& nxt) const {
        // 가장 멀리 떨어진 페이지를 victim page 로 선정한다.
        // 하지만 다수의 페이지가 INT_MAX 만큼의 거리를 가진다면, 더 빨리 적재된 페이지를 victim page 로 선정한다.
        if (this->position == nxt.position) {
            return this->loadTime < nxt.loadTime;
        }
        return this->position > nxt.position;
    }
};

int N; // 프로세스 1개에 할당될 프레임 개수
int numberOfPageInProcess[MAX_PROCESS]; // numberOfPageInProcess[i] : i번째 프로세스에 할당된 페이지의 개수

vector<Reference> referenceString; // Reference string (참조열)
vector<Segment> mainMemory; // Main memory (RAM, 주기억장치)

void inputData() {
    scanf("%d", &N);
    Reference input = {0, 0};
    while (1) {
        scanf("%d %d", &input.pid, &input.page);
        if (input.pid == -1 && input.page == -1) break;
        referenceString.push_back(input);
    }
}

void initData() {
    mainMemory.clear();
    memset(numberOfPageInProcess, 0, sizeof(int) * MAX_PROCESS);
}

void printMainMemory() {
    for (int i = 0; i < mainMemory.size(); i++) {
        const Segment &segment = mainMemory[i];
        printf("%d %d %d\n", i, segment.pid, segment.page);
    }
}

Segment toSegment(const Reference &reference, const int &currTime) {
    Segment segment;
    segment.pid = reference.pid;
    segment.page = reference.page;
    segment.loadTime = currTime;
    segment.frame = 0; // 프레임은 출력 때 붙여준다.
    return segment;
}

bool pageFault(Reference reference) {
    for (const Segment &segment : mainMemory) {
        // 하나라도 동일한 페이지가 이미 적재되어 있다면 Page fault 는 발생하지 않는다.
        if (segment == reference) {
            return false;
        }
    }
    return true;
}

void fifo() {
    int pageFaultCount = 0;
    for (int i = 0; i < referenceString.size(); i++) {
        const Reference &curr = referenceString[i];
        if (pageFault(curr)) {
            /* Page replacement */
            if (numberOfPageInProcess[curr.pid] >= N) { // 이미 N개 적재된 상태
                // curr.pid 번째 프로세스에 가장 먼저 들어온 페이지를 찾는다.
                int index = 0, minTime = INT32_MAX;
                for (int j = 0; j < mainMemory.size(); j++) {
                    const Segment &segment = mainMemory[j];
                    if (segment.pid == curr.pid && minTime > segment.loadTime) {
                        minTime = segment.loadTime;
                        index = j;
                    }
                }
                mainMemory[index] = toSegment(curr, i); // 가장 먼저 들어온 페이지를 현재 들어온 페이지로 대체
            }
            else {
                mainMemory.push_back(toSegment(curr, i)); // 현재 reference 를 삽입한다.
                numberOfPageInProcess[curr.pid]++;
            }
            pageFaultCount++;
        }
    }
    printf("FIFO: %d\n", pageFaultCount);
    printMainMemory();
}

void lru() {
    int pageFaultCount = 0;

    for (int i = 0; i < referenceString.size(); i++) {
        const Reference &curr = referenceString[i];
        if (pageFault(curr)) {
            /* Page replacement */
            if (numberOfPageInProcess[curr.pid] >= N) { // 이미 N개 적재된 상태
                set<int> currProcessInMemory; // page

                // 현재 메모리에 적재된 curr.pid 프로세스의 페이지 목록을 가져온다.
                for (const Segment& segment : mainMemory) {
                    if (segment.pid == curr.pid) {
                        currProcessInMemory.insert(segment.page);
                    }
                }

                for (int pos = i - 1; pos >= 0; pos--) {
                    const Reference &currReference = referenceString[pos];
                    if (currReference.pid == curr.pid) {
                        currProcessInMemory.erase(currReference.page);
                    }
                    if (currProcessInMemory.size() == 1) break;
                }

                int victimPage = *(currProcessInMemory.begin());
                for (Segment &segment : mainMemory) {
                    if (segment.pid == curr.pid && segment.page == victimPage) {
                        segment = toSegment(curr, i); // victim page 를 새로운 Segment 로 대체
                        break;
                    }
                }
            }
            else {
                mainMemory.push_back(toSegment(curr, i)); // 현재 reference 를 삽입한다.
                numberOfPageInProcess[curr.pid]++;
            }
            pageFaultCount++;
        }
    }
    printf("LRU: %d\n", pageFaultCount);
    printMainMemory();
}

void opt() {
    int pageFaultCount = 0;
    for (int i = 0; i < referenceString.size(); i++) {
        const Reference &curr = referenceString[i];
        if (pageFault(curr)) {
            /* Page replacement */
            if (numberOfPageInProcess[curr.pid] >= N) { // 이미 N개 적재된 상태
                vector<TempProcess> currProcessInMemory; // page, position, loadTime

                // 현재 메모리에 적재된 curr.pid 프로세스의 페이지 목록을 가져온다. 초기 위치는 INT_MAX 로 설정
                for (const Segment& segment : mainMemory) {
                    if (segment.pid == curr.pid) {
                        currProcessInMemory.push_back({segment.page, INT32_MAX, segment.loadTime});
                    }
                }

                // i + 1 번 위치부터 끝까지 탐색하며 curr.pid && curr.page 를 만족하는걸 찾으면 위치 갱신
                for (TempProcess &currProcess : currProcessInMemory) {
                    for (int pos = i + 1; pos < referenceString.size(); pos++) {
                        const int &tmpPid = curr.pid;
                        const int &tmpPage = currProcess.page;
                        const Reference &currReference = referenceString[pos];

                        if (currReference.pid == tmpPid && currReference.page == tmpPage) {
                            currProcess.position = pos;
                            break;
                        }
                    }
                }

                // victim page 선정 기준에 따라 정렬(front 만 필요하지만, 요소 수가 적어서 편의상 정렬)
                sort(currProcessInMemory.begin(), currProcessInMemory.end());
                int victimPage = currProcessInMemory.front().page; // victim page 선정 완료

                for (Segment &segment : mainMemory) {
                    if (segment.pid == curr.pid && segment.page == victimPage) {
                        segment = toSegment(curr, i); // victim page 를 새로운 Segment 로 대체
                        break;
                    }
                }
            }
            else {
                mainMemory.push_back(toSegment(curr, i)); // 현재 reference 를 삽입한다.
                numberOfPageInProcess[curr.pid]++;
            }
            pageFaultCount++;
        }
    }
    printf("OPT: %d\n", pageFaultCount);
    printMainMemory();
}

void run() {
    fifo();
    initData();
    lru();
    initData();
    opt();
}

int main() {

//    freopen("../markingData/sampleData9/2.inp", "r", stdin);
//    freopen("../markingData/sampleData9/my.out", "w", stdout);
    freopen("page.inp", "r", stdin);
    freopen("page.out", "w", stdout);

    inputData();
    run();

    return 0;
}
