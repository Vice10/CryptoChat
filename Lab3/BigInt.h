#pragma once
#ifndef _BIGINT_H
#define _BIGINT_H

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <string>
using namespace std;
typedef unsigned int uint;
typedef unsigned char uc;
#include "tvector.h"            // for sequence of digits
#include <map>

class BigInt
{
  public:
    
    BigInt();                    // default constructor, value = 0
    BigInt(int);                 // assign an integer value
    BigInt(const string &);    // assign a string
    
    BigInt(const BigInt & rhs);
    const BigInt & operator = (const BigInt &);
    ~BigInt();
    
    // operators: arithmetic, relational

    const BigInt & operator += (const BigInt &);
    const BigInt & operator -= (const BigInt &);
    const BigInt & operator *= (const BigInt &);
    const BigInt & operator *= (int num);
    const BigInt & operator /= (const BigInt &);
    const BigInt & operator /= (int num);
    const BigInt & operator %= (const BigInt &);
    BigInt abs(const BigInt& num);
    BigInt pow(const BigInt& a, const BigInt& b);
    BigInt sqrt(const BigInt& n);
    BigInt phi(BigInt n);
    int mulInv(int a, int b);
    BigInt mulInv(BigInt a, BigInt b);
    //int chineseRemainder(vector<int> n, vector<int> a);
    BigInt chineseRemainder(const vector<BigInt>& a, const vector<BigInt>& n);
    BigInt GCD(BigInt a, BigInt b);

    const bool isZero()const { if (myDigits[0] == '0') return true; else return false; };

   
    string   ToString() const;   // convert to string
    int      ToInt()    const;   // convert to int
    double   ToDouble() const;   // convert to double 

    // facilitate operators ==, <, << without friends

    bool Equal(const BigInt & rhs)        const;
    bool LessThan(const BigInt & rhs)     const;
    //void SetNegative() { mySign = negative; }
    //void SetPositive() { mySign = positive; }

  private:
    
    // other helper functions

    bool IsNegative()  const;    // return true iff number is negative
    bool IsPositive()  const;    // return true iff number is positive
    int  NumDigits()   const;    // return # digits in number
    
    int  GetDigit(int k) const;
    void AddSigDigit(int value);
    void ChangeDigit(int k, int value);

    void Normalize();
    void DivideHelp(const BigInt & lhs, const BigInt & rhs,
                    BigInt & quotient, BigInt & remainder);

    // private state/instance variables

    enum Sign{positive,negative};
    Sign mySign;                // is number positive or negative
    tvector<char> myDigits;     // stores all digits of number
    int myNumDigits;		// stores # of digits of number 
};
BigInt LCM(const vector<BigInt>& vect);
BigInt LCM(BigInt a, BigInt b);
BigInt karatsuba(const BigInt& lhs, const BigInt& rhs);

ostream & operator <<(ostream &, const BigInt &);
istream & operator >>(istream &, BigInt &);

BigInt operator +(const BigInt & lhs, const BigInt & rhs);
BigInt operator -(const BigInt & lhs, const BigInt & rhs);
BigInt operator *(const BigInt & lhs, const BigInt & rhs);
BigInt operator *(const BigInt & lhs, int num);
BigInt operator *(int num, const BigInt & rhs);
BigInt operator /(const BigInt & lhs, const BigInt & rhs);
BigInt operator /(const BigInt & lhs, int num);
BigInt operator %(const BigInt & lhs, const BigInt & rhs);


bool operator == (const BigInt & lhs, const BigInt & rhs);
bool operator <  (const BigInt & lhs, const BigInt & rhs);
bool operator != (const BigInt & lhs, const BigInt & rhs);
bool operator >  (const BigInt & lhs, const BigInt & rhs);
bool operator >= (const BigInt & lhs, const BigInt & rhs);
bool operator <= (const BigInt & lhs, const BigInt & rhs);


#endif   // _BIGINT_H not defined
