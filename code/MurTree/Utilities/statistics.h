//Distributed under the MIT license, see License.txt
//Copyright © 2022 Emir Demirović

#pragma once

namespace MurTree
{
	struct Statistics
	{
		Statistics()
		{
			num_terminal_nodes_with_node_budget_one = 0;
			num_terminal_nodes_with_node_budget_two = 0;
			num_terminal_nodes_with_node_budget_three = 0;

			time_in_terminal_node = 0;

			/*num_nodes_processed_terminal = 0;
			num_nodes_processed_nonterminal = 0;

			num_nodes_pruned_weighted = 0;
			num_nodes_pruned_children_bound = 0;
			num_nodes_pruned_sibling_bound = 0;
			num_nodes_pruned_sibling_bound_lb = 0;
			num_nodes_pruned_node_bound = 0;
			num_nodes_closed_trivially = 0;

			num_cache_hit_optimality = 0;
			num_cache_miss_optimality = 0;
			num_cache_hit_optimality_weighted = 0;
			num_cache_hit_nonzero_bound = 0;*/
		}
		int num_terminal_nodes_with_node_budget_one;
		int num_terminal_nodes_with_node_budget_two;
		int num_terminal_nodes_with_node_budget_three;

		double time_in_terminal_node;
	};
}