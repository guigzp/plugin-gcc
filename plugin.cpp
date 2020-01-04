#include "gcc-plugin.h"
#include "plugin-version.h"
#include "tree-pass.h"
#include "context.h"
#include "function.h"
#include "tree.h"
#include "basic-block.h"
#include "gimple-expr.h"
#include "gimple.h"
#include "gimple-pretty-print.h"
#include "gimple-iterator.h"

#include <vector>

int plugin_is_GPL_compatible;

std::vector<int> features(57);
std::vector<int> features_sum(57);

void print_results(std::vector<int> features, const char *name)
{
	printf("\n\nMétodo %s\n\n", name);
	printf("FT01 - Number of basic blocks in the method: %d\n", features[1]);
	printf("FT02 - Number of basic blocks with a single successor: %d\n", features[2]);
	printf("FT03 - Number of basic blocks with two successors: %d\n", features[3]);
	printf("FT04 - Number of basic blocks with more than two successors: %d\n", features[4]);
	printf("FT05 - Number of basic blocks with a single predecessor: %d\n", features[5]);
	printf("FT06 - Number of basic blocks with two predecessors: %d\n", features[6]);
	printf("FT07 - Number of basic blocks with more than two predecessors: %d\n", features[7]);
	printf("FT08 - Number of basic blocks with a single predecessor and a single successor: %d\n", features[8]);
	printf("FT09 - Number of basic blocks with a single predecessor and two successors: %d\n", features[9]);
	printf("FT10 - Number of basic blocks with a two predecessors and one successor: %d\n", features[10]);
	printf("FT11 - Number of basic blocks with two successors and two predecessors: %d\n", features[11]);
	printf("FT12 - Number of basic blocks with more than two successors and more than two predecessors: %d\n", features[12]);
	printf("FT13 - Number of basic blocks with number of instructions less than 15: %d\n", features[13]);
	printf("FT14 - Number of basic blocks with number of instructions in the interval [15, 500]: %d\n", features[14]);
	printf("FT15 - Number of basic blocks with number of instructions greater than 500: %d\n", features[15]);
	printf("FT16 - Number of edges in the control flow graph: %d\n", features[16]);
	printf("FT17 - Number of critical edges in the control flow graph: %d\n", features[17]);
	printf("FT18 - Number of abnormal edges in the control flow graph: %d\n", features[18]);
	printf("FT19 - Number of direct calls in the method: %d\n", features[19]);
	printf("FT20 - Number of conditional branches in the method: %d\n", features[20]);
	printf("FT21 - Number of assignment instructions in the method: %d\n", features[21]);
	printf("FT22 - Number of unconditional branches in the method: %d\n", features[22]);
	// printf("FT23 - Number of binary integer operations in the method: %d\n", features[23]);
	// printf("FT24 - Number of binary floating point operations in the method: %d\n", features[24]);
	printf("FT25 - Number of instructions in the method: %d\n", features[25]);
	printf("FT26 - Average of number of instructions in basic blocks: %d\n", features[26]);
	printf("FT27 - Average of number of phi-nodes at the beginning of a basic block: %d\n", features[27]);
	printf("FT28 - Average of arguments for a phi-node: %d\n", features[28]);
	printf("FT29 - Number of basic blocks with no phi nodes: %d\n", features[29]);
	printf("FT30 - Number of basic blocks with phi nodes in the interval [0, 3]: %d\n", features[30]);
	printf("FT31 - Number of basic blocks with more than 3 phi nodes: %d\n", features[31]);
	printf("FT32 - Number of basic block where total number of arguments for all phi-nodes is in greater than 5: %d\n", features[32]);
	printf("FT33 - Number of basic block where total number of arguments for all phi-nodes is in the interval [1, 5]: %d\n", features[33]);
	printf("FT34 - Number of switch instructions in the method: %d\n", features[34]);
	printf("FT35 - Number of unary operations in the method: %d\n", features[35]);
	// printf("FT36 - Number of instruction that do pointer arithmetic in the method: %d\n", features[36]);
	// printf("FT37 - Number of indirect references via pointers (* in C): %d\n", features[37]);
	// printf("FT38 - Number of times the address of a variables is taken (& in C): %d\n", features[38]);
	// printf("FT39 - Number of times the address of a function is taken (& in C): %d\n", features[39]);
	// printf("FT40 - Number of indirect calls (i.e. done via pointers) in the method: %d\n", features[40]);
	printf("FT41 - Number of assignment instructions with the left operand an integer constant in the method: %d\n", features[41]);
	printf("FT42 - Number of binary operations with one of the operands an integer constant in the method: %d\n", features[42]);
	printf("FT43 - Number of calls with pointers as arguments: %d\n", features[43]);
	printf("FT44 - Number of calls with the number of arguments is greater than 4: %d\n", features[44]);
	printf("FT45 - Number of calls that return a pointer: %d\n", features[45]);
	printf("FT46 - Number of calls that return an integer: %d\n", features[46]);
	printf("FT47 - Number of occurrences of integer constant zero: %d\n", features[47]);
	printf("FT48 - Number of occurrences of 32-bit integer constants: %d\n", features[48]);
	printf("FT49 - Number of occurrences of integer constant one: %d\n", features[49]);
	printf("FT50 - Number of occurrences of 64-bit integer constants: %d\n", features[50]);
	printf("FT51 - Number of references of local variables in the method: %d\n", features[51]);
	printf("FT52 - Number of references (def/use) of static/extern variables in the method: %d\n", features[52]);
	printf("FT53 - Number of local variables referred in the method: %d\n", features[53]);
	printf("FT54 - Number of static/extern variables referred in the method: %d\n", features[54]);
	printf("FT55 - Number of local variables that are pointers in the method: %d\n", features[55]);
	printf("FT56 - Number of static/extern variables that are pointers in the method: %d\n", features[56]);
}

void clear_features()
{
	for (int i = 0; i < 57; i++)
	{
		features_sum[i] += features[i];
		features[i] = 0;
	}
}

namespace
{
const pass_data my_first_pass_data = {GIMPLE_PASS, "plugin_features"};
struct plugin_features : gimple_opt_pass
{
	plugin_features(gcc::context *ctx) : gimple_opt_pass(my_first_pass_data, ctx)
	{
	}
	virtual unsigned int execute(function *fun) override
	{

		clear_features();

		basic_block bb;
		edge e;
		edge_iterator ei;
		gimple_stmt_iterator gsi;
		int instructions = 0, phi_nodes = 0, aux = 0;

		features[1] = n_basic_blocks_for_fn(fun) - 2;
		features[16] = n_edges_for_fn(fun);

		FOR_EACH_BB_FN(bb, fun)
		{
			instructions = 0;
			phi_nodes = 0;
			aux = 0;
			FOR_EACH_EDGE(e, ei, bb->succs)
			{
				if (EDGE_CRITICAL_P(e) != 0)
				{
					features[17]++;
				}
				if (e->flags & EDGE_ABNORMAL)
				{
					features[18]++;
				}
			}

			gsi = gsi_start_bb(bb);
			if (gimple_code(gsi_stmt(gsi)) == GIMPLE_PHI)
			{
				features[27]++;
			}

			for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
			{
				gimple *stmt = gsi_stmt(gsi);
				instructions++;
				int args = 0;
				int ops = gimple_num_ops(stmt);
				tree rhs1, rhs2, rhs3;

				if (get_gimple_rhs_class(gimple_expr_code(stmt)) == GIMPLE_UNARY_RHS)
				{
					features[35]++;
				}

				for (int i = 0; i < ops; i++)
				{
					tree node = gimple_op(stmt, i);
					if (node != NULL)
					{
						// printf("Codigo: %s - %d\n", get_tree_code_name(TREE_CODE(node)), TREE_CODE(node));
						// printf("Tipo: %s\n", get_tree_code_name(TREE_CODE(TREE_TYPE(node))));
						// printf("Tamanho: %d\n", TYPE_PRECISION(TREE_TYPE(node)));

						if (TREE_CODE(node) == VAR_DECL)
						{
							if (DECL_EXTERNAL(node) != 0 || TREE_STATIC(node) != 0)
							{
								features[52]++;
								features[54]++;
								if (TREE_CODE(TREE_TYPE(node)) == POINTER_TYPE)
								{
									features[56]++;
								}
							}
							else
							{
								features[51]++;
								features[53]++;
								if (TREE_CODE(TREE_TYPE(node)) == POINTER_TYPE)
								{
									features[55]++;
								}
							}
						}

						if (TREE_CODE(node) == INTEGER_CST)
						{
							if (TYPE_PRECISION(TREE_TYPE(node)) == 64)
							{
								features[50]++;
							}
							else
							{
								features[48]++;
							}
						}

						if (integer_zerop(node))
						{
							features[47]++;
						}
						else if (integer_onep(node))
						{
							features[49]++;
						}
					}
				}

				switch (gimple_code(stmt))
				{

				case GIMPLE_CALL:
					features[19]++;
					if (TREE_CODE(TREE_TYPE(gimple_call_fntype(stmt))) == POINTER_TYPE)
					{
						features[45]++;
					}
					else if (TREE_CODE(TREE_TYPE(gimple_call_fntype(stmt))) == INTEGER_TYPE)
					{
						features[46]++;
					}

					args = gimple_call_num_args(stmt);

					if (args > 4)
					{
						features[44]++;
					}
					for (int i = 0; i < args; i++)
					{
						if (TREE_CODE(TREE_TYPE(gimple_call_arg(stmt, i))) == POINTER_TYPE)
						{
							features[43]++;
							break;
						}
					}

					break;

				case GIMPLE_COND:
					features[20]++;
					break;

				case GIMPLE_ASSIGN:
					features[21]++;
					if (TREE_CODE(gimple_assign_lhs(stmt)) == INTEGER_CST)
					{
						features[41]++;
					}

					if (TREE_CODE_CLASS(gimple_assign_rhs_code(stmt)) == tcc_binary)
					{
						rhs1 = gimple_assign_rhs1(stmt);
						rhs2 = gimple_assign_rhs2(stmt);
						rhs3 = gimple_assign_rhs3(stmt);

						if ((rhs1 && (TREE_CODE(rhs1) == INTEGER_CST)) || (rhs2 && (TREE_CODE(rhs2) == INTEGER_CST)) || (rhs3 && (TREE_CODE(rhs3) == INTEGER_CST)))
						{
							features[42]++;
						}
					}
					break;

				case GIMPLE_GOTO:
					features[22]++;
					break;

				case GIMPLE_SWITCH:
					features[34]++;
					break;

				case GIMPLE_PHI:
					phi_nodes++;
					aux += gimple_phi_num_args(gsi_stmt(gsi));
					features[28] += gimple_phi_num_args(gsi_stmt(gsi));
					break;

				default:
					break;
				}
			}
			features[25] += instructions;
			if (instructions < 15)
			{
				features[13]++;
			}
			else if (instructions <= 500)
			{
				features[14]++;
			}
			else
			{
				features[15]++;
			}

			if (phi_nodes == 0)
			{
				features[29]++;
			}
			else if (phi_nodes <= 3)
			{
				features[30]++;
			}
			else
			{
				features[31]++;
			}

			if (aux > 5)
			{
				features[32]++;
			}
			else if (aux > 0)
			{
				features[33]++;
			}

			if (EDGE_COUNT(bb->succs) == 1)
			{
				features[2]++;
				if (EDGE_COUNT(bb->preds) == 1)
				{
					features[5]++;
					features[8]++;
				}
				else if (EDGE_COUNT(bb->preds) == 2)
				{
					features[6]++;
					features[10]++;
				}
				else if (EDGE_COUNT(bb->preds) > 2)
				{
					features[7]++;
				}
			}
			else if (EDGE_COUNT(bb->succs) == 2)
			{
				features[3]++;
				if (EDGE_COUNT(bb->preds) == 1)
				{
					features[5]++;
					features[9]++;
				}
				else if (EDGE_COUNT(bb->preds) == 2)
				{
					features[6]++;
					features[11]++;
				}
				else if (EDGE_COUNT(bb->preds) > 2)
				{
					features[7]++;
				}
			}
			else if (EDGE_COUNT(bb->succs) > 2)
			{
				features[4]++;
				if (EDGE_COUNT(bb->preds) == 1)
				{
					features[5]++;
				}
				else if (EDGE_COUNT(bb->preds) == 2)
				{
					features[6]++;
				}
				else if (EDGE_COUNT(bb->preds) > 2)
				{
					features[12]++;
				}
			}
			else if (EDGE_COUNT(bb->preds) == 1)
			{
				features[5]++;
			}
			else if (EDGE_COUNT(bb->preds) == 2)
			{
				features[6]++;
			}
			else if (EDGE_COUNT(bb->preds) > 2)
			{
				features[7]++;
			}
		}
		features[26] = features[25] / features[1];
		if (features[27] != 0)
		{
			features[27] / features[1];
		}
		if (features[29] == features[1])
		{
			features[28] = 0;
		}
		else
		{
			features[28] = features[28] / (features[1] - features[29]);
		}
		print_results(features, function_name(fun));
		return 0;
	}
};
} // namespace

int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
	struct register_pass_info pass_info;

	pass_info.pass = new plugin_features(g);
	pass_info.reference_pass_name = "cfg";
	pass_info.ref_pass_instance_number = 1;

	register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info);

	return 0;
}