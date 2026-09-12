//
//  Vec2.cpp
//
//  Part of GrainLib
//  https://grain.one
//

#include "../../include/grain/Math/Vec2.hpp"
// #include "grain/File/File.hpp"

namespace Grain {

template <typename T>
void Vec2<T>::readFromFile(File& file) {
    /*
    x_ = file.readValue<T>();
    y_ = file.readValue<T>();
    */
}

template void Vec2<int32_t>::readFromFile(File&);
template void Vec2<int64_t>::readFromFile(File&);
template void Vec2<float>::readFromFile(File&);
template void Vec2<double>::readFromFile(File&);


template <typename T>
void Vec2<T>::writeToFile(File& file) {
    /*
    file.writeValue<T>(x_);
    file.writeValue<T>(y_);
    */
}

template void Vec2<int32_t>::writeToFile(File&);
template void Vec2<int64_t>::writeToFile(File&);
template void Vec2<float>::writeToFile(File&);
template void Vec2<double>::writeToFile(File&);

} // namespace Grain