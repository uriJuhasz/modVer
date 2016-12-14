#ifndef FRONTEND_BOOGIE_PARSER_H
#define FRONTEND_BOOGIE_PARSER_H

#include "../ParseTree.h"
#include "../../parser/ParserBase.h"
#include "common/data_types.h"
#include <functional>

namespace frontend{
namespace boogie{
namespace parser{
using common::String;
using ParseTree::Program;
using std::string;
using frontend::parser::Exception;
using frontend::parser::TextPos;

class BPException : public Exception{
public:BPException (const string& _what, const TextPos& _where)
	: what(_what), where(_where){}
	virtual const std::string message() const{return "At " + std::string(where) + " - " + what;}
	string what;
	frontend::parser::TextPos where;
};
class MLCommentException : public BPException{
public:MLCommentException  (const TextPos& _where)
	: BPException("multiline comment exceeds line",where){}
};
class SkipException : public BPException{
public:SkipException (const wchar_t end,const TextPos& where)
	: BPException("failed to find " + string(1,end) + " while skipping",where){}
};
class StringLiterlExceedsLineException : public BPException{
public:StringLiterlExceedsLineException  (const TextPos& where)
	: BPException("String literal exceeds line",where){}
};
class StringLiterlExceedsFileException : public BPException{
public:StringLiterlExceedsFileException  (const TextPos& where)
	: BPException("String literal exceeds file",where){}
};
class ExpectedException : public BPException{
	public:ExpectedException(const string& what, const TextPos& where)
		: BPException("expected \"" + what + "\"",where){}
	};

void parse(const common::String& input, Program& output);

}//namespace parser
}//namespace boogie
}//namespace frontend

#endif // PARSER_H
