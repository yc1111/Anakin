/* Copyright (c) 2018 Baidu, Inc. All Rights Reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. 
*/

#ifndef ANAKIN_TYPE_TRAITS_EXTEND_H
#define ANAKIN_TYPE_TRAITS_EXTEND_H 

#include <string>
#include <utility>
#include <type_traits>
#include "framework/core/base.h"
#include "saber/saber_types.h"

namespace anakin {

/// Type changing for  std::vector<bool> which considered a mistake in STL.
template<typename T>
struct std_vector_type_warpper {
    typedef T type;
    typedef T ret_type;
};

template<>
struct std_vector_type_warpper<bool> {
    typedef std::string type;
    typedef bool ret_type;
};

template<>
struct std_vector_type_warpper<const bool> {
    typedef std::string type;
    typedef const bool ret_type;
};

template<typename T>
struct is_bool_type : std::is_same<T, bool> {};

template<typename T>
struct is_bool_type<const T> : std::is_same<T, bool> {};


template<bool Boolean>
struct Bool2Type{};

template<size_t index, typename Arg, typename ...Args>
struct ParamPackType;

template<size_t index, typename Arg, typename ...Args>
struct ParamPackType : ParamPackType<index-1, Args...> {};

template<typename Arg, typename ...Args>
struct ParamPackType<0, Arg, Args...> {
    typedef Arg type; 
};

template<typename T>
struct function_traits;

template<typename RetType, typename ...Args>
struct function_traits<RetType(Args...)> {
    typedef RetType return_type;
    enum {
        size = sizeof...(Args)
    };

    template<size_t index>
    struct Param {
        typedef typename ParamPackType<index, Args...>::type type;
    };
};

template<typename ClassType, typename RetType, typename ...Args>
struct function_traits<RetType(ClassType::*)(Args...) const> { 
    typedef RetType return_type; 
    enum {
        size = sizeof...(Args)
    };

    template<size_t index> 
    struct Param { 
        typedef typename ParamPackType<index, Args...>::type type; 
    };
};

template<typename LambdaFunc>
struct function_traits : function_traits< decltype( &LambdaFunc::operator() )> {};

template<typename RetType, typename ...Args>
struct function_traits<RetType(Args...) const> : function_traits<RetType(Args...)> {};

/// Judge if the function return type is void. 
template<typename>
struct is_void_function;

template<typename functor>
struct is_void_function : 
    std::is_void<typename function_traits<functor>::return_type> {};

/// Judge if the function return type is Status.
template<typename>
struct is_status_function;

template<typename functor>
struct is_status_function : 
    std::is_same<typename function_traits<functor>::return_type, Status> {};

/** 
 *  \brief Detect the real type of precision.
 */
template<Precision P>
struct PrecisionWrapper {
    typedef float type;
};

template<>
struct PrecisionWrapper<Precision::INT8> {
    typedef int8_t type;
};

template<>
struct PrecisionWrapper<Precision::FP16> { 
    typedef unsigned short type;
};

template<Precision P>
struct precision_traits {
    operator Precision() {
        return P;
    }
    typedef typename PrecisionWrapper<P>::type type;
};

/// Get the corresponding host target type.
template<typename Ttype>
struct target_host {
    typedef saber::X86 type;
};

template<>
struct target_host<saber::ARM> {
    typedef saber::ARM type;
};

template<>
struct target_host<saber::NV> {
    typedef saber::NVHX86 type;
};



} /* namespace anakin */

#endif
