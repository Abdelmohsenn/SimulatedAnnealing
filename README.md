Simulated Annealing Algorithm

The program parses the given netlist and then implements a random initial placement for the given netlist.
Furthermore, the intial random placement cost is used to iteratively optimize the routing and placement of cells using the annealing algorithm. 
As the temperature is initially high, we get to accept bad moves in order to make more jumps searching for a sub-optimal local minimum. 
The temperature then decreases, causing the algorithm to only accept good moves, getting us closer to the local minimum.
