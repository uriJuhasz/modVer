#include "ParseTree.h"

namespace frontend{
namespace boogie{
namespace ParseTree{

ConstantOrderSpec::ConstantOrderSpec(){}
ConstantOrderSpec::ConstantOrderSpec(const ConstantOrderSpec& o)
    : specified(o.specified), complete(o.complete), parents(o.parents){}
pConstantOrderSpec ConstantOrderSpec::clone() const{return make_unique<ConstantOrderSpec>(*this); }


}
}
}
