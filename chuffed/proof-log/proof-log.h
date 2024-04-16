#ifndef CHUFFED_PROOF_LOGGING_H
#define CHUFFED_PROOF_LOGGING_H

#ifdef PROOF_LOGGING

#include <chuffed/core/sat-types.h>

namespace proof_log {

struct Conclusion {
private:
	bool is_unsat{false};
	Lit lit{};

	Conclusion() = default;

public:
	/**
	 * Create a conclusion which states the problem is unsatisfiable.
	 */
	static Conclusion unsatisfiable();

	/**
	 * Create a conclusion which states the given literal asserts the lower bound
	 * of the objective value.
	 */
	static Conclusion optimal(Lit bounds_lit);

	bool is_unsatisfiable() const { return is_unsat; }
	Lit objective_bound_lit() const { return lit; }
};

/**
 * Setup state to support proof logging.
 */
void init();

/**
 * Introduce a clause which is used in conflict analysis. The ID of the clause
 * will be used in the first `resolve()` call, as one of the antecedents.
 */
void intro(Clause& cl);

/**
 * Add a resolve step to the proof log. This will use the antecedents to
 * introduce the required clauses for cl to be RUP.
 */
void resolve(Clause& cl);

/**
 * Indicate the given clause is deleted.
 *
 * It is expected that `resolve` has been called with this clause previously.
 */
void del(Clause& cl);

/**
 * Finish logging and clean up resources, to prepare for process exit.
 */
void finalize(Conclusion conclusion);

/**
 * Get the number of seconds spent logging.
 */
float get_total_logging_time_seconds();

};  // namespace proof_log

#endif
#endif
