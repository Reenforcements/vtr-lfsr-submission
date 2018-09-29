//
// Created by imaustyn on 9/21/18.
//

#ifndef VTR_LFSR_H
#define VTR_LFSR_H

#include "types.h"
#include <string>

void instantiate_lfsr(nnode_t *node, short mark, netlist_t *netlist);
void lfsr_ify_names_in_node_list(nnode_t **list, size_t count, std::string *expectedName);
char *lfsr_ify_name(char*name_in, std::string *expectedName);
void print_info_about_node(nnode_t *node);

#endif //VTR_LFSR_H
