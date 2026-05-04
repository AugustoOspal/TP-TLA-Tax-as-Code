#include "Generator.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownGeneratorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Generator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGeneratorModule() {
	_logger = createLogger("Generator");
	return _shutdownGeneratorModule;
}

/** PRIVATE FUNCTIONS */

static void _generateProgram(Program * program);

/**
 * Generates the output of the program.
 */
static void _generateProgram(Program * program) {
	logDebugging(_logger, "The AST has %s declarations.", program->declarations != NULL ? "some" : "no");
}

/** PUBLIC FUNCTIONS */

void executeGenerator(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	_generateProgram(compilerState->abstractSyntaxtTree);
	logDebugging(_logger, "Generation is done.");
}
