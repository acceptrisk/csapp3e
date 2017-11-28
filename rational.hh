#ifndef __RATIONAL_HH__
#define __RATIONAL_HH__
#include <iostream>

template<typename T> class Rational;

template<typename T>
const Rational<T> doMultiply(const Rational<T>& lhs,
                             const Rational<T>& rhs)
{
  return Rational<T>(lhs.numerator() * rhs.numerator(), 
                     lhs.denominator() * rhs.denominator());
}

template<typename T>
class Rational {
  public:
    Rational(T num=0, T den=0):numerator_(num),
                               denominator_(den)
    {}
    
    const T numerator() const {
      return numerator_;
    }

    const T denominator() const {
      return denominator_;
    }
    
    void print(std::ostream& os) const {
        os << "(" << numerator() << ", " << denominator() << ")";
    }

    friend 
      const Rational<T> operator*(const Rational<T>& lhs,
                                  const Rational<T>& rhs)
      {
          return doMultiply(lhs, rhs);
      }

  private:
    T numerator_;
    T denominator_;

};

#endif //__RATIONAL_HH__
