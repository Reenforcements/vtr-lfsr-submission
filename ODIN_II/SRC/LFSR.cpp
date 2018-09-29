//
// Created by imaustyn on 9/21/18.
//

#include "read_blif.h"
#include "globals.h"
#include "types.h"
#include "netlist_utils.h"

#include "vtr_util.h"
#include "vtr_path.h"
#include "vtr_memory.h"
#include "vtr_error.h"
#include "vtr_time.h"

#include "LFSR.h"

// Make sure each LFSR we instantiate has a unique prefix.
unsigned int lfsr_current_number = 0;

void instantiate_lfsr(nnode_t *node, short mark, netlist_t *netlist)
{
    // Assume its always 32 bit.
    oassert(node->num_output_pins == 32); // 32 bit output
    oassert(node->num_input_pins == 34); // 32 bit input and clk/rst
    oassert(node->num_input_port_sizes == 2); // Seed and clk/rst
    oassert(node->num_output_port_sizes == 1); // LFSR output

    // Save the global netlist in a temp variable so we don't accidentally break something.
    netlist_t *verilog_netlist_temp;
    verilog_netlist_temp = verilog_netlist;

    // Load the LFSR.blif
    try {
        char lfsrBlifFileName[] = "./MY_TESTS/lfsr.blif";
        printf("Trying to read %s \n", lfsrBlifFileName);

        read_blif(lfsrBlifFileName);
        netlist_t *lfsr_netlist = verilog_netlist;
        // Put back the global one.
        verilog_netlist = verilog_netlist_temp;

        printf("Successfully read %s \n", lfsrBlifFileName);

        // Okay so, now we have to link up the loaded netlist somehow...
        // There's a function called "remap_pin_to_new_node". It seems to let you switch
        //  an input from a node to an input of another node. Let's try it!

        // Remap input pins.
        for (int i = 0; i < node->num_input_pins; i++) {
            // Since these nodes are top level, they won't come with input pins.
            allocate_more_input_pins(lfsr_netlist->top_input_nodes[i], 1);
            int newIdx =lfsr_netlist->top_input_nodes[i]->num_input_pins-1;
            // Move the input from the "AST-esque" node to the top level node in the LFSR.blif.
            remap_pin_to_new_node(node->input_pins[i], lfsr_netlist->top_input_nodes[i], newIdx);

            // Makes the node just pass the signal through
            // A single input AND gate works perfectly.
            lfsr_netlist->top_input_nodes[i]->type = LOGICAL_AND;
            // This step is critical. Copy the traversal number to the new node.
            lfsr_netlist->top_input_nodes[i]->traverse_visited = node->traverse_visited;

        }

        for(int i = 0; i < node->num_output_pins; i++)
        {
            allocate_more_output_pins(lfsr_netlist->top_output_nodes[i], 1);
            remap_pin_to_new_node(node->output_pins[i], lfsr_netlist->top_output_nodes[i], lfsr_netlist->top_output_nodes[i]->num_output_pins-1);
            lfsr_netlist->top_output_nodes[i]->type = LOGICAL_AND;
            lfsr_netlist->top_output_nodes[i]->traverse_visited = node->traverse_visited;
        }

        // Copy the traversal number to all the internal nodes too.
        for (int i = 0; i < lfsr_netlist->num_internal_nodes; i++) {
            lfsr_netlist->internal_nodes[i]->traverse_visited = node->traverse_visited;
        }

        auto expectedName = std::string("lfsr_") + std::to_string(lfsr_current_number) + std::string("_");

        // Change the names of all the nodes so there's no name collisions
        lfsr_ify_names_in_node_list(lfsr_netlist->top_input_nodes, lfsr_netlist->num_top_input_nodes, &expectedName);
        lfsr_ify_names_in_node_list(lfsr_netlist->top_output_nodes, lfsr_netlist->num_top_output_nodes, &expectedName);
        lfsr_ify_names_in_node_list(lfsr_netlist->internal_nodes, lfsr_netlist->num_internal_nodes, &expectedName);
        lfsr_ify_names_in_node_list(lfsr_netlist->ff_nodes, lfsr_netlist->num_ff_nodes, &expectedName);

    } catch(vtr::VtrError& vtr_error) {
        printf("Failed to load LFSR file: %s\n", vtr_error.what());
    }

    // Restore the global netlist
    verilog_netlist = verilog_netlist_temp;

    // Increment the global lfsr count
    lfsr_current_number++;
}

void lfsr_ify_names_in_node_list(nnode_t **list, size_t count, std::string *expectedName) {
    for (int i = 0; i < count; i++) {
        // Node name
        list[i]->name = lfsr_ify_name(list[i]->name, expectedName);

        // Inputs
        for (int j = 0; j <  list[i]->num_input_pins; j++) {
            list[i]->input_pins[j]->name = lfsr_ify_name(list[i]->input_pins[j]->name, expectedName);
        }

        // Outputs
        for (int j = 0; j <  list[i]->num_output_pins; j++) {
            list[i]->output_pins[j]->name = lfsr_ify_name(list[i]->output_pins[j]->name, expectedName);
        }
    }
}

// Prefixes the name of a node/pin on the loaded netlist if it hasn't yet been prefixed.
char * lfsr_ify_name(char*name_in, std::string *expectedName) {
    if (name_in == NULL)
        return NULL;

    auto n = std::string(name_in);

    if(n.substr(0,expectedName->length()) != *expectedName) {
        std::string s = *expectedName + std::string(n);
        return vtr::strdup(s.c_str());
    }
    // The name was already changed
    return name_in;
}

// Gives a couple lines of general info about a node.
void print_info_about_node(nnode_t *node)
{
    printf("Showing info for node named %s.\n", node->name);
    printf("Input pins: %i\n", node->num_input_pins);
    for (int i = 0; i < node->num_input_pins; i++)
    {
        printf("  Input pin names: %i: %s\n", i, node->input_pins[i]->name );
        if (node->input_pins[i]->net != NULL)
        {
            printf("    Input pin net name: %s\n", node->input_pins[i]->net->name );
            printf("        Fanout pins: %i\n", node->input_pins[i]->net->num_fanout_pins );
            for (int x = 0; x < node->input_pins[i]->net->num_fanout_pins; x++ ) {
                if (node->input_pins[i]->net->fanout_pins[x] == NULL)
                    continue;
                printf("          Fanout pin name: %i: %s\n", x, node->input_pins[i]->net->fanout_pins[x]->name );
                printf("          Fanout pin node name: %s (node type: %i)\n",
                        node->input_pins[i]->net->fanout_pins[x]->node != NULL ? node->input_pins[i]->net->fanout_pins[x]->node->name : "Null",
                       node->input_pins[i]->net->fanout_pins[x]->node != NULL ? node->input_pins[i]->net->fanout_pins[x]->node->type : -1);
            }
            printf("        Net driver pin: %i, name: %s\n", node->input_pins[i]->net->driver_pin, node->input_pins[i]->net->driver_pin->name );
            printf("        Combined: %i\n", node->input_pins[i]->net->combined );
            printf("        Cycle: %i\n", node->input_pins[i]->net->cycle );
            printf("        Values: %s\n", node->input_pins[i]->net->values );
            printf("        Pin net idx: %i\n", node->input_pins[i]->pin_net_idx );
        }
    }
    printf("Output pins: %i\n", node->num_output_pins);
    for (int i = 0; i < node->num_output_pins; i++)
    {
        printf("  Output pin names: %i: %s\n", i, node->output_pins[i]->name );
        if (node->output_pins[i]->net != NULL)
        {
            printf("    Output pin net name: %s\n", node->output_pins[i]->net->name );
            printf("        Fanout pins: %i\n", node->output_pins[i]->net->num_fanout_pins );
            for (int x = 0; x < node->output_pins[i]->net->num_fanout_pins; x++ ) {
                if (node->output_pins[i]->net->fanout_pins[x] == NULL)
                    continue;

                printf("          Fanout pin name: %i: %s\n", x, node->output_pins[i]->net->fanout_pins[x]->name );
                printf("          Fanout pin node name: %s (node type: %i)\n",
                       node->output_pins[i]->net->fanout_pins[x]->node ? node->output_pins[i]->net->fanout_pins[x]->node->name : "Null",
                       node->output_pins[i]->net->fanout_pins[x]->node != NULL ? node->output_pins[i]->net->fanout_pins[x]->node->type : -1);
            }
            printf("        Net driver pin: %i, name: %s\n", (void*)node->output_pins[i]->net->driver_pin, node->output_pins[i]->net->driver_pin->name );
            printf("        Combined: %i\n", node->output_pins[i]->net->combined );
            printf("        Cycle: %i\n", node->output_pins[i]->net->cycle );
            printf("        Values: %s\n", node->output_pins[i]->net->values );
            printf("        Pin net idx: %i\n", node->output_pins[i]->pin_net_idx );
        }
    }
}
