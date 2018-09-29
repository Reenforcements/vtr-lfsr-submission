
### FILES ###

lfsr.v
	Contains a Verilog module that implements a 32-bit LFSR.

testSingle.v
	Contains a module that instantiates an LFSR using the division symbol.

testMultiple.v
    Contains a module that instantiates multiple LFSRs using the division symbol.
    One of the LFSR outputs is multiplied against the output of another.


### TESTS ###

Change directory into the `MY_TESTS` directory and then call `python ./run_tests.py` to run all tests.


### SOURCE FILE CHANGES ###

ADDED LFSR.h/LFSR.cpp
    - void instantiate_lfsr(nnode_t *node, short mark, netlist_t *netlist);
        - Replaces a division node in the net with soft logic that can be synthesized.
        - Reads ./MY_TESTS/lfsr.blif using `read_blif` from blif utilities.
            - lfsr.blif the lfsr.v module compiled separately using ODIN.
        - Replaces inputs on the original nodes with top level nodes from lfsr module.
        - Replaces output on the original nodes with top level nodes from lfsr module.
        - Sets the same traversal number of all nodes in the loaded netlist as the original node.
        - Renames each node/pin with a unique prefix and number to prevent naming collisions when instantiating multiple LFSR's.
    - void lfsr_ify_names_in_node_list(nnode_t **list, size_t count, std::string *expectedName)
        - Renames all nodes/pins in the given list (if they haven't been renamed yet)
    - char *lfsr_ify_name(char*name_in, std::string *expectedName)
        - Actually adjusts a given name (if necessary)

CHANGED netlist_create_from_ast.cpp
    - Creates an operation node with proper number of inputs/outputs where it would normally just throw an error.

CHANGED partial_map.cpp
    - Replaces the division operation node with LFSR soft logic using `instantiate_lfsr` (defined in LFSR.cpp)
