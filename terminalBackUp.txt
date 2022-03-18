#include <iostream>
#include <map>
#include<unordered_map>
#include <algorithm>
#include <string>
#include <vector>
#include <cassert>
#include <set>
#include <unordered_set>
#include <stack>
#include<fstream>

#define TESTING         true
#define SUBMIT          false
#define NOTUSEDYET      '*'
#define GENSOME         '|'
#define SHOWDEBUG       true
#define HIDEDEBUG       false
#define DISCARDDONTCARE true
#define CONTAINDONTCARE false

using namespace std;

vector<string> res;
map<int, vector<pair<string,char>>>::iterator iter;
map<int, vector<pair<string,char>>> sortedBarOneBuffer;
map<int, vector<pair<string, char>>> nextSortedBarOneBuffer;
vector<string> primaryImplicant;
set<string> enrollBook;
set<string> dontCareCollector;
map<string, unordered_set<string>> columnKeyPrimValue;/*such column can be covered by what prim*/
map<string, pair<char,unordered_set<string>>> primAndItsGen;/*first=primStr, second=prim's gnerated result*/

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
stack<string> generateOneNoBar(string str, bool withoutDontCare,bool debugSignal) {
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
			if (withoutDontCare) {
				if (dontCareCollector.find(tp) == dontCareCollector.end())
					repository.push(tp);
			}
			else{
				if (debugSignal)cout << tp << endl;
				repository.push(tp);
			}
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
void initInput(string INPUTFILE ,bool debugSignal) {//some thing went wrong here
	ifstream ifs(INPUTFILE.c_str(),ifstream::in);
	vector<string> vec;
	if(ifs.is_open()){
		ifs>>numInputVaraibles>>numInputStrings;
		cout<<numInputVaraibles <<" "<<numInputStrings<<endl;
		for (int i = 0; i < numInputStrings; i++) {
			string str = "";
			char c;
			bool hasDash = false;
			for (int j = 0; j < numInputVaraibles; j++) {
				ifs >> c;
				if (c == '-')
					hasDash = true;
				str.push_back(c);
			}
			auto onlyOneZero = generateOneNoBar(str,CONTAINDONTCARE,debugSignal);
			while (!onlyOneZero.empty()){
				string noBarStr = onlyOneZero.top();
				onlyOneZero.pop();
				if (hasDash)
					dontCareCollector.insert(noBarStr);
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

		ifs.close();
	}else{
		cout<<"FAIL TO OPEN THE FILE"<<endl;
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
void columnCoverInit(bool debugSignal) {
	for (auto primIter = primaryImplicant.begin(); primIter != primaryImplicant.end(); ++primIter) {
		string primStr = *primIter;
		auto primExpansion = generateOneNoBar(primStr, CONTAINDONTCARE, debugSignal);
		unordered_set<string> hashTableToSearchEleAboutPrim;
		while (!primExpansion.empty()){
			auto column = primExpansion.top();
			primExpansion.pop();
			hashTableToSearchEleAboutPrim.insert(column);
			if (columnKeyPrimValue.find(column) != columnKeyPrimValue.end()) {
				columnKeyPrimValue[column].insert(primStr);
			}
			else {
				unordered_set<string> primBuffer;
				primBuffer.insert(primStr);
				columnKeyPrimValue.insert(make_pair(column, primBuffer));
			}
		}
		auto pr = make_pair(NOTUSEDYET, hashTableToSearchEleAboutPrim);
		primAndItsGen.insert(make_pair(primStr, pr));
	}
	if (debugSignal) {

		for (auto x = columnKeyPrimValue.begin(); x != columnKeyPrimValue.end(); ++x) {
			cout << "for PRIM " << x->first << endl;
			for (auto y = x->second.begin(); y != x->second.end(); ++y) {
				cout << *y << endl;
			}
		}
		for (auto x = primAndItsGen.begin(); x != primAndItsGen.end(); ++x) {
			cout << "each PRIM" << x->first << " CONDITION:";
			if (x->second.first == NOTUSEDYET)cout << " NOT USED YET" << endl;
			else cout << " HAS BEEN USED" << endl;
			for (auto y = x->second.second.begin(); y != x->second.second.end(); ++y)
				cout << *y << endl;
		}
	}
}

string getRarestColumn() {
	int curMin = INT16_MIN;
	string tmpStr;
	for (auto x = columnKeyPrimValue.begin(); x != columnKeyPrimValue.end(); ++x) {
		if (x->second.size() < curMin) {
			tmpStr = x->first;
			curMin = x->second.size();
		}
	}
	return tmpStr;
}

void naiveCross(bool debugSignal) {
	while (!columnKeyPrimValue.empty()){
		string rarestColumn = getRarestColumn();
		cout << "SELECT: " << rarestColumn << endl;
		//remove the column

		for (auto x = primAndItsGen.begin(); x != primAndItsGen.end(); ++x) {
			if (x->second.second.find(rarestColumn)!=x->second.second.end()) {
				assert(x->second.first == NOTUSEDYET);
				for (auto eachColumn = x->second.second.begin(); eachColumn!=x->second.second.end(); ++eachColumn){
					if(columnKeyPrimValue.find(*eachColumn)!=columnKeyPrimValue.end())
						columnKeyPrimValue.erase(*eachColumn);
				}
				x->second.first = GENSOME;
				res.push_back(x->first);
				break;
			}
		}
	}
	if (debugSignal) {
		cout << "SELECT ELEMENT: " << endl;
		for (auto x:res) {
			cout << x << endl;
		}
	}
}
void demoResult(string OUTPUTFILE) {
	int literalCount = 0;
	for (auto x : res) {
		for (auto c : x)
			if (c != '-')
				literalCount++;
	}
	cout << literalCount << endl;
	cout << res.size() << endl;
	for (auto x : res)cout << x << endl;

	ofstream ofs;
	ofs.open(OUTPUTFILE.c_str());
	if(ofs.is_open()){
		ofs<<literalCount<<endl;
		ofs<<res.size()<<endl;
		for (auto x : res)ofs << x << endl;
		ofs.close();
	}
}

int main(int argc, char* argv[]){
	string inputPath;
	string outputPath;
	bool mode=TESTING;
	if(mode){
		inputPath = "../verifier/";
		outputPath = "../verifier/";
	}else {
		inputPath = "./";
		outputPath = "./";
	}
	string INPUTFILENAME = argv[1];
	string OUTPUTFILENAME = argv[2];
	string INPUTFILE = inputPath + INPUTFILENAME;
	string OUTPUTFILE = outputPath + OUTPUTFILENAME;

	cout << INPUTFILE<<" and >_< "<<OUTPUTFILE<<endl;

	initInput(INPUTFILE,HIDEDEBUG/*debugSignal*/);
	int cntIter = 0;
	while (primaryImplicantGeneration(HIDEDEBUG/*debugSignal*/)) {
		if(SHOWDEBUG){
		cout << "#########################################################" << endl;
		cout << ">? "<< cntIter << " iteration ends above " << endl;
		}
		cntIter++;
	}
	columnCoverInit(HIDEDEBUG);
	naiveCross(HIDEDEBUG);
	demoResult(OUTPUTFILE);

	return 0;
}
