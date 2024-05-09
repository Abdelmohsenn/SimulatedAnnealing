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
#include <ctime>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono> // timing library
#include <unordered_set>
#include <iomanip>
#include<random>
#include <climits>

//#include "CImg.h"


//using namespace cimg_library;
using namespace std;
using namespace std::chrono;



//Classes and Structs
struct Cost {
    // int_max for faster
    int tot_len = INT_MAX;
    int x_min = INT_MAX;
    int x_max = INT_MAX;
    int y_min = INT_MAX;
    int y_max = INT_MAX;
    Cost():x_max(-1),x_min(-1),y_min(-1),y_max(-1),tot_len(0){};
};


class Temperature {
public:
    double initialTemp;
    double FinalTemp;
     Temperature(){
        initialTemp = 0;
        FinalTemp=0;
    }
    Temperature(int intialcost,int size){
        initialTemp = 500 * intialcost;
        FinalTemp=0.000005*intialcost/size;
    }
    
};
struct cells{
    
    int identifier;
    int ymargin = INT_MAX;
    int xmargin = INT_MAX;
    vector <int> connections;
    cells() : identifier(-1), ymargin(0), xmargin(0), connections() {};
    cells(int i,int x, int y){
        identifier=i;
        ymargin=y;
        xmargin=x;
    }
};

struct Netlist{
    int NumberOfCells;
    int NumberOfComponents;
    int row;
    int column;
};



//Funcs
void Parsing_and_Assigning(string , Netlist&);
void InitialGrid(Netlist&,int);
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
          //  cout << numCells << " " << NumComp << " " << numRows << " " << numColumns << endl;
            
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
    
   
    vector<int> unique=unique_cells(nets);// unique cells
   /* for(int i=0;i<unique.size();i++){
        cout<<unique[i]<<" ";
    }*/
    //cout<<endl;
    //    cout<< " da eh ? => " << nets[2][2].identifier; // testing what value this is by index
    
    InitialGrid(Read,0);
    RandomInitialPlacement(unique,Read);
   // cout << "---------------------------------------------------------------\n";
    cout<<endl<<"Grid after Random Placement: \n";
    InitialGrid(Read,1);
    
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
vector <Cost> cost_new; // vector

//vector<int>
int get_tot_length(){
    int tot=0;
    for(int i=0;i<cost_new.size();i++){
        tot += cost_new[i].tot_len;
    }
    return tot;
    
}
// new cost function with faster execution, using the global cord vector.
void get_cost_new() {
    cost_new.clear();
    cost_new.resize(nets.size());

    for (const auto& cell : cell_cord) {
        for (const int& conn_idx : cell.connections) {
            auto& cost = cost_new[conn_idx];
            if (cost.x_max == -1) {
                cost.x_max = cost.x_min = cell.xmargin;
                cost.y_max = cost.y_min = cell.ymargin;
            } else {
                
                if (cell.xmargin < cost.x_min) cost.x_min = cell.xmargin;
                if (cell.xmargin > cost.x_max) cost.x_max = cell.xmargin;

                if (cell.ymargin < cost.y_min) cost.y_min = cell.ymargin;
                if (cell.ymargin > cost.y_max) cost.y_max = cell.ymargin;
            }
        }
    }

    for (auto& cost : cost_new) {
            cost.tot_len = (cost.x_max - cost.x_min) + (cost.y_max - cost.y_min);
        
    }
}

int c=0;
void get_cost(vector<cells> cell_cordn){
    cost_new.clear();
    cost_new.resize(nets.size());
    for(int i=0;i<cell_cordn.size();i++){
        for(int j=0;j<cell_cordn[i].connections.size();j++){
            int cell_indx=cell_cordn[i].connections[j];
            if(cost_new[cell_indx].x_max==-1){// empty set by default
                cost_new[cell_indx].x_max=cell_cordn[i].xmargin;
                cost_new[cell_indx].x_min=cell_cordn[i].xmargin;
                cost_new[cell_indx].y_max=cell_cordn[i].ymargin;
                cost_new[cell_indx].y_min=cell_cordn[i].ymargin;
            }
            else{
                    if(cell_cordn[i].xmargin< cost_new[cell_indx].x_min){
                    cost_new[cell_indx].x_min=cell_cordn[i].xmargin;
                }
                else if(cell_cordn[i].xmargin> cost_new[cell_indx].x_max){
                    cost_new[cell_indx].x_max=cell_cordn[i].xmargin;
                }
                   if(cell_cordn[i].ymargin< cost_new[cell_indx].y_min){
                    cost_new[cell_indx].y_min=cell_cordn[i].ymargin;
                }
                else if(cell_cordn[i].ymargin> cost_new[cell_indx].y_max){
                    cost_new[cell_indx].y_max=cell_cordn[i].ymargin;
                }
            }
        }
    }
    for(int i=0;i<cost_new.size();i++){
        cost_new[i].tot_len=(cost_new[i].x_max-cost_new[i].x_min)+(cost_new[i].y_max-cost_new[i].y_min);
    }
}

void equate_net_cord(vector<vector<cells>>& nets_vec, vector<cells>& coord) {
    for (size_t i = 0; i < nets_vec.size(); ++i) {
        
        for (size_t j = 0; j < nets_vec[i].size(); ++j) {
            int index = get_identfier_index(coord, nets_vec[i][j].identifier);
            nets_vec[i][j].xmargin = coord[index].xmargin;
            nets_vec[i][j].ymargin = coord[index].ymargin;
            coord[index].connections.push_back(i);
            
        }
    }
}

void RandomInitialPlacement(vector<int >unique,Netlist Read){
    int counter=0;
    int sze=unique.size();
     minstd_rand rng(time(0));
   // uniform_int_distribution<int> rndm(0, unique.size()-1);
     uniform_int_distribution<int> zno(0, 1);

    for (int i = 0; i<Read.row; i++) {
        
        for (int j = 0; j<Read.column; j++) {
            uniform_int_distribution<int> rndm(0, unique.size()-1);
            cells cell_temp;
            int random=zno(rng);
            if(random){
                int rd=rndm(rng);
                grid[i][j] =unique[rd];
                cell_temp.identifier=unique[rd];
                 unique.erase(unique.begin() + rd);
               // grid[i][j] =unique[counter];
              //  cell_temp.identifier=unique[counter];
                cell_temp.xmargin=i;
                cell_temp.ymargin=j;
                cell_cord.push_back(cell_temp);
                counter++;
            }
            else if((Read.column*Read.row)- (i*Read.column+j)<=sze-counter ){/*fill grid when the left spaces are less or equal
                                                                                        to remaing cells to be inserted */
                 int rd=rndm(rng);
                grid[i][j] =unique[rd];
                cell_temp.identifier=unique[rd];
                 unique.erase(unique.begin() + rd);
               // grid[i][j] =unique[counter];
              //  cell_temp.identifier=unique[counter];
                cell_temp.xmargin=i;
                cell_temp.ymargin=j;
                cell_cord.push_back(cell_temp);
                counter++;
            }
            
        }
        
    }
    equate_net_cord(nets,cell_cord);
//     get_cost(nets);
    get_cost_new();
    /*  cout<<endl<<"Nets of nets: \n";
      int ci=0;
    for (auto net : nets) {
        cout<<"NET "<<ci<<" ";
        ci++;
        for (auto cell : net) {
            cout << cell.identifier <<"("<<cell.xmargin<<","<<cell.ymargin<<")"<< " ";
        }
        cout << endl;
    }
     cout<<endl<<"Cells and their net connections: \n";
    for(int i=0;i<cell_cord.size();i++){
        cout<<cell_cord[i].identifier<<"("<<cell_cord[i].xmargin<<","<<cell_cord[i].ymargin<<") connections: ";
        for(int j=0;j<cell_cord[i].connections.size();j++){
            cout<<cell_cord[i].connections[j]<<" , ";
        }
        cout<<endl;
    }*/
 
    
    
}
int get_rand_int(int min,int max){
   random_device rd;
   mt19937 gen(rd());
   uniform_int_distribution<int> dist(min,max);
   return dist(gen);
    
}
double get_rand_double(double min,double max){
   random_device rd;
   mt19937 gen(rd());
   uniform_real_distribution<double> dist(min,max);
   return dist(gen);
    
}
int counter =0;

void swap_cells(cells c1,cells c2){
    
    if(c1.identifier>=0&&c1.identifier<cell_cord.size()&&c2.identifier>=0&&c2.identifier<cell_cord.size()&&c1.identifier!=c2.identifier){
        int i1=get_identfier_index(cell_cord,c1.identifier);
        int i2=get_identfier_index(cell_cord,c2.identifier);
        cell_cord[i1].xmargin=c2.xmargin;
        cell_cord[i1].ymargin=c2.ymargin;
        cell_cord[i2].xmargin=c1.xmargin;
        cell_cord[i2].ymargin=c1.ymargin;
        
        get_cost_new();
        if (counter==0){
            cout << i1 << " cooresponds => "<< cell_cord[i1].identifier<<endl;
            cout << i2 << " cooresponds => "<< cell_cord[i2].identifier<<endl;
            counter++;
        }
        grid[c1.xmargin][c1.ymargin]=c2.identifier;
        grid[c2.xmargin][c2.ymargin]=c1.identifier;
         
    }
    else if(c1.identifier>=0&&c1.identifier<cell_cord.size()&&c2.identifier==-1){
        
         int i1=get_identfier_index(cell_cord,c1.identifier);
        cell_cord[i1].xmargin=c2.xmargin;
        cell_cord[i1].ymargin=c2.ymargin;
        get_cost_new();
         grid[c1.xmargin][c1.ymargin]=c2.identifier;
         grid[c2.xmargin][c2.ymargin]=c1.identifier; 
    }
    else if(c2.identifier>=0&&c2.identifier<cell_cord.size()&&c1.identifier==-1){
           
        int i2=get_identfier_index(cell_cord,c2.identifier);
        cell_cord[i2].xmargin=c1.xmargin;
        cell_cord[i2].ymargin=c1.ymargin;
        get_cost_new();
        grid[c1.xmargin][c1.ymargin]=c2.identifier;
        grid[c2.xmargin][c2.ymargin]=c1.identifier; 
    }

}




void simulate_annealing(int intial_wire_lenght, Netlist NL){
    int cell_identfier_1,cell_identfier_2;
    int cell_x1,cell_x2,cell_y1,cell_y2;
    int HPWL_1,HPWL_2,HPWL_diff;
    int N_moves=cell_cord.size();
    float coolingrate = 0.95;
//    cout<<"cell numbers"<<cell_cord.size()<<"\n";

    Temperature T(intial_wire_lenght,nets.size());
    double curr_temp=T.initialTemp;
    double prob;
    double rand_doubl;
    minstd_rand rng(time(0));
    
            uniform_int_distribution<int> intRowsRange(0, NL.row-1);
            uniform_int_distribution<int> intColumnsRange(0, NL.column-1);
            uniform_real_distribution<double> doubleDist(0, 1);
    int counter=0;
    milliseconds t =milliseconds(0);
     while(curr_temp>T.FinalTemp){
      
        for(int i=0;i<N_moves; ++i){
            counter++;
           /*cell_x1=get_rand_int(0,NL.row-1);
            cell_y1=get_rand_int(0,NL.column-1);
            cell_x2=get_rand_int(0,NL.row-1);
            cell_y2=get_rand_int(0,NL.column-1);*/
           
            cell_x1=intRowsRange(rng);
            cell_y1=intColumnsRange(rng);
            cell_x2=intRowsRange(rng);
            cell_y2=intColumnsRange(rng);
         
            cell_identfier_1= grid[cell_x1][cell_y1];
            cell_identfier_2= grid[cell_x2][cell_y2];
            HPWL_1=get_tot_length();
            
            cells c1(cell_identfier_1,cell_x1,cell_y1);
            cells c2(cell_identfier_2,cell_x2,cell_y2);
            swap_cells(c1,c2);
            
            HPWL_2=get_tot_length();
            HPWL_diff=HPWL_2-HPWL_1;
          
            if(HPWL_diff>=0){
                prob=exp(-1*(double)HPWL_diff/curr_temp);
//                rand_doubl=get_rand_double(0,1);
              rand_doubl=doubleDist(rng);
            
                if(rand_doubl>prob){
                      cells c1(cell_identfier_1,cell_x2,cell_y2);
                      cells c2(cell_identfier_2,cell_x1,cell_y1);
//                    auto start = steady_clock::now();
                      swap_cells(c1,c2);
//                    auto end = steady_clock::now();
//                    auto delta_time= duration_cast<milliseconds>(end-start);
//                    t=t+delta_time;
//                    cout<<delta_time;
                    
                  
                }
            }

        }
        curr_temp = curr_temp * coolingrate;
     }
    
    cout<< " total number of iterations => "<<counter<<endl;
    cout<< " total time taken in swap => "<<t<<endl;

}

void InitialGrid(Netlist& Read,int indc){
    cout << "--------------------------------------------------------------------------------------------------------------------------------\n";
    grid.resize(Read.row);
    for(int i = 0; i < grid.size(); i++) {
        grid[i].resize(Read.column, -1);
    }
    if(!indc){
    for (int i = 0; i<Read.row; i++) {
        
        for (int j = 0; j<Read.column; j++) {
            if (grid[i][j] == -1) {
               // cout << setw(4)<<"-"<< " "; // Print the empty cells
                 cout << setw(4)<<"1"<< " "; 
                
            } else{
                //cout << setw(4)<<grid[i][j] << " "; // Print the value of each cell
                 cout << setw(4)<<"0"<< " "; 
            }
        }
        cout<<endl;
    }
  }
  else{
     for (int i = 0; i<Read.row; i++) {
        
        for (int j = 0; j<Read.column; j++) {
            if (grid[i][j] == -1) {
               // cout << setw(4)<<"-"<< " "; // Print the empty cells
                 cout << setw(4)<<"--"<< " "; 
                
            } else{
                cout << setw(4)<<grid[i][j] << " "; // Print the value of each cell
               //  cout << setw(4)<<"0"<< " "; 
            }
        }
        cout<<endl;
    }

  }
    cout << "--------------------------------------------------------------------------------------------------------------------------------\n";
}


int main(){
    
   // Temperature obj;
    Netlist obj1;
    string FileName = "/Users/muhammadabdelmohsen/Desktop/Spring 24/DD2/Project/d0.txt";
    Parsing_and_Assigning(FileName,obj1);
    
    int intial_wire_lenght=get_tot_length();
    cout<<"\nTotal Wire Length is " <<intial_wire_lenght<<endl;// outputting the wire length
    
    auto start = steady_clock::now();
   simulate_annealing(intial_wire_lenght,obj1);
   auto end=steady_clock::now();
    int final_wire_lenght=get_tot_length();
   InitialGrid(obj1,1);
   cout<<"\nTotal Wire Length is " << final_wire_lenght<<endl;
   auto delta_time= duration_cast<milliseconds>(end-start);
   cout<<"Time Taken: "<<delta_time.count()/1000.0<<" seconds"<<endl;
    
   cout<<endl<<"Cells and their net connections: \n";
    for(int i=0;i<cell_cord.size();i++){
        cout<<cell_cord[i].identifier<<"("<<cell_cord[i].xmargin<<","<<cell_cord[i].ymargin<<") connections: ";
        for(int j=0;j<cell_cord[i].connections.size();j++){
            cout<<cell_cord[i].connections[j]<<" , ";
        }
        cout<<endl;
    }
}

