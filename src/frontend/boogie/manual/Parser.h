#ifndef FRONTEND_BOOGIE_PARSER_H
#define FRONTEND_BOOGIE_PARSER_H

#include "../AST.h"
#include "../../parser/ParserBase.h"
#include "common/data_types.h"
#include <functional>

namespace frontend{
namespace boogie{
namespace parser{
using common::String;
using AST::Program;
using std::string;
using frontend::parser::Exception;

class MLCommentException : public Exception{};
class SkipException : public Exception{};
class StringLiterlExceedsLineException : public Exception{};
class StringLiterlExceedsFileException : public Exception{};
class LexException : public Exception{};

void parse(const common::String& input, Program& output);

}//namespace parser
}//namespace boogie
}//namespace frontend

#endif // PARSER_H
