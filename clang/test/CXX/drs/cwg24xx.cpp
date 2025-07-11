// RUN: %clang_cc1 -std=c++98 -pedantic-errors %s -verify=expected,cxx98-14
// RUN: %clang_cc1 -std=c++11 -pedantic-errors %s -verify=expected,cxx98-14
// RUN: %clang_cc1 -std=c++14 -pedantic-errors %s -verify=expected,cxx98-14
// RUN: %clang_cc1 -std=c++17 -pedantic-errors %s -verify=expected,since-cxx17
// RUN: %clang_cc1 -std=c++20 -pedantic-errors %s -verify=expected,since-cxx20,since-cxx17
// RUN: %clang_cc1 -std=c++23 -pedantic-errors %s -verify=expected,since-cxx20,since-cxx17
// RUN: %clang_cc1 -std=c++2c -pedantic-errors %s -verify=expected,since-cxx20,since-cxx17

namespace cwg2406 { // cwg2406: 5
#if __cplusplus >= 201703L
void fallthrough(int n) {
  void g(), h(), i();
  switch (n) {
  case 1:
  case 2:
    g();
    [[fallthrough]];
  case 3: // warning on fallthrough discouraged
    do {
      [[fallthrough]];
      // since-cxx17-error@-1 {{fallthrough annotation does not directly precede switch label}}
    } while (false);
  case 6:
    do {
      [[fallthrough]];
      // since-cxx17-error@-1 {{fallthrough annotation does not directly precede switch label}}
    } while (n);
  case 7:
    while (false) {
      [[fallthrough]];
      // since-cxx17-error@-1 {{fallthrough annotation does not directly precede switch label}}
    }
  case 5:
    h();
  case 4: // implementation may warn on fallthrough
    i();
    [[fallthrough]];
    // since-cxx17-error@-1 {{fallthrough annotation does not directly precede switch label}}
  }
}
#endif
} // namespace cwg2406

namespace cwg2428 { // cwg2428: 19
#if __cplusplus >= 202002L
template <typename>
concept C [[deprecated]] = true; // #cwg2428-C

template <typename>
[[deprecated]] concept C2 = true;
// since-cxx20-error@-1 {{expected unqualified-id}}

template <typename T>
concept C3 = C<T>;
// since-cxx20-warning@-1 {{'C' is deprecated}}
//   since-cxx20-note@#cwg2428-C {{'C' has been explicitly marked deprecated here}}

template <typename T, C U>
// since-cxx20-warning@-1 {{'C' is deprecated}}
//   since-cxx20-note@#cwg2428-C {{'C' has been explicitly marked deprecated here}}
requires C<T>
// since-cxx20-warning@-1 {{'C' is deprecated}}
//   since-cxx20-note@#cwg2428-C {{'C' has been explicitly marked deprecated here}}
void f() {
  bool b = C<int>;
  // since-cxx20-warning@-1 {{'C' is deprecated}}
  //   since-cxx20-note@#cwg2428-C {{'C' has been explicitly marked deprecated here}}
};

void g(C auto a) {};
// since-cxx20-warning@-1 {{'C' is deprecated}}
//   since-cxx20-note@#cwg2428-C {{'C' has been explicitly marked deprecated here}}

template <typename T>
auto h() -> C auto {
// since-cxx20-warning@-1 {{'C' is deprecated}}
//   since-cxx20-note@#cwg2428-C {{'C' has been explicitly marked deprecated here}}
  C auto foo = T();
  // since-cxx20-warning@-1 {{'C' is deprecated}}
  //   since-cxx20-note@#cwg2428-C {{'C' has been explicitly marked deprecated here}}
  C auto *bar = T();
  // since-cxx20-warning@-1 {{'C' is deprecated}}
  //   since-cxx20-note@#cwg2428-C {{'C' has been explicitly marked deprecated here}}
  C auto &baz = T();
  // since-cxx20-warning@-1 {{'C' is deprecated}}
  //   since-cxx20-note@#cwg2428-C {{'C' has been explicitly marked deprecated here}}
  C auto &&quux = T();
  // since-cxx20-warning@-1 {{'C' is deprecated}}
  //   since-cxx20-note@#cwg2428-C {{'C' has been explicitly marked deprecated here}}
  return foo;
}
#endif
} // namespace cwg2428

namespace cwg2430 { // cwg2430: 2.7
struct S {
  S f(S s) { return s; }
};
} // namespace cwg2430

namespace cwg2450 { // cwg2450: 18
#if __cplusplus >= 202302L
struct S {int a;};
template <S s>
void f(){}

void test() {
f<{0}>();
f<{.a= 0}>();
}

#endif
} // namespace cwg2450

namespace cwg2459 { // cwg2459: 18
#if __cplusplus >= 202302L
struct A {
  constexpr A(float) {}
};
template<A> struct X {};
X<1> x;
#endif
} // namespace cwg2459

namespace cwg2445 { // cwg2445: 19
#if __cplusplus >= 202002L
  template <typename> constexpr bool F = false;
  template <typename T> struct A { };

  template <typename T, typename U>
  bool operator==(T, A<U *>);

  template <typename T, typename U>
  bool operator!=(A<T>, U) {
   static_assert(F<T>, "Isn't this less specialized?");
   return false;
  }

  bool f(A<int> ax, A<int *> ay) { return ay != ax; }

  template<class T> concept AlwaysTrue=true;
  template <class T> struct B {
    template <AlwaysTrue U>
    bool operator==(const B<U>&)const;
  };


  template <typename U>
  bool operator==(const B<int>&,const B<U>&) {
   static_assert(F<int>, "Isn't this less specialized?");
   return false;
  }

  bool g(B<int> bx, B<int *> by) { return bx == by; }

  struct C{
    template<AlwaysTrue T>
    int operator+(T){return 0;}
    template<class T>
    void operator-(T){}
  };
  template<class T>
  void operator+(C&&,T){}
  template<AlwaysTrue T>
  int operator-(C&&,T){return 0;}

  void t(int* iptr){
    int x1 = C{} + iptr;
    int x2 = C{} - iptr;
  }

  struct D{
    template<AlwaysTrue T>
    int operator+(T) volatile {return 1;}
  };

  template<class T>
  void operator+(volatile D&,T) {}

  int foo(volatile D& d){
    return d + 1;
  }
#endif
} // namespace cwg2445

namespace cwg2486 { // cwg2486: 4 c++17
struct C {
  void fn() throw();
};

static void call(C& c, void (C::*f)()) {
  (c.*f)();
}

static void callNE(C& c, void (C::*f)() throw()) {
// cxx98-14-warning@-1 {{mangled name of 'callNE' will change in C++17 due to non-throwing exception specification in function signature}}
  (c.*f)();
}

void ref() {
  C c;
  call(c, &C::fn); // <= implicit cast removes noexcept
  callNE(c, &C::fn);
}

void (*p)();
void (*pp)() throw() = p;
// since-cxx17-error@-1 {{cannot initialize a variable of type 'void (*)() throw()' with an lvalue of type 'void (*)()': different exception specifications}}

struct S {
  typedef void (*p)();
  operator p(); // #cwg2486-conv
};
void (*q)() throw() = S();
// since-cxx17-error@-1 {{no viable conversion from 'S' to 'void (*)() throw()'}}
//   since-cxx17-note@#cwg2486-conv {{candidate function}}
} // namespace cwg2486


namespace cwg2496 { // cwg2496: 21
#if __cplusplus >= 201102L
struct S {
    virtual void f(); // #cwg2496-f
    virtual void g() &; // #cwg2496-g
    virtual void h(); // #cwg2496-h
    virtual void i();
    virtual void j() &;
    virtual void k() &&;
    virtual void l() &;
};

struct T : S {
    virtual void f() &;
    // expected-error@-1 {{cannot overload a member function with ref-qualifier '&' with a member function without a ref-qualifier}}
    // expected-note@#cwg2496-f {{previous declaration is here}}
    virtual void g();
    // expected-error@-1 {{cannot overload a member function without a ref-qualifier with a member function with ref-qualifier '&'}}
    // expected-note@#cwg2496-g {{previous declaration is here}}
    virtual void h() &&;
    // expected-error@-1 {{cannot overload a member function with ref-qualifier '&&' with a member function without a ref-qualifier}}
    // expected-note@#cwg2496-h {{previous declaration is here}}
    virtual void i();
    virtual void j() &;
    virtual void k() &;
    virtual void l() &&;
};
#endif
}
