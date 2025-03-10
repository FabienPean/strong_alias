/*
The MIT License (MIT)

Copyright (c) 2020-2025 Fabien Péan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include <utility>
#include <type_traits>

// Macro for conveniently defining a new alias
// The second argument is passed in variadic macro and should be a typename
#define STRONG_ALIAS(NAME, ...) \
struct NAME final : strong::alias<__VA_ARGS__,NAME> \
{\
	using strong::alias<__VA_ARGS__,NAME>::alias;\
}

namespace strong
{
    template<typename T>
    using rm_cvref_t = std::remove_cvref_t<T>;

    struct is_alias_t {};
    template<typename Name>
    struct alias_name : is_alias_t {};
    template<typename T, typename Name>
    struct alias;

    template<typename Arg>
    concept is_alias = std::is_base_of_v<is_alias_t, rm_cvref_t<Arg>>;
    template<typename Arg, typename Name>
    concept is_same_alias = is_alias<Arg> && std::is_base_of_v<alias_name<Name>, rm_cvref_t<Arg>>;
    template<typename Arg, typename Name>
    concept is_different_alias = is_alias<Arg> && !std::is_base_of_v<alias_name<Name>, rm_cvref_t<Arg>>;

    template <typename T, typename Name> requires std::is_scalar_v<T>
    struct alias<T, Name> : alias_name<Name>
    {
    private:
        T value;

    public:
        template<typename... Args> requires (is_alias<Args>&& ...)
        explicit constexpr alias(Args&&... args) noexcept(((std::is_lvalue_reference_v<Args>&& ...) && std::is_nothrow_copy_constructible_v<T>) or ((std::is_rvalue_reference_v<Args> && ...) && std::is_nothrow_move_constructible_v<T>))
            : value{ std::forward<Args>(args)... } { static_assert(sizeof...(Args) <= 1); };

        template<typename Arg> requires(!is_alias<Arg>)
        constexpr alias(Arg&& arg)  noexcept((std::is_lvalue_reference_v<Arg>&& std::is_nothrow_copy_constructible_v<T>) or (std::is_rvalue_reference_v<Arg> && std::is_nothrow_move_constructible_v<T>))
            : value{ std::forward<Arg>(arg) } {}

        // Implicit conversion
        constexpr operator T()       noexcept { return value; }
        constexpr operator T() const noexcept { return value; }
        // Increment/Decrement
        constexpr alias& operator++()     requires requires(T& a) {++a;} { ++value; return *this; };
        constexpr alias& operator--()     requires requires(T& a) {--a;} { --value; return *this; };
        constexpr alias  operator++(int)  requires requires(T& a) {a++;} { return alias{value++}; };
        constexpr alias  operator--(int)  requires requires(T& a) {a--;} { return alias{value--}; };
        // Assignment operators
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator= (const Arg& arg)  { value   = arg; return *this; }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator+=(const Arg& arg)  { value  += arg; return *this; }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator-=(const Arg& arg)  { value  -= arg; return *this; }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator*=(const Arg& arg)  { value  *= arg; return *this; }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator/=(const Arg& arg)  { value  /= arg; return *this; }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator%=(const Arg& arg)  { value  %= arg; return *this; }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator&=(const Arg& arg)  { value  &= arg; return *this; }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator|=(const Arg& arg)  { value  |= arg; return *this; }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator^=(const Arg& arg)  { value  ^= arg; return *this; }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator<<=(const Arg& arg) { value <<= arg; return *this; }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator>>=(const Arg& arg) { value >>= arg; return *this; }
        // Comparison operators
        template<typename Arg> requires is_different_alias<Arg,Name>
        constexpr bool operator ==(const Arg& arg) const = delete;
        template<typename Arg> requires is_different_alias<Arg,Name>
        constexpr bool operator !=(const Arg& arg) const = delete;
        template<typename Arg> requires is_different_alias<Arg,Name>
        constexpr bool operator >=(const Arg& arg) const = delete;
        template<typename Arg> requires is_different_alias<Arg,Name>
        constexpr bool operator <=(const Arg& arg) const = delete;
        template<typename Arg> requires is_different_alias<Arg,Name>
        constexpr bool operator > (const Arg& arg) const = delete;
        template<typename Arg> requires is_different_alias<Arg,Name>
        constexpr bool operator < (const Arg& arg) const = delete;
        // Logical operators
        template<typename Arg> requires is_different_alias<Arg,Name>
        constexpr bool operator &&(const Arg& arg) const = delete;
        template<typename Arg> requires is_different_alias<Arg,Name>
        constexpr bool operator ||(const Arg& arg) const = delete;
        // Member access operators
        constexpr auto* operator->() requires std::is_pointer_v<T> and std::is_class_v<std::remove_pointer_t<T>> { return value; };
        constexpr auto& operator*()  requires std::is_pointer_v<T> { return *value; };
    };

    // Specialize for class types to use inheritance
    template <typename T, typename Name> requires (std::is_class_v<T> and not std::is_final_v<T>)
    struct alias<T, Name> : alias_name<Name>, T
    {
    public:
        template<typename... Args>
        explicit constexpr alias(Args&&... args)  noexcept(((std::is_lvalue_reference_v<Args> and ...) and std::is_nothrow_copy_constructible_v<T>) or ((std::is_rvalue_reference_v<Args> and ...) and std::is_nothrow_move_constructible_v<T>))
            : T(std::forward<Args>(args)...) {}

        template<typename Arg> requires (not is_alias<rm_cvref_t<Arg>> or is_same_alias<rm_cvref_t<Arg>,Name>)
        constexpr alias(Arg&& arg) noexcept((std::is_lvalue_reference_v<Arg> and std::is_nothrow_copy_constructible_v<T>) or (std::is_rvalue_reference_v<Arg> and std::is_nothrow_move_constructible_v<T>))
            : T(std::forward<Arg>(arg)) {}

        // Increment/Decrement
        constexpr alias& operator++()    requires requires(T& a) {++a;} { ++static_cast<T&>(*this); return *this; };
        constexpr alias& operator--()    requires requires(T& a) {--a;} { --static_cast<T&>(*this); return *this; };
        constexpr alias  operator++(int) requires requires(T& a) {a++;} { alias tmp{*this}; ++(*this); return tmp; };
        constexpr alias  operator--(int) requires requires(T& a) {a--;} { alias tmp{*this}; --(*this); return tmp; };
        // Assignment operators
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator=  (const Arg& arg) { return static_cast<alias&>(T::operator  =(arg)); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator+= (const Arg& arg) { return static_cast<alias&>(T::operator +=(arg)); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator-= (const Arg& arg) { return static_cast<alias&>(T::operator -=(arg)); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator*= (const Arg& arg) { return static_cast<alias&>(T::operator *=(arg)); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator/= (const Arg& arg) { return static_cast<alias&>(T::operator /=(arg)); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator%= (const Arg& arg) { return static_cast<alias&>(T::operator %=(arg)); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator&= (const Arg& arg) { return static_cast<alias&>(T::operator &=(arg)); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator|= (const Arg& arg) { return static_cast<alias&>(T::operator |=(arg)); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator^= (const Arg& arg) { return static_cast<alias&>(T::operator ^=(arg)); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator<<=(const Arg& arg) { return static_cast<alias&>(T::operator<<=(arg)); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr alias& operator>>=(const Arg& arg) { return static_cast<alias&>(T::operator>>=(arg)); }
        // Comparison operators
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr bool operator ==(const Arg& arg) const { return T::operator==(arg); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr bool operator !=(const Arg& arg) const { return T::operator!=(arg); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr bool operator >=(const Arg& arg) const { return T::operator>=(arg); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr bool operator <=(const Arg& arg) const { return T::operator<=(arg); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr bool operator > (const Arg& arg) const { return T::operator>(arg); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr bool operator < (const Arg& arg) const { return T::operator<(arg); }
        // Logical operators
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr bool operator &&(const Arg& arg) const { return T::operator&&(arg); }
        template<typename Arg> requires(not is_alias<Arg> or is_same_alias<Arg,Name>)
        constexpr bool operator ||(const Arg& arg) const { return T::operator||(arg); }
    };
}


#ifdef STRONG_ALIAS_TEST
#include <cstdint>
#include <vector>
#include <Eigen/Dense>

int main()
{
    /// Fundamental type alias
    /////////////////////////////////////////////
    STRONG_ALIAS(A, int);
    STRONG_ALIAS(B, std::int32_t);
    { A a; A b{ std::int8_t{42} }; }        // ✔️
    { A a; A b{ a }; }                      // ✔️
    { A a; A  b = a; }                      // ✔️
    { A a, b; A a3 = a + b; }               // ✔️
    { A a; A b; a += b; }                   // ✔️
    { A a; A b; a == b; }                   // ✔️
    { A a; a++; ++a; --a; a--; }            // ✔️
    { A a; a == 1; }                        // ✔️
    { A a; a[0]; }                          // ❌
    { A a; B b; a = b + 5; }                // ✔️
    { A a; B b(a); }                        // ✔️
    { A a; B b(a + 1); }                    // ✔️
    { A a; B b; a += b; }                   // ❌
    { A a; B b; a == b; }                   // ❌
    { A a; B b = a; }                       // ❌
    { A a; B b; b = a; }                    // ❌
    { A a; [](B)  {} (a); }                 // ❌
    { A a; [](B&) {} (a); }                 // ❌
    { A a; [](B&&){} (std::move(a)); }      // ❌

    /// Pointer fundamental type alias
    /////////////////////////////////////////////
    STRONG_ALIAS(D, int*);
    int dd = 0;
    { D c{&dd}; *c = 1; }                        // ✔️
    { D c{&dd}; c.operator->(); }                // ❌
    { D c{&dd}; c[0]; }                          // ✔️
    { D c{&dd}; c += 1; c -= 1; }                // ✔️
    { D c{&dd}; c++; c--; }                      // ✔️
    { D c{&dd}; c == nullptr; c != nullptr; }    // ✔️
    { D c{&dd}; c > 0; }                         // ❌
    { D c{&dd}; c *= 1; c /= 1; }                // ❌

    /// Pointer class type alias
    /////////////////////////////////////////////
    STRONG_ALIAS(C, std::vector<double>*);
    std::vector<double> cc;
    { C c{&cc}; (*c).size(); }                   // ✔️
    { C c{&cc}; c->size(); }                     // ✔️
    { C c{&cc}; c[0].size(); }                   // ✔️
    { C c{&cc}; c += 1; c -= 1; }                // ✔️
    { C c{&cc}; c++; c--; }                      // ✔️
    { C c{&cc}; c *= 1; c /= 1; }                // ❌

    ///// Class type alias
    /////////////////////////////////////////////
    STRONG_ALIAS(X, Eigen::Matrix<double, 3, 1>);
    STRONG_ALIAS(Y, Eigen::Vector3d);
    { X a; X b{ 42.,3.14,2.4 }; }           // ✔️   
    { X a; X b{ a }; }                      // ✔️   
    { X a; a[0]+=1; }                       // ✔️  
    { X a; a(0)+=1; }                       // ✔️    
    { X a; X b; b = a; }                    // ✔️   
    { X a; X b; X a3 = a + b; }             // ✔️   
    { X a; X b; a += b; }                   // ✔️
    { X a; X b; a == b; }                   // ✔️
    { X a; Y b; a = b.array() + 5; }        // ✔️   
    { X a; Y b(a); }                        // ✔️    
    { X a; Y b(a.array() + 1); }            // ✔️    
    { X a; Y b; a += b; }                   // ❌
    { X a; Y b; a == b; }                   // ❌
    { X a; Y b = a; }                       // ❌
    { X a; Y b; b = a; }                    // ❌
    { X a; [](Y)  {} (a); }                 // ❌
    { X a; [](Y&) {} (a); }                 // ❌
    { X a; [](Y&&){} (std::move(a)); }      // ❌

    return 0;
}
// g++ -w -std=c++20 -Ieigen -DSTRONG_ALIAS_TEST strong_alias.h -o strong_alias -fmax-errors=1000 > errors.log
// grep -hr "✔️" errors.log 
// grep -hr "❌" errors.log  | cut -d'|' -f1 | sort -u
#endif
