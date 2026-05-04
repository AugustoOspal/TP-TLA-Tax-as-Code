#ifndef CALCULATOR_HEADER
#define CALCULATOR_HEADER

/**
 * We reuse the types from the AST for convenience, but you should separate
 * the layers of the backend and frontend using another group of
 * domain-specific models or DTOs (Data Transfer Objects).
 */
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include <limits.h>
#include <stdbool.h>

/** Initialize module's internal state. */
ModuleDestructor initializeCalculatorModule();

/**
 * The result of a computation. It's considered valid only if "succeed" is
 * true.
 */
typedef struct {
	bool succeeded;
	int value;
} ComputationResult;

typedef ComputationResult (*BinaryOperator)(const int, const int);

/**
 * Computes the program value using the current compiler state.
 */
ComputationResult executeCalculator(CompilerState * compilerState);

#endif

