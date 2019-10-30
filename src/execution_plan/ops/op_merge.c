/*
* Copyright 2018-2019 Redis Labs Ltd. and Contributors
*
* This file is available under the Redis Labs Source Available License Agreement
*/

#include "op_merge.h"
#include "../../query_ctx.h"
#include "../../schema/schema.h"
#include "../../arithmetic/arithmetic_expression.h"
#include <assert.h>

/* Forward declarations. */
static OpResult MergeInit(OpBase *opBase);
static Record MergeConsume(OpBase *opBase);
static void MergeFree(OpBase *opBase);

static void _AddProperties(OpMerge *op, Record r, GraphEntity *ge, PropertyMap *props) {
	for(int i = 0; i < props->property_count; i++) {
		SIValue val = AR_EXP_Evaluate(props->values[i], r);
		GraphEntity_AddProperty(ge, props->keys[i], val);
		SIValue_Free(&val);
	}

	if(op->stats) op->stats->properties_set += props->property_count;
}


static inline Record _pullFromStream(OpBase *branch) {
	return OpBase_Consume(branch);
}

static Record _pullFromRightStream(OpMerge *op, Record lhs_record) {
	OpBase *rhs = op->op.children[1];
	// OpBase_PropagateReset(rhs);
	// Propegate record to the top of the right-hand side stream.
	if(op->rhs_arg) ArgumentSetRecord(op->rhs_arg, Record_Clone(lhs_record));
	return _pullFromStream(rhs);
}

static Record _pullFromLeftStream(OpMerge *op) {
	OpBase *left_handside = op->op.children[0];
	return _pullFromStream(left_handside);
}

static Record _createPattern(OpMerge *op, Record lhs_record) {
	if(op->create_arg) ArgumentSetRecord(op->create_arg, Record_Clone(lhs_record));
	return _pullFromStream(op->op.children[2]);
}

OpBase *NewMergeOp(const ExecutionPlan *plan, ResultSetStatistics *stats, bool have_lhs_stream) {
	/* Merge is an Apply operator with three children, with the first potentially being NULL.
	 * They will be created outside of here, as with other Apply operators (see CartesianProduct
	 * and ValueHashJoin). */
	OpMerge *op = malloc(sizeof(OpMerge));
	op->stats = stats;
	op->have_lhs_stream = have_lhs_stream;
	op->expression_evaluated = false;
	op->rhs_arg = NULL;
	op->create_arg = NULL;

	// Set our Op operations
	OpBase_Init((OpBase *)op, OPType_MERGE, "Merge", MergeInit, MergeConsume, NULL, NULL, MergeFree,
				plan);

	return (OpBase *)op;
}

static Record MergeConsume(OpBase *opBase) {
	OpMerge *op = (OpMerge *)opBase;

	Record lhs_record = NULL;
	Record rhs_record = NULL;
	Record r = NULL;
	while(true) {
		// Try to get a record from left stream.
		if(op->have_lhs_stream) {
			lhs_record = _pullFromLeftStream(op);
			if(lhs_record == NULL) return NULL; // Depleted.
		}

		// TODO remove RHS and Create returns; this operation should be eager and have both a consume and handoff context.

		// Try to get a record from right stream.
		rhs_record = _pullFromRightStream(op, lhs_record);
		if(rhs_record) {
			// Pattern was successfully matched.
			op->expression_evaluated = true;
			return rhs_record;
		}

		if(!op->expression_evaluated) {
			// Only create pattern if we have no LHS stream (and thus no bound variables)
			// or an LHS stream and no RHS stream (bound variables and the pattern did not match).
			r = _createPattern(op, lhs_record);
			return r;
		} else {
			break; // Depleted.
		}
	}

	/* Out of "infinity" loop either both left and right streams managed to produce data
	 * or we're depleted. */
	return r;
}

static OpResult MergeInit(OpBase *opBase) {
	OpMerge *op = (OpMerge *)opBase;
	assert(opBase->childCount == 3);

	// If the RHS stream is populated by an Argument tap, store a reference to it.
	OpBase *rhs_stream = op->op.children[1];
	op->rhs_arg = (Argument *)ExecutionPlan_LocateOp(rhs_stream, OPType_ARGUMENT);

	// If the create stream is populated by an Argument tap, store a reference to it.
	OpBase *create_stream = op->op.children[2];
	op->create_arg = (Argument *)ExecutionPlan_LocateOp(create_stream, OPType_ARGUMENT);

	return OP_OK;
}

static void MergeFree(OpBase *ctx) {
}

