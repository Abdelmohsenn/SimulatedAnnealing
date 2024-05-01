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
#include <map>
#include <chrono> // timing library

using namespace std;
using namespace std::chrono;


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

struct Netlist{
    int NumberOfCells;
    int NumberOfComponents;
    int row;
    int column;
};


//struct netlists{
//
//    int countofcomp=-1;
//    vector <cells> cell;
//
//
//};

void Parsing_and_Assigning(string , Netlist&);
void InitialGrid(Netlist&);
void SimulatedAnnealing();
vector <vector<cells>> nets;
vector<vector<int>> grid;

void Parsing_and_Assigning(string file, Netlist& Read) {
    
    int numCells = 0, NumComp = 0, numRows = 0, numColumns = 0 , index = 0, netword = -1;;
    vector <cells> net;
    cells Cell;
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
            
        } else { //egde cas
            clog << "Failed Line: Incorrect number of components or invalid data format\n";
            return;
        }
    } else { //egde cas
        clog << "Failed to read line from the file\n";
        return;
    }
    
    // parsing the rest of the file
    
    while (getline(readfile, netline)) {
        istringstream Netparser(netline);
        Netparser >> NumComp; // each number of component in each net
        while (Netparser >> netword) {
            Cell.identifier = netword;
            net.push_back(Cell);
            //            cout << netword << " ";
        }
        nets.push_back(net);
        net.clear();
        // flag for a new net.
        //        cout << endl;
        
    }
    cout<<endl;
    cout<<endl;
    cout<<"Net of nets: \n";
    for (auto net : nets) {
        for (auto cell : net) {
            cout << cell.identifier << " ";
        }
        cout << endl;
    }
    
    //    cout<< " da eh ? => " << nets[2][2].identifier; // testing what value this is by index
    InitialGrid(Read);
    
}
void SimulatedAnnealing(){
    
    
    
}

void InitialGrid(Netlist& Read){
    
    cout << "---------------------------------------------------------------\n";
    grid.resize(Read.row);
    for(int i = 0; i < grid.size(); i++) {
        grid[i].resize(Read.column, -1);
    }
    for (int i = 0; i<Read.row; i++) {
        for (int j = 0; j<Read.column; j++) {
            cout << grid[i][j] << " "; // Print the value of each cell
        }
        cout<<endl;
    
    }
    cout << "---------------------------------------------------------------\n";

}


int main(){
    Temperature obj;
    Netlist obj1;
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
