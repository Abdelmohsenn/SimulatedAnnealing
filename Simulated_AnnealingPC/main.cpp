#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include <unordered_set>
#include <iomanip>
#include<random>
#include <climits>
#include <cstdlib>

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

struct Net {
    int id;
    map<int, int> cells;  // Changed from vector<int> to map<int, int>

    Net(int id) : id(id) {}
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

int NumberOfCells; int NumberOfComponents; int row; int column;
//Funcs
void Parsing_and_Assigning(string );
void InitialGrid(int);
vector<int> unique_cells(const vector<vector<cells>>&);
void RandomInitialPlacement(vector<int>);
void SimulatedAnnealing();

//Global Vars
vector <vector<cells>> nets;
vector<vector<int>> grid;
vector<cells> cell_cord;

// two new global vectors <==
vector<Net> NETS;
vector <cells> CELLS;


void Parsing_and_Assigning(string file) {
    int netIndex = 0;
    int numCells = 0, NumComp = 0, numRows = 0, numColumns = 0, netword;
    vector<cells> net;
    cells Cell;
    string line, netline;
    ifstream readfile;
    readfile.open(file);
    if (!readfile.is_open()) {
        cerr << "Failed to open the file\n";
        return;
    }
    if (getline(readfile, line)) {
        istringstream ss(line);
        if ((ss >> NumberOfCells >> NumberOfComponents >> row >> column) && ss.eof()) {
        } else {
            cerr << "Failed Line: Incorrect number of components or invalid data format in NetlistInfo Line\n";
            return;
        }
    } else {
        cerr << "Failed to read line from the file\n";
        return;
    }

    int lineNumber = 2; // starting point
    CELLS.resize(NumberOfCells);  // Resize CELLS to the number of cells

    while (getline(readfile, netline)) {
        istringstream Netparser(netline);
        if (!(Netparser >> NumComp)) {
            cerr << "Error parsing line " << lineNumber << ": Missing number of components\n";
            return;
        }

        net.clear();
        while (Netparser >> netword) {
            Cell.identifier = netword;
            net.push_back(Cell);
            net.back().connections.push_back(netIndex); // assigning the index of the current net to the cell
            CELLS[netword].connections.push_back(netIndex); // assigning each cell with all nets indexes

            CELLS[netword].identifier=netword;
        }

        if (net.size() != NumComp) {
            cerr << "Error parsing line " << lineNumber << ": Number of components does not match\n";
            return;
        }

        // Update NETS with the current net
        NETS.push_back(Net(netIndex));
        for (const cells& cell : net) {
            NETS.back().cells[cell.identifier] = cell.identifier; // Map to map the net index to cell identifiers
        }

        netIndex++;
        nets.push_back(net);
        lineNumber++;
    }

    readfile.close();

    vector<int> unique = unique_cells(nets);
    InitialGrid(0);
    RandomInitialPlacement(unique);
    InitialGrid(0);
    cout << endl << "Grid after Random Placement: \n";
    InitialGrid(1);

    
    // print NETS with corresponding CELL ids using map, now the indexes are the sam as cell identifiers
//    cout << endl << "NETS with corresponding CELL ids:" << endl;
//    for (const Net& net : NETS) {
//        cout << "Net " << net.id << " -> Cells: ";
//        for (const auto& pair : net.cells) {
//            cout << pair.first << " ";
//        }
//        cout << endl;
//    }
//    
//    // Print CELLS with all corresponding nets using index
//    cout << endl << "CELLS with all corresponding nets:" << endl;
//    for (const cells& cell : CELLS) {
//        cout << "Cell " << cell.identifier << " -> Nets: ";
//        for (int netIndex : cell.connections) {
//            cout << netIndex << " ";
//        }
//        cout << endl;
//    }
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


int get_tot_length(){
    int tot=0;
    for(int i=0;i<cost_new.size();i++){
        tot += cost_new[i].tot_len;
    }
    return tot;
    
}

void RandomPlacementCost() {
    cost_new.clear();
    cost_new.resize(nets.size());
    
    for (const auto& cell : CELLS) {
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
// new update cot for only the affected nets
void Update_Cost(vector<cells>& CELLS, int index1, int index2) {
    unordered_set<int> affected_nets; // set for only the affected nets to iterate over

    if (index1 >= 0) {
        for (int net : CELLS[index1].connections) {
            affected_nets.insert(net);
        }
    }

    if (index2 >= 0) {
        for (int net : CELLS[index2].connections) {
            affected_nets.insert(net);
        }
    }

    // calculate costs only for affected nets
    for (int net_index : affected_nets) {
        auto& net = NETS[net_index];
        int x_min = INT_MAX, x_max = -1, y_min = INT_MAX, y_max = -1;

        // calculate each cell
        for (auto& cell_id : net.cells) {
            int idx = cell_id.second; // map accessing
            x_min = min(x_min, CELLS[idx].xmargin);
            x_max = max(x_max, CELLS[idx].xmargin);
            y_min = min(y_min, CELLS[idx].ymargin);
            y_max = max(y_max, CELLS[idx].ymargin);
        }

        Cost& cost = cost_new[net_index];
        cost.x_min = x_min;
        cost.x_max = x_max;
        cost.y_min = y_min;
        cost.y_max = y_max;
        cost.tot_len = (x_max - x_min) + (y_max - y_min);
    }
}
//void saveGridStateToFile(const vector<vector<int>>& grid, int step) {
//    ofstream file("/Users/muhammadabdelmohsen/Desktop/Spring 24/DD2/Project/Gifs/T1/grid_state_" + to_string(step) + ".csv");
//    for (const auto& row : grid) {
//        for (size_t i = 0; i < row.size(); ++i) {
//            file << row[i];
//            if (i < row.size() - 1) {
//                file << ",";
//            }
//        }
//        file << "\n";
//    }
//
//    file.close();
//}


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

void RandomInitialPlacement(vector<int> unique) {
    int counter = 0;
    int sze = unique.size();
    minstd_rand rng(static_cast<unsigned int>(time(0))); // Ensure proper seeding
    uniform_int_distribution<int> zno(0, 1);

    cout << "Starting Random Initial Placement" << endl;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            uniform_int_distribution<int> rndm(0, unique.size() - 1);
            int random = zno(rng);
            if (random) {
                if (!unique.empty()) {
                    int rd = rndm(rng);
                    grid[i][j] = unique[rd];
                    cells cell_temp(unique[rd], i, j); // Updated xmargin and ymargin with i, j
//                    cell_cord.push_back(cell_temp);
                    CELLS[unique[rd]].xmargin = i; // Correctly updating CELLS vector(new)
                    CELLS[unique[rd]].ymargin = j;
                    unique.erase(unique.begin() + rd);
//                    cout << "laced cell " << cell_temp.identifier << " at (" << i << ", " << j << ")" << endl;
                }
            } else if ((column * row) - (i * column + j) <= sze - counter) {
                if (!unique.empty()) {
                    int rd = rndm(rng);
                    grid[i][j] = unique[rd];
                    cells cell_temp(unique[rd], i, j);
//                    cell_cord.push_back(cell_temp);
                    CELLS[unique[rd]].xmargin = i;
                    CELLS[unique[rd]].ymargin = j;
                    unique.erase(unique.begin() + rd);
//                    cout << " placed cell " << cell_temp.identifier << " at (" << i << ", " << j << ")" << endl;
                }
            }
        }
    }

//    cout << " Verifying cell coordinates:" << endl;
//    for (const auto& cell : CELLS) {
//        cout << "Cell " << cell.identifier << " at (" << cell.xmargin << ", " << cell.ymargin << ")" << endl;
//    }
    RandomPlacementCost();
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
    
    if(c1.identifier>=0&&c1.identifier<CELLS.size()&&c2.identifier>=0&&c2.identifier<CELLS.size()&&c1.identifier!=c2.identifier){
        int i1=c1.identifier;
        int i2=c2.identifier;
        CELLS[i1].xmargin=c2.xmargin;
        CELLS[i1].ymargin=c2.ymargin;
        CELLS[i2].xmargin=c1.xmargin;
        CELLS[i2].ymargin=c1.ymargin;
        
        Update_Cost(CELLS,c1.identifier,c2.identifier);// new update cost with cell ids as indexes
        grid[c1.xmargin][c1.ymargin]=c2.identifier;
        grid[c2.xmargin][c2.ymargin]=c1.identifier;
        
    }
    else if(c1.identifier>=0&&c1.identifier<CELLS.size()&&c2.identifier==-1){
        
        int i1=c1.identifier;
        CELLS[i1].xmargin=c2.xmargin;
        CELLS[i1].ymargin=c2.ymargin;
        Update_Cost(CELLS,c1.identifier,-1);// new update cost with cell ids as indexes
        grid[c1.xmargin][c1.ymargin]=c2.identifier;
        grid[c2.xmargin][c2.ymargin]=c1.identifier;
    }
    else if(c2.identifier>=0&&c2.identifier<CELLS.size()&&c1.identifier==-1){
        
        int i2=c2.identifier;
        CELLS[i2].xmargin=c1.xmargin;
        CELLS[i2].ymargin=c1.ymargin;
        Update_Cost(CELLS,-1,c2.identifier);// new update cost with cell ids as indexes
        grid[c1.xmargin][c1.ymargin]=c2.identifier;
        grid[c2.xmargin][c2.ymargin]=c1.identifier;
    }
    
}

  vector<int> total_wire_length; //used to plot the temperature vs TWL graph
  vector<double> temp; //used to plot the temperature vs TWL graph
  vector<float> coolingrate = {0.95, 0.9, 0.85, 0.8, 0.75};  // used to plot the cooling rate vs TWL graph
  vector<int> totalgraphlength; // used to plot the cooling rate vs TWL graph


void simulate_annealing(int intial_wire_length, float coolingrate){
    int cell_identfier_1,cell_identfier_2;
    int cell_x1,cell_x2,cell_y1,cell_y2;
    int HPWL_1,HPWL_2,HPWL_diff;
    int N_moves=CELLS.size();
   // float coolingrate = 0.95;
    //cout<<"cell numbers"<<cell_cord.size()<<"\n";
    
    Temperature T(intial_wire_length,nets.size());
    double curr_temp=T.initialTemp;
    double prob;
    double rand_doubl;
    // minstd_rand rng(time(0));
    unsigned seed = 12345; 
    minstd_rand rng(seed);
   
    temp.push_back(T.initialTemp);
    uniform_int_distribution<int> intRowsRange(0, row-1);
    uniform_int_distribution<int> intColumnsRange(0, column-1);
    uniform_real_distribution<double> doubleDist(0, 1);
    int counter=0;
    int HPWL_3=0;
    milliseconds t = milliseconds(0);
  
    int step = 0;
    while(curr_temp>T.FinalTemp){
        
        for(int i=0;i<20*N_moves; ++i){
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
                    
                    // cout<<"random double"<<rand_doubl<<endl;
                    // cout<<"rng"<<rng<<endl;
                }
            }
            
        }

        HPWL_3=get_tot_length();
        curr_temp = curr_temp * coolingrate;
        total_wire_length.push_back(HPWL_3);
        temp.push_back(curr_temp);
//        saveGridStateToFile(grid, step); // gif
        step++;
    }
    
    cout<< " total number of iterations => "<<counter<<endl;
    cout<< " total time taken in swap => " <<t.count()<<endl;
    
}


void InitialGrid(int indc){
    cout << "--------------------------------------------------------------------------------------------------------------------------------\n";
    grid.resize(row);
    for(int i = 0; i < grid.size(); i++) {
        grid[i].resize(column, -1);
    }
    if(!indc){
        for (int i = 0; i<row; i++) {
            
            for (int j = 0; j<column; j++) {
                if (grid[i][j] == -1) {
                    // cout << setw(4)<<"-"<< " "; // Print the empty cells
                    cout <<"1"<< " ";
                    
                } else{
                    //cout << setw(4)<<grid[i][j] << " "; // Print the value of each cell
                    cout <<"0"<< " ";
                }
            }
            cout<<endl;
        }
    }
    else{
        for (int i = 0; i<row; i++) {
            
            for (int j = 0; j<column; j++) {
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

void plot_graphs()
{
ofstream file("graph.csv");
    if (file.is_open()) {
        for (size_t i = 0; i < total_wire_length.size(); ++i) {
            file <<temp[i]  << "," << total_wire_length[i] << "\n";
        }
        file.close();
    } else {
        std::cerr << "Unable to open file";
    }


    const char* command =
             "python -c \""
    "import matplotlib.pyplot as plt; "
    "import numpy as np; "
    "data = np.loadtxt('graph.csv', delimiter=','); "  
    "x = data[:, 0]; "
    "y = data[:, 1]; "
    "plt.plot(x, y, 'b-'); " 
    "plt.xlabel('Temperature'); "
    "plt.ylabel('Total Wire Length'); "
    "plt.gca().invert_xaxis(); " 
    "plt.title('Wire Length vs Temperature'); "
    "plt.grid(True); "
    "plt.show()\"";
    std::system(command);

}
void plot_coolingrate()
{
ofstream file("coolrate.csv");
 if (file.is_open()) {
        for (size_t i = 0; i < coolingrate.size(); ++i) {
            file <<coolingrate[i] << "," << totalgraphlength[i] << "\n";
        }
        file.close();
    } else {
        std::cerr << "Unable to open file";
    }


    const char* command =
             "python -c \""
    "import matplotlib.pyplot as plt; "
    "import numpy as np; "
    "data = np.loadtxt('coolrate.csv', delimiter=','); "  
    "x = data[:, 0]; "
    "y = data[:, 1]; "
    "plt.plot(x, y, 'b-'); " 
    "plt.xlabel('Cooling Rate'); "
    "plt.ylabel('Total Wire Length'); "
   
    "plt.title('Wire Length vs Temperature'); "
    "plt.grid(True); "
    "plt.show()\"";
    std::system(command);


}
int main(int argc, char* argv[]){
    
    // Temperature obj;
    string FileName = argv[1];
    Parsing_and_Assigning(FileName);
    
    int intial_wire_length=get_tot_length();
    cout<<"\nTotal Wire Length is " <<intial_wire_length<<endl;// outputting the wire length
    
    // for(int i=0; i< coolingrate.size();i++)
    // {
    auto start = steady_clock::now();
    simulate_annealing(intial_wire_length, coolingrate[0]);
    auto end=steady_clock::now();
    int final_wire_lenght=get_tot_length();
    InitialGrid(1);
    cout<<"\nTotal Wire Length is " << final_wire_lenght<<endl;

    totalgraphlength.push_back(final_wire_lenght);
    auto delta_time= duration_cast<milliseconds>(end-start);
    cout<<"Time Taken: "<<delta_time.count()/1000.0<<" seconds"<<endl;
 //   }
    plot_graphs();
  //  plot_coolingrate();
    //    cout<<endl<<"Cells and their net connections: \n";
    //    for(int i=0;i<cell_cord.size();i++){
    //        cout<<cell_cord[i].identifier<<"("<<cell_cord[i].xmargin<<","<<cell_cord[i].ymargin<<") connections: ";
    //        for(int j=0;j<cell_cord[i].connections.size();j++){
    //            cout<<cell_cord[i].connections[j]<<" , ";
    //        }
    //        cout<<endl;
    //    }
//    for (int i =0; i<CELLS[15].connections.size(); i++){
//        
//        cout<<CELLS[15].connections[i]<<endl; // hena masaln tal3ly 1,2,6,14 which are the indexes of the nets associated.
//        
//    }
//    NETS[2].maxX=7;
//    NETS[2].maxY=3;
//    NETS[2].minY=1;
//    NETS[2].minX=3;
//    
//    cout<<CELLS[15].connections[1];
//    
//    cout<<CELLS[15].ymargin;
//    cout<<NETS[CELLS[15].connections[1]].cells[14];
//    cout<<"da eh now=>"<<cost_new[0].tot_len<<endl;
//    cout<<cell_cord.size()<<"Compared to"<<CELLS.size();
    
}
//g++ main.cpp -O3 -lX11 -lpthread -std=c++17 -march=native -funroll-loops -ffast-math -o main
