#ifndef SEMANTIC_ANALYZER_HEADER
#define SEMANTIC_ANALYZER_HEADER

#include "../../support/type/CompilationStatus.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

// Entry point for semantic analysis
CompilationStatus executeSemanticAnalysis(Program* program);

#endif
