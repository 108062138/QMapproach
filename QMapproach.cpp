// QMapproach.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <algorithm>
#include <string>
#include <vector>
#include <cassert>
#include <set>
#include <stack>
#define NOTUSEDYET  '*'
#define GENSOME     '|'

using namespace std;
map<int, vector<pair<string,char>>>::iterator iter;

map<int, vector<pair<string,char>>> sortedBarOneBuffer;
map<int, vector<pair<string, char>>> nextSortedBarOneBuffer;
vector<string> primaryImplicant;
set<string> enrollBook;
set<string> dontCareCollector;

int numInputVaraibles;
int numInputStrings;
pair<bool,int> mergeValidation(string& a, string& b) {
	assert(a.size() == b.size());
	int cntDiff = 0;
	int diffPosition = 0;
	for (int i = 0; i < a.size(); i++) {
		if (a[i] != b[i]) {
			cntDiff++;
			diffPosition = i;
		}
	}
	if (cntDiff == 1)return make_pair(true, diffPosition);
	else return make_pair(false, -1);//-1 means those can't be merged. Such merging situation should not be executed
}
stack<string> generateOneNoBar(string str, bool debugSignal) {
	stack<string> st;
	st.push(str);
	stack<string> repository;
	if (debugSignal)cout << "str that is about to get processed: " << str << endl;
	while (!st.empty()){
		auto tp = st.top();
		st.pop();
		int meetBar = tp.size();
		for (int i = 0; i < tp.size(); i++) {
			if (tp[i] == '-') {
				meetBar = i;
				break;
			}
		}
		if (meetBar == tp.size()) {
			if (debugSignal)cout << tp << endl;
			repository.push(tp);
		}
		else
		{
			string a = tp;
			a[meetBar] = '0';
			string b = tp;
			b[meetBar] = '1';
			st.push(a);
			st.push(b);
		}
	}
	if (debugSignal)cout << "end of processing" << str << endl;
	return repository;
}
void displayPrimImplicant() {
	cout << "display PRIMIMPLICANT" << endl;
	for (auto i = primaryImplicant.begin(); i != primaryImplicant.end(); ++i) {
		cout << *i << endl;
	}
}
void displayDontCare() {
	cout << "display DONTCARE" << endl;
	for (auto i = dontCareCollector.begin(); i != dontCareCollector.end(); ++i) {
		cout << *i << endl;
	}
	cout << "end DONTCARE" << endl;
}
void initInput(bool debugSignal) {//some thing went wrong here
	for (int i = 0; i < numInputStrings; i++) {
		string str = "";
		char c;
		bool hasDash = false;
		for (int j = 0; j < numInputVaraibles; j++) {
			cin >> c;
			if (c == '-')
				hasDash = true;
			str.push_back(c);
		}
		auto onlyOneZero = generateOneNoBar(str,debugSignal);
		while (!onlyOneZero.empty()){
			string noBarStr = onlyOneZero.top();
			onlyOneZero.pop();
			if (hasDash)dontCareCollector.insert(noBarStr);
			if (enrollBook.find(noBarStr) != enrollBook.end()) {
				continue;
			}
			int cntBarOne = 0;
			for (int j = 0; j < numInputVaraibles; j++) {
				if (noBarStr[j] == '1')cntBarOne++;
				assert(noBarStr[j] != '-');
			}
			if (sortedBarOneBuffer.find(cntBarOne) != sortedBarOneBuffer.end()) {
				sortedBarOneBuffer[cntBarOne].push_back(make_pair(noBarStr, NOTUSEDYET));
			}
			else {
				vector<pair<string, char>> vec;
				vec.push_back(make_pair(noBarStr, NOTUSEDYET));
				sortedBarOneBuffer.insert(make_pair(cntBarOne, vec));
			}
			enrollBook.insert(noBarStr);
		}
	}
	if (debugSignal) {
		for (iter = sortedBarOneBuffer.begin(); iter != sortedBarOneBuffer.end(); ++iter) {
			cout << "#of barOne= " << iter->first << " and it has " << iter->second.size() << " item" << endl;
			for (auto x : iter->second) {
				cout << x.first << " condition: " << x.second << endl;
			}
		}
	}
}
bool primaryImplicantGeneration(bool debugSignal) {
	for (iter = sortedBarOneBuffer.begin(); iter != sortedBarOneBuffer.end(); ++iter) {
		auto nextIter = next(iter,1);
		if (nextIter == sortedBarOneBuffer.end())continue;//meet the end
		if (abs(iter->first - nextIter->first) == 1) {//the only required merging condition
			for (auto& a : iter->second) {
				for (auto& b : nextIter->second) {
					auto validRes = mergeValidation(a.first, b.first);
					if (validRes.first) {
						a.second = GENSOME;
						b.second = GENSOME;
						int newCntBarOne = iter->first + 1;
						string tmpStr = a.first;
						tmpStr[validRes.second] = '-';
						if (enrollBook.find(tmpStr) != enrollBook.end())continue;
						if (nextSortedBarOneBuffer.find(newCntBarOne) != nextSortedBarOneBuffer.end()) {
							nextSortedBarOneBuffer[newCntBarOne].push_back(make_pair(tmpStr, NOTUSEDYET));
						}
						else
						{
							vector<pair<string, char>> vec;
							vec.push_back(make_pair(tmpStr, NOTUSEDYET));
							nextSortedBarOneBuffer.insert(make_pair(newCntBarOne, vec));
						}
						enrollBook.insert(tmpStr);
					}
				}
				
			}
		}
	}
	for (iter = sortedBarOneBuffer.begin(); iter != sortedBarOneBuffer.end(); ++iter) {
		for (auto a : iter->second) {
			if (a.second == NOTUSEDYET) {
				cout << "get IMPLICANT " << a.first << " its status= " << a.second << endl;
				primaryImplicant.push_back(a.first);
			}
		}
	}
	if(debugSignal){
		cout << "for the sortedBarOneBuffer" << endl;
		for (iter = sortedBarOneBuffer.begin(); iter != sortedBarOneBuffer.end(); ++iter) {
			cout << "#of barOne= " << iter->first << " and it has " << iter->second.size() << " item" << endl;
			for (auto x : iter->second) {
				cout << x.first << " condition: " << x.second << endl;
			}
		}
		cout << "===========================================================================================" << endl;
		cout << "for the nextSortedBarOneBuffer" << endl;
		for (iter = nextSortedBarOneBuffer.begin(); iter != nextSortedBarOneBuffer.end(); ++iter) {
			cout << "#of barOne= " << iter->first << " and it has " << iter->second.size() << " item" << endl;
			for (auto x : iter->second) {
				cout << x.first << " condition: " << x.second << endl;
			}
		}
	}
	int nextSortedBarOneSize = nextSortedBarOneBuffer.size();
	sortedBarOneBuffer.clear();
	sortedBarOneBuffer = nextSortedBarOneBuffer;
	nextSortedBarOneBuffer.clear();
	if (nextSortedBarOneSize == 0)return false;
	else return true;
}
int main(){
	cin >> numInputVaraibles >> numInputStrings;
	initInput(false/*debugSignal*/);
	int cntIter = 0;
	while (primaryImplicantGeneration(false/*debugSignal*/)) {
		cout << "#########################################################" << endl;
		cout << ">? "<< cntIter << " iteration ends above " << endl;
		cntIter++;
	}

	displayPrimImplicant();
	displayDontCare();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
