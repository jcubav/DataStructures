#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <map>

using namespace std;
void analyzeData(char *, char *, int);

int main(int argc, char **argv) {
    char infile[100] = "input.csv";
    char outfile[100] = "output.csv";
    int reqSize = 5;
    if(argc == 4) {
        strcpy(infile, argv[1]);
        strcpy(outfile, argv[2]);
        reqSize = atoi(argv[3]);
    }
    analyzeData(infile, outfile, reqSize);
}

class Heap {
    pair<string, float> *H;                                                 // string holds value and float holds priority
    int size, maxSize;                                                      // current size and maxsize
    string type;                                                            // "max" for maxHeap and "min" for minHeap
    
    public:
    Heap(int s, string t) {
        size = 0;
        maxSize = s;
        type = (t == "max")?"max":"min";
        H = new pair<string, float>[maxSize];
    }
    ~Heap() {
        delete[] H;
    }
    
    int getSize() { return size; }                                          // Returns heap size
    float getTop() { return H[0].second; }                                  // Returns priority of element at top of heap
    void setTop(string item, float key) {                                   // Replaces top of heap element with given values 
        H[0].first = item;
        H[0].second = key;
        heapify(0);
    }
    
    void insert(string item, float key) {                                   // Inserts elements when heap size is less than maximum capacity
        if(size < maxSize) {
            H[size].first = item;
            H[size].second = key;
            size++;
            buildHeap();
        }
    }
    
    void heapify(int i) {                                                   // Corrects heap assuming it is correct except for root at position i
        int l, r, m = i;
        l = 2*i + 1;
        r = 2*i + 2;
        if(type == "max") {
            if(l < maxSize && H[l].second > H[m].second)
                m = l;
            if(r < maxSize && H[r].second > H[m].second)
                m = r;
        }
        else {
            if(l < maxSize && H[l].second < H[m].second)
                m = l;
            if(r < maxSize && H[r].second < H[m].second)
                m = r;
        }
        
        if(m != i) {
            pair<string, float> temp = H[m];
            H[m] = H[i];
            H[i] = temp;
            heapify(m);
        }
    }
    
    void buildHeap() {
        for(int i=size-1; i>=0; i--)
            heapify(i);
    }
    
    void printVal(string header, char *outfile) {
        fstream fout(outfile, fstream::out | fstream::app);
        fout << header;
        for(int i=0; i<maxSize; i++) {
            if(H[i].first != "") 
                fout << H[i].first << "," << H[i].second << "\n";
        }
        fout << "\n";
        fout.close();
    }
};

void analyzeData(char *infile, char *outfile, int reqSize) {
    map<string, pair<int,int> > states;                                     // To store aggregate populations of all states
    map<string, pair<int,int> >::iterator mIter;
    Heap topC(reqSize, "min"), botC(reqSize, "max"), topS(reqSize, "min");  // Heap datastructures for the required data sets
    fstream fin(infile, fstream::in);
    string city, state;
    int pop10, pop11, pop12;
    float growth;
    char c;
    
    getline(fin, city, '"');                                                // Removing first line of header data
    while(fin.good()) {
        getline(fin, city, '"');
        fin >> c >> pop10 >> c >> pop11 >> c >> pop12 >> c;
        city = "\"" + city + "\"";
        if(city.find(';') != string::npos)                                  // To handle 2 exceptions
            state = "\"" + city.substr(city.find(';') + 2);
        else
            state = "\"" + city.substr(city.find(',') + 2);
    
        mIter = states.find(state);                                         // Maintaining map of all states, population
        if(mIter != states.end()) {
            mIter->second.first += pop10;
            mIter->second.second += pop12;
        }
        else {
            states[state].first = pop10;
            states[state].second = pop12;
        }
        
        if(pop12 > 50000) {                                                 // Creating heap of top & bottom cities with population above 50000
            growth = 100 * (float(pop12 - pop10)/pop10);
            if(topC.getSize() < reqSize) {
                topC.insert(city, growth);
                botC.insert(city, growth);
            }
            else { 
                if(topC.getTop() < growth)
                   topC.setTop(city, growth);
                if(botC.getTop() > growth)
                   botC.setTop(city, growth);
            }
        }
    }
    fin.close();

    for(mIter=states.begin(); mIter!=states.end(); ++mIter) {               // Workiing on state population aggregates
        state = mIter->first;
        pop10 = mIter->second.first;
        pop12 = mIter->second.second;
        growth = 100 * (float(pop12 - pop10)/pop10);
        if(topS.getSize() < reqSize)                                        // Creating heap of top states
            topS.insert(state, growth);
        else if(topS.getTop() < growth)
               topS.setTop(state, growth);
    }

    topC.printVal("Top cities to target,Change %\n", outfile);            // Writing results to output file
    botC.printVal("Top cities to avoid,Change %\n", outfile);
    topS.printVal("Top states to target,Change %\n", outfile);
}