//
// Created by jinseonghwang on 21. 3. 25..
//

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;

struct Person {
    int id;
    string first, last;

    bool operator<(const Person& nxt) const {
        return this->id < nxt.id;
    }
};

int N, lenLongestFirst;
vector<Person> people;
map<string, int> mp;

int main() {

    freopen("./test.inp", "r", stdin);
    freopen("./test.out", "w", stdout);

    cin >> N;
    for (int i = 0; i < N; ++i) {
        int id;
        string first, last;
        cin >> id >> first >> last;
        people.push_back({id, first, last});

        if (lenLongestFirst < first.length()) {
            lenLongestFirst = first.length();
        }

        if (mp.find(last) == mp.end()) { // 존재하지 않는 last name
            mp[last] = 1;
        }
        else { // 이미 존재하는 last name
            mp[last]++;
        }
    }

    sort(people.begin(), people.end());

    for (vector<Person>::iterator iter = people.begin(); iter != people.end(); ++iter) {
        cout.fill('0');
        cout.width(5);
        cout << right << iter->id << ' ';

        cout.fill(' ');
        cout.width(lenLongestFirst);
        cout << left << iter->first << ' ';
        cout << iter->last << '\n';
    }

    bool isFirstDup = true;

    for (map<string, int>::iterator iter = mp.begin(); iter != mp.end(); ++iter) {
        if (iter->second > 1) {
            if (isFirstDup) {
                cout << '\n';
                isFirstDup = false;
            }
            cout << iter->first << ' ' << iter->second << '\n';
        }
    }

    return 0;
}