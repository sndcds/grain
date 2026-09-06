#include <grain/Core/ObjectBase.hpp>

namespace Grain {

namespace {

ObjectBase::Id nextObjectId()
    noexcept {
    static ObjectBase::Id next_id = 1;
    return next_id++;
}

} // namespace

ObjectBase::ObjectBase()
    : id_(nextObjectId()) {
    const char* a =
        "dsjklsdjkldsjd sklöds klöd sjklds jklds jkld sjklds jkld sjkld sjkld sjklds jklds fdjkldf sjklfds jklf dsjkldfs jkl jkld sk";
}

} // namespace Grain