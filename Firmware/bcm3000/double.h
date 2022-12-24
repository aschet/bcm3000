// Most of the code in this file was derived from the f64lib samples.

#include <fp64lib.h>

class Double
{
public:
    Double() : value(0ULL) {}

    Double(double f) :value(fp64_sd(f)) { }
        
    Double(const Double &d) :value(d.value) { }

    const char *toString() const {
      return fp64_to_string(value, 17, 15);
    }
    
    Double& operator+=(const Double &other) {
      this->value = fp64_add(this->value, other.value);
      return (*this);
    }

    Double operator+(const Double &other) const {
      Double res;
      res.value = fp64_add(this->value, other.value);
      return (res);
    }
    
    Double& operator-=(const Double &other) {
      this->value = fp64_sub(this->value, other.value);
      return (*this);
    }
    
    Double operator-(const Double &other) const {
      Double res;
      res.value = fp64_sub(this->value, other.value);
      return (res);
    }

    Double& operator*=(const Double &other) {
      this->value = fp64_mul(this->value, other.value);
      return (*this);
    }
    
    Double operator*(const Double &other) const {
      Double res;
      res.value = fp64_mul(this->value, other.value);
      return (res);
    }
    
    Double& operator/=(const Double &other) {
      this->value = fp64_div(this->value, other.value);
      return (*this);
    }
    
    Double operator/(const Double &other) const {
      Double res;
      res.value = fp64_div(this->value, other.value);
      return (res);
    }
    
    bool operator<(const Double & other) const {
      return fp64_compare(this->value, other.value) == 3;
    }

    bool operator>(const Double & other) const {
      return fp64_compare(this->value, other.value) == 5;
    }

    operator double() const { 
      return fp64_ds(this->value);
    }

    float64_t value;
};

Double log10(const Double& f) {
  Double res;
  res.value = fp64_log10(f.value);
  return res;
}

bool isnan(const Double& f) {
  return fp64_isnan(f.value) || fp64_isinf(f.value);
}
