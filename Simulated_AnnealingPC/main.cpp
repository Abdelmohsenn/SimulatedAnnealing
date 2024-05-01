//
//  main.cpp
//  Simulated_AnnealingPC
//
//  Created by muhammad abdelmohsen on 30/04/2024.
//

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
using namespace std;
class cost {
public:
    int initialcost=1;
    int finalcost;
    
};

class Temperature: cost {
public:
    float initialTemp;
    float FinalTemp;
    
    Temperature(){
        initialTemp = 500 * initialcost;
    }
    
};
struct cells{
    
    int identifier;
    int ymargin;
    int xmargin;
    
};

struct Counts{
    int NumberOfCells;
    int NumberOfComponents;
    int row;
    int column;
};


struct netlists{
    
    int countofcomp=-1;
    vector <cells> cell;
    
    
};

void Grid(Counts& Read){
    
    for (int i = 0; i<Read.row; i++) {
        for (int j = 0; j<Read.column; j++) {
                        cout<< " -- ";
        }
        cout<<endl;
    }
}
vector <vector<netlists>> nets;

void Parsing_and_Assigning(string file, Counts& Read) {
    int numCells = 0, NumComp = 0, numRows = 0, numColumns = 0 , i=1, netword = -1;;
    Temperature x;
    string line,netline;
    ifstream readfile;
    readfile.open(file);
    
    if (!readfile.is_open()) { //egde case
        cout << "Failed to open the file\n";
        return;
    } // parsing first line
    if (getline(readfile, line)) {
        istringstream ss(line);
        
        if ((ss >> numCells >> NumComp >> numRows >> numColumns) && ss.eof()) {
            cout << numCells << " " << NumComp << " " << numRows << " " << numColumns << endl;
            
            Read.row = numRows;
            Read.column = numColumns;
            Read.NumberOfCells = numCells;
            Read.NumberOfComponents = NumComp;
            
        } else {
            cerr << "Failed Line: Incorrect number of components or invalid data format\n";
            return;
        }
    } else {
        cerr << "Failed to read line from the file\n";
        return;
    }
    
    // parsing the rest of the file
    
    vector <int> nets;
    
    while (getline(readfile, netline)) {
        istringstream Netparser(netline);
        while (Netparser >> netword) {
            nets.push_back(netword);
            cout << netword << " ";
        }
        nets.push_back(-1); // flag for a new net.
        cout << endl;

    }
    cout<<endl;
    cout<<endl;
    for (int i = 0; i <nets.size(); i++) {
        cout<<nets[i]<< " ";
        
    }
    Grid(Read);

}



int main(){
    Temperature obj;
    Counts obj1;
    string FileName = "/Users/muhammadabdelmohsen/Desktop/Spring 24/DD2/Project/d0.txt";
    
    Parsing_and_Assigning(FileName,obj1);

    
}

/*
 //    double n = x.initialTemp;
 //    for (int i =0 ; i<100; i++) {
 //        n*=0.95;
 //        cout<<n<<" ";
 //        if(n<0.1){
 //            cout<<"Stop!";
 //        }
 //    }
 */
