// RUN: %clang_cc1 -analyze -analyzer-checker=core,debug.ExprInspection -analyzer-store region -analyzer-ipa=inlining -cfg-add-initializers -verify %s
// XFAIL: *

void clang_analyzer_eval(bool);

class A {
  int x;
public:
  A();
  int getx() const {
    return x;
  }
};

A::A() : x(0) {
}

class B : public A {
  int y;
public:
  B();
};

B::B() {
}

void f() {
  B b;
  clang_analyzer_eval(b.getx() == 0); // expected-warning{{TRUE}}
}
