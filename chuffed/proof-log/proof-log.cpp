#include "proof-log.h"
#include <chuffed/core/options.h>
#include <chuffed/core/sat.h>
#include <chuffed/support/vec.h>

#include <fstream>

#ifdef PROOF_LOGGING

static std::ofstream log_file;

struct encountered_atom {
	bool is_bound{false};
	int var{0};
};

static std::vector<bool> encountered_vars;

void proof_log::init() { log_file.open(so.proof_file, std::ios::out); }

static void encountered_variable(int variable) {
	if (encountered_vars.size() <= variable) {
		encountered_vars.resize(variable + 1);
	}

	encountered_vars[variable] = true;
}

static void log_literals(Clause& cl) {
	for (int ii = 0; ii < cl.size(); ii++) {
		Lit l(cl[ii]);

		encountered_variable(var(l));

		log_file << (sign(l) ? "" : "-") << var(l) + 1 << " ";
	}
}

void proof_log::intro(Clause& cl) {
	if (so.proof_skeleton) {
		return;
	}

	// If this is a learnt clause, we will have logged it with resolve(), and
	// it will have a valid clause ID.
	if (cl.learnt) {
		return;
	}

	log_file << "i ";
	log_literals(cl);
	log_file << std::endl;
}

void proof_log::resolve(Clause& cl) {
	log_file << "r " << cl.clauseID() << " ";
	log_literals(cl);
	log_file << std::endl;
}

void proof_log::del(Clause& cl) { log_file << "d " << cl.clauseID() << std::endl; }

static void print_literal_atom_mapping() {
	for (int variable = 0; variable < encountered_vars.size(); variable++) {
		if (!encountered_vars[variable]) {
			continue;
		}

		Lit l(variable, true);

		const auto& atomic_constraint = getLitString(toInt(l));
		log_file << variable << " " << atomic_constraint << std::endl;
	}
}

void proof_log::finalize() {
	log_file << "m" << std::endl;
	print_literal_atom_mapping();

	log_file.close();
}

#endif
