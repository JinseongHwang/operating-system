//
// Created by jinseonghwang on 21. 3. 30..
//
// 03

#include <cstdio>
#include <queue>
#include <algorithm>
#include <vector>

using namespace std;

typedef long long ll;

struct SJF {
    ll arrival, usage;

    bool operator<(const SJF &nxt) const {
        if (this->usage == nxt.usage) {
            return this->arrival > nxt.arrival;
        }
        return this->usage > nxt.usage;
    }
};

void printProcessStatus(const SJF &process) {
//    printf("%lld %lld\n", process.arrival, process.usage);
}

int main() {

//    freopen("./sjf.inp", "r", stdin);
//    freopen("./sjf.out", "w", stdout);

    ll N;
    scanf("%lld", &N);

    vector<SJF> input(N);
    for (int i = 0; i < N; i++) {
        SJF inputSJF;
        scanf("%lld %lld", &inputSJF.arrival, &inputSJF.usage);
        input[i] = inputSJF;
    }

    sort(input.begin(), input.end(), [](const SJF& a, const SJF& b) {
        if (a.arrival == b.arrival) {
            return a.usage < b.usage;
        }
        return a.arrival < b.arrival;
    });

    SJF firstProcess = input.front();

    ll currTime = firstProcess.arrival + firstProcess.usage;
    ll result = 0, sumUsages = 0;

    priority_queue<SJF, vector<SJF> > pq;
    priority_queue<SJF, vector<SJF> > repository;

    for (int i = 1; i < N; i++) {
        SJF newProcess = input[i];

        // sumUsages + currTime 보다 큰 값이 들어오면 시간점프이다.
        if (currTime + sumUsages < newProcess.arrival) {
            while (!repository.empty()) {
                SJF preMinProcess = repository.top();
                repository.pop();
                pq.push(preMinProcess);
            }
            while (!pq.empty()) {
                SJF currProcess = pq.top();
                pq.pop();
                printProcessStatus(currProcess);
                sumUsages -= currProcess.usage;

                result += currTime - currProcess.arrival;
                currTime = max(currTime, currProcess.arrival) + currProcess.usage;
            }
            currTime = newProcess.arrival + newProcess.usage;
            continue;
        }

        sumUsages += newProcess.usage;

        vector<SJF> postProcess;
        while (!repository.empty()) {
            SJF currProcess = repository.top();
            repository.pop();
            if (currProcess.arrival <= currTime) {
                pq.push(currProcess);
            }
            else {
                postProcess.push_back(currProcess);
            }
        }
        while (!postProcess.empty()) {
            repository.push(postProcess.back());
            postProcess.pop_back();
        }

        if (newProcess.arrival <= currTime) {
            pq.push(newProcess);
        }
        else {
            SJF currProcess = pq.top();
            pq.pop();
            printProcessStatus(currProcess);
            sumUsages -= currProcess.usage;

            result += currTime - currProcess.arrival;
            currTime = max(currTime, currProcess.arrival) + currProcess.usage;
            repository.push(newProcess);
        }
    }
    while (!repository.empty()) {
        SJF preMinProcess = repository.top();
        repository.pop();
        pq.push(preMinProcess);
    }
    while (!pq.empty()) {
        SJF currProcess = pq.top();
        pq.pop();
        printProcessStatus(currProcess);
        sumUsages -= currProcess.usage;

        result += currTime - currProcess.arrival;
        currTime = max(currTime, currProcess.arrival) + currProcess.usage;
    }

    printf("%lld\n", result);

    return 0;
}