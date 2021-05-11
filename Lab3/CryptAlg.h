#pragma once
#include "BigInt.h"
#include <bitset>
#include "base64.h"
#include <cryptopp/modes.h>
#include <cryptopp/rijndael.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha3.h>
using namespace CryptoPP;
class PollardRhoLog
{
public:
	PollardRhoLog(BigInt n, BigInt N, BigInt alpha,	BigInt beta,BigInt x, BigInt a, 
		BigInt b): n(n), N(N), alpha(alpha), beta(beta), x(x), a(a), b(b) {};
	BigInt n;
	BigInt N;
	BigInt alpha;
	BigInt beta;
	BigInt x;
	BigInt a;
	BigInt b;
};
struct cippair {
	BigInt a;
	BigInt b;
};
class CryptAlg
{
public:
	BigInt naiveFactor(BigInt n);
	BigInt rhoPollard(BigInt N);
	BigInt rhoFloyd(BigInt n);
	BigInt floydHelper(BigInt x, BigInt c, BigInt m);
	BigInt smallGiantStep(BigInt a, BigInt b, BigInt m);
	BigInt mobious(BigInt n);
	BigInt phi(BigInt n);
	BigInt eulerPrimePower(BigInt base, BigInt power);
	vector<pair<BigInt, BigInt>> uniqueFactors(BigInt n);
	BigInt carmichael(BigInt n);
	BigInt carmichaelTPC(BigInt p, BigInt q); // carmichael function for n = p * q; p,q - prime
	BigInt jacobiSymbol(BigInt a, BigInt n);
	BigInt legendreSymbol(BigInt a, BigInt p);
	// binary exponentiation by modulo
	BigInt binpower(BigInt base, BigInt e, BigInt mod);
	bool check_composite(BigInt n, BigInt a, BigInt d, BigInt s);
	bool MillerRabin(BigInt n, BigInt iter);
	bool Baillie_psw(BigInt n, BigInt k);
	bool StrongLucas(BigInt n);
	BigInt genPrime(BigInt power); // the first prime greater than 2^(n-1)
	//Converts n to base b as a list of integers between 0 and b - 1
	//Most significant digit on the left
	vector<BigInt> convertToBase(BigInt n, BigInt b);
	cippair cipolla(BigInt n, BigInt p);
	cippair cipolla_mult(cippair p1, cippair p2, BigInt w, BigInt p);
	string textToBinary(string word);
	BigInt binaryToBigInt(string binary);
	string convertToHex(BigInt n);
	void convertToUc(BigInt n, uc* charArr, uint buffSize);
	BigInt convertToBigInt(uc* charArr, uint buffSize);
	byte* toByteArr(string str);
	string byteToString(const byte* bts, uint len);
	string aesEncrypt(string plain, string key); /// returns encrypted
	string aesDecrypt(string cipher, string key);
	string encrypt_buffer(string input, unsigned char key[AES::DEFAULT_KEYLENGTH], unsigned char iv[AES::DEFAULT_KEYLENGTH]);
	string decrypt_buffer(string input, unsigned char key[AES::DEFAULT_KEYLENGTH], unsigned char iv[AES::DEFAULT_KEYLENGTH]);
	string getDigest(string msg);
	bool verifyHash(string msg, string digest);
};