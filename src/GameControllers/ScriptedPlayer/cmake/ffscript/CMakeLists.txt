cmake_minimum_required (VERSION 3.2)
project(ffscript)

get_filename_component(FFSCRIPT_ROOT_ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/../../ffscript ABSOLUTE)

add_library(ffscript 
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/BasicFunction.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/BasicType.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/CLamdaProg.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/CodeUpdater.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/CommandTree.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/CommandUnitBuilder.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/CompilerSuite.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/CompositeConstrutorUnit.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ConditionalOperator.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/Context.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ContextScope.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ControllerExecutor.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/DefaultCommands.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/DefaultPreprocessor.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/DestructorContextScope.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/DynamicFunctionFactory.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/Executor.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ExpresionParser.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/Expression.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/expressionunit.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ExpUnitExecutor.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ExpUnitExecutor2.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/FactoryTree.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/FuncLibrary.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/FunctionFactory.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/FunctionRegisterHelper.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/FunctionScope.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/FwdCompositeConstrutorUnit.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/GlobalScope.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/GlobalScopeParser.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/InstructionCommand.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/Internal.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/InternalCompilerSuite.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/LoopScope.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/MemberVariableAccessors.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/MemoryBlock.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/Preprocessor.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/Program.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/RefFunction.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ScopedCompilingScope.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ScopedContext.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ScopeRuntimeData.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ScriptCompiler.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ScriptFunction.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ScriptRunner.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ScriptScope.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ScriptScopeParser.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ScriptTask.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/ScriptType.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/StaticContext.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/stdafx.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/StructClass.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/Supportfunctions.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/Template.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/TypeManager.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/Utils.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/Variable.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/function/DynamicFunction.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/function/DynamicFunction2.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscript/template/TemplateTypeManager.cpp
)

target_include_directories(ffscript PUBLIC ${FFSCRIPT_ROOT_ABSOLUTE}/ffscript)

add_library(ffscriptLibrary 
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscriptLibrary/GeometryLib.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscriptLibrary/MathLib.cpp
	${FFSCRIPT_ROOT_ABSOLUTE}/ffscriptLibrary/RawStringLib.cpp
)
target_include_directories(ffscriptLibrary PUBLIC ${FFSCRIPT_ROOT_ABSOLUTE}/ffscriptLibrary)

target_link_libraries(ffscriptLibrary ffscript)

set_target_properties(ffscript PROPERTIES FOLDER external)
set_target_properties(ffscriptLibrary PROPERTIES FOLDER external)