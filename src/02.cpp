//
// Created by jinseonghwang on 21. 3. 26..
//

#include <cstdio>
#include <algorithm>

using namespace std;

int main() {

    freopen("../markingData/sampleData2/2.inp", "r", stdin);
    freopen("../markingData/sampleData2/my.out", "w", stdout);

    int N;
    scanf("%d", &N);

    long long ans = 0;
    int arv, use, currPos = 0;
    for (int i = 0; i < N; i++) {
        scanf("%d %d", &arv, &use);
        if (arv < currPos) {
            ans += (currPos - arv);
        }
        currPos = max(currPos, arv) + use;
    }

    printf("%lld\n", ans);

    return 0;
}
/*
3
3 5
9 15
14 15
 */