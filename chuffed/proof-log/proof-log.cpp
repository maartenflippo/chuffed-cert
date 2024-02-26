#include "proof-log.h"
#include <chuffed/core/options.h>
#include <chuffed/core/sat.h>
#include <chuffed/support/vec.h>

#include <fstream>

#ifdef PROOF_LOGGING

static std::ofstream log_file;

void proof_log::init() { log_file.open(so.proof_file, std::ios::out); }

static void log_literals(Clause* cl) {
	for (int ii = 0; ii < cl->size(); ii++) {
		Lit l((*cl)[ii]);
		log_file << (sign(l) ? "" : "-") << var(l) + 1 << " ";
	}
}

void proof_log::intro(Clause* cl) {
	if (so.proof_skeleton) {
		return;
	}

	// If this is a learnt clause, we will have logged it with resolve(), and
	// it will have a valid clause ID.
	if (cl->learnt) {
		return;
	}

	log_file << "i ";
	log_literals(cl);
	log_file << std::endl;
}

void proof_log::resolve(Clause* cl) {
	log_file << "r " << cl->clauseID() << " ";
	log_literals(cl);
	log_file << std::endl;
}

static void print_literal_atom_mapping() {
	for (int lit = 0; lit < sat.assigns.size(); lit++) {
		const auto& atomic_constraint = getLitString(lit);
		log_file << lit << " " << atomic_constraint << std::endl;
	}
}

void proof_log::finalize() {
	log_file << "m" << std::endl;
	print_literal_atom_mapping();

	log_file.close();
}

#endif
