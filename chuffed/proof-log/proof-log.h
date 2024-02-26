#ifndef CHUFFED_PROOF_LOGGING_H
#define CHUFFED_PROOF_LOGGING_H

#ifdef PROOF_LOGGING

#include <chuffed/core/sat-types.h>

namespace proof_log {

/**
 * Setup state to support proof logging.
 */
void init();

/**
 * Introduce a clause which is used in conflict analysis. The ID of the clause
 * will be used in the first `resolve()` call, as one of the antecedents.
 */
void intro(Clause* cl);

/**
 * Add a resolve step to the proof log. This will use the antecedents to
 * introduce the required clauses for cl to be RUP.
 */
void resolve(Clause* cl);

/**
 * Finish logging and clean up resources, to prepare for process exit.
 */
void finalize();

};  // namespace proof_log

#endif
#endif
