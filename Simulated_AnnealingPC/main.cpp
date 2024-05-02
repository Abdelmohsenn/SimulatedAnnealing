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
#include <unordered_set>
#include <iomanip>
//#include "CImg.h"


//using namespace cimg_library;
using namespace std;
using namespace std::chrono;


//Classes and Structs
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

//Funcs
void Parsing_and_Assigning(string , Netlist&);
void InitialGrid(Netlist&);
vector<int> unique_cells(const vector<vector<cells>>&);
void RandomInitialPlacement(vector<int >,Netlist );
void SimulatedAnnealing();

//Global Vars
vector <vector<cells>> nets;
vector<vector<int>> grid;
vector<cells> cell_cord;

void Parsing_and_Assigning(string file, Netlist& Read) {
    
    int numCells = 0, NumComp = 0, numRows = 0, numColumns = 0, netword;
    vector <cells> net;
    cells Cell;
    string line,netline;
    ifstream readfile;
    readfile.open(file);
    
    if (!readfile.is_open()) { //egde case
        cerr << "Failed to open the file\n";
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
            
        } else { //egde case
            cerr << "Failed Line: Incorrect number of components or invalid data format in NetlistInfo Line\n";
            return;
        }
    } else { //egde case
        cerr << "Failed to read line from the file\n";
        return;
    }
    
    // parsing the rest of the file
    int lineNumber = 2; // starting point
    while (getline(readfile, netline)) {
        
        istringstream Netparser(netline);
        if (!(Netparser >> NumComp)) {
            cerr << "Error parsing line " << lineNumber << ": Missing number of components\n";
            return; // exit if the file format is not correct
        }
        
        while (Netparser >> netword) {
            Cell.identifier = netword;
            net.push_back(Cell);
        }
        
        // Check if the number of components matches the expected number
        
        if (net.size() != NumComp) {
            cerr << "Error parsing line " << lineNumber << ": Number of components does not match\n";
            return; // exit if the file format is not correct
        }
        //        cout<<"Net size here = > "<<net.size()<<"\n";
        nets.push_back(net);
        net.clear(); //resetting
        lineNumber++;
    }
    
    readfile.close();
    
    cout<<endl;
    cout<<endl;
    cout<<"Nets of nets: \n";
    for (auto net : nets) {
        for (auto cell : net) {
            cout << cell.identifier << " ";
        }
        cout << endl;
    }
    vector<int> unique=unique_cells(nets);// unique cells
    for(int i=0;i<unique.size();i++){
        cout<<unique[i]<<" ";
    }
    cout<<endl;
    //    cout<< " da eh ? => " << nets[2][2].identifier; // testing what value this is by index
    
    InitialGrid(Read);
    RandomInitialPlacement(unique,Read);
    cout << "---------------------------------------------------------------\n";
    cout<<"Grid after Random Placement: \n";
    InitialGrid(Read);
    
}
void print_cell_cord(vector<cells> cell_vec){
    cout<<endl<<"Cells Coordinates";
    for(int i=0;i<cell_vec.size();i++){
        cout<<cell_vec[i].identifier<<" ";
        cout<<cell_vec[i].xmargin<<" "<<cell_vec[i].ymargin<<endl;
    }
}

void adjust_cell_cord(vector<cells> &cell_vec,int index,cells cell_app){
    cell_vec[index].identifier=cell_app.identifier;
    cell_vec[index].xmargin =cell_app.xmargin;
    cell_vec[index].ymargin=cell_app.ymargin;
}

int get_identfier_index(vector<cells> vec,int cell_numb){
    for(int i=0;i<vec.size();i++){
        if(vec[i].identifier==cell_numb)
            return i;
    }
    return 0;
}

vector<int> unique_cells(const vector<vector<cells>>& nets) {
    unordered_set<int> unique_cell_vec;
    for (const auto& net : nets) {
        for (const auto& cell : net) {
            unique_cell_vec.insert(cell.identifier);
        }
    }
    vector<int> unique_cell_vecVector(unique_cell_vec.begin(), unique_cell_vec.end());
    return unique_cell_vecVector;
}
vector<int> cost; // Changed it to a global variable to calculate the total wire length
vector<int> get_cost(vector<vector<cells>> nets_vec){
    
    
    for(int i=0;i<nets_vec.size();i++){
        
        int x_min= nets_vec[i][0].xmargin;
        int y_min=nets_vec[i][0].ymargin;
        int x_max=nets_vec[i][0].xmargin;
        int y_max=nets_vec[i][0].ymargin;
        
        for(int j=0;j<nets_vec[i].size();j++){
            if(nets_vec[i][j].xmargin<x_min){
                x_min=nets_vec[i][j].xmargin;
            }
            else if(nets_vec[i][j].xmargin>x_max){
                x_max=nets_vec[i][j].xmargin;
            }
            if(nets_vec[i][j].ymargin<y_min){
                y_min=nets_vec[i][j].ymargin;
            }
            else if(nets_vec[i][j].ymargin>y_max){
                y_max=nets_vec[i][j].ymargin;
            }
        }
        cost.push_back((x_max-x_min)+(y_max-y_min));
        
    }
    return cost;
}
void equate_net_cord(vector<vector<cells>>& nets_vec,vector<cells> coord){
    for(vector<cells>& vec:nets_vec){
        for(cells& cell:vec){
            int index=get_identfier_index(coord,cell.identifier);
            cell.xmargin= coord[index].xmargin;
            cell.ymargin=coord[index].ymargin;
            
        }
    }
}
void RandomInitialPlacement(vector<int >unique,Netlist Read){
    int counter=0;
    
    
    for (int i = 0; i<Read.row; i++) {
        
        for (int j = 0; j<Read.column; j++) {
            cells cell_temp={0};
            int random=rand()%2;
            if(random){
                grid[i][j] =unique[counter];
                cell_temp.identifier=unique[counter];
                cell_temp.xmargin=i;
                cell_temp.ymargin=j;
                cell_cord.push_back(cell_temp);
                counter++;
            }
            else if((Read.column*Read.row)- (i*Read.column+j)<=unique.size()-counter ){/*fill grid when the left spaces are less or equal
                                                                                        to remaing cells to be inserted */
                grid[i][j] =unique[counter];
                cell_temp.identifier=unique[counter];
                cell_temp.xmargin=i;
                cell_temp.ymargin=j;
                cell_cord.push_back(cell_temp);
                counter++;
            }
            
        }
        
    }
    
    // print_cell_cord(cell_cord);
    equate_net_cord(nets,cell_cord);
    /*   for(int i=0;i<nets.size();i++){
     for(int j=0;j<nets[i].size();j++){
     cout<<endl<<nets[i][j].identifier<<" "<<nets[i][j].xmargin<<" "<<nets[i][j].ymargin<<endl;
     }
     }*/
    vector<int> intial_cost=get_cost(nets);
    cout<<endl<<"Initial Cost"<<endl;
    for(int i=0;i<intial_cost.size();i++){
        cout<<intial_cost[i]<<endl;
    }
    
    
}

int ComputeTotalWireLength(){
    int TotalWireLength=0;
    
    for (int i = 0; i<cost.size(); i++) {
        TotalWireLength+=cost[i];
    }
    return TotalWireLength;
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
            if (grid[i][j] == -1) {
                cout << setw(4)<<"-"<< " "; // Print the empty cells
                
            } else
                cout << setw(4)<<grid[i][j] << " "; // Print the value of each cell
        }
        cout<<endl;
    }
    cout << "---------------------------------------------------------------";
}


int main(){
    
    Temperature obj;
    Netlist obj1;
    string FileName = "/Users/muhammadabdelmohsen/Desktop/Spring 24/DD2/Project/d0.txt";
    Parsing_and_Assigning(FileName,obj1);
    
    cout<<"\nTotal Wire Length is " << ComputeTotalWireLength()<<endl;// outputting the wire length
    
    
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
