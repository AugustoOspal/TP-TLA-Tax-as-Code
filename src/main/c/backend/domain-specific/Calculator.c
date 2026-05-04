#include "Calculator.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownCalculatorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Calculator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeCalculatorModule() {
	_logger = createLogger("Calculator");
	return _shutdownCalculatorModule;
}

/** PUBLIC FUNCTIONS */

ComputationResult executeCalculator(CompilerState * compilerState) {
    logDebugging(_logger, "Executing calculator (no-op for TaC)...");
	ComputationResult computationResult = {
		.succeeded = true,
		.value = 0
	};
	return computationResult;
}
