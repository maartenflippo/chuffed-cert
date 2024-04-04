#include "proof-log.h"
#include <chuffed/core/options.h>
#include <chuffed/core/sat.h>
#include <chuffed/support/vec.h>

#include <fstream>
#include <iostream>

#ifdef PROOF_LOGGING

static std::ofstream log_file;

struct encountered_atom {
	bool is_bound{false};
	int var{0};
};

static std::vector<unsigned int> encountered_vars;
static unsigned int next_variable_id = 1;

static size_t num_inferences = 0;
static size_t num_lemmas = 0;
static size_t num_deletions = 0;

void proof_log::init() { log_file.open(so.proof_file, std::ios::out); }

static int encounter_lit(Lit lit) {
	auto variable = var(lit);

	if (encountered_vars.size() <= variable) {
		encountered_vars.resize(variable + 1);
	}

	if (encountered_vars[variable] == 0) {
		encountered_vars[variable] = next_variable_id++;
	}

	auto variable_code = static_cast<int>(encountered_vars[variable]);
	return sign(lit) ? variable_code : -variable_code;
}

static void log_literals(Clause& cl) {
	for (int ii = 0; ii < cl.size(); ii++) {
		Lit l(cl[ii]);

		auto code = encounter_lit(l);
		log_file << " " << code;
	}
}

void proof_log::intro(Clause& cl) {
	num_lemmas += 1;

	if (so.proof_skeleton) {
		return;
	}

	// If this is a learnt clause, we will have logged it with resolve(), and
	// it will have a valid clause ID.
	if (cl.learnt) {
		return;
	}

	log_file << "l ";
	log_literals(cl);
	log_file << std::endl;
}

void proof_log::resolve(Clause& cl) {
	num_inferences += 1;

	log_file << "r " << cl.clauseID();
	log_literals(cl);
	log_file << std::endl;
}

void proof_log::del(Clause& cl) {
	num_deletions += 1;
	log_file << "d " << cl.clauseID() << std::endl;
}

static void print_literal_atom_mapping() {
	std::ofstream map_file;
	map_file.open(so.proof_map_file, std::ios::out);

	for (int variable = 0; variable < encountered_vars.size(); variable++) {
		if (encountered_vars[variable] == 0) {
			continue;
		}

		Lit l(variable, true);

		const auto& atomic_constraint = getLitString(toInt(l));
		map_file << encountered_vars[variable] << " " << atomic_constraint << std::endl;
	}

	map_file.close();
}

static void print_conclusion(proof_log::Conclusion conclusion) {
	if (conclusion.is_unsatisfiable()) {
		log_file << "c UNSAT" << std::endl;
	} else {
		auto lit = conclusion.objective_bound_lit();
		auto code = encounter_lit(lit);

		log_file << "c " << code << std::endl;
	}
}

void proof_log::finalize(proof_log::Conclusion conclusion) {
	print_conclusion(conclusion);

	log_file.close();
	print_literal_atom_mapping();

	std::cout << "% num_lemmas: " << num_lemmas << std::endl;
	std::cout << "% num_inferences: " << num_inferences << std::endl;
	std::cout << "% num_deletions: " << num_deletions << std::endl;
}

proof_log::Conclusion proof_log::Conclusion::optimal(Lit bounds_lit) {
	Conclusion conclusion;
	conclusion.is_unsat = false;
	conclusion.lit = bounds_lit;
	return conclusion;
}

proof_log::Conclusion proof_log::Conclusion::unsatisfiable() {
	Conclusion conclusion;
	conclusion.is_unsat = true;
	return conclusion;
}

#endif
