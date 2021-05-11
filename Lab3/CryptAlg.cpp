#include "CryptAlg.h"
#include <ctime>


BigInt CryptAlg::naiveFactor(BigInt n)
{
	uint primeList[] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97
		,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,
		227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,
		367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,
		521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,
		677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,
		857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019 };

	for (uint prime : primeList) {
		if (n.GCD(n, prime) != 1)
			return n / prime;
	}
	return n;
}

BigInt CryptAlg::rhoPollard(BigInt N)
{
	if (naiveFactor(N) != N)
		return naiveFactor(N);
	srand((unsigned)time(0));
	BigInt x = rand() % (N - 2) + 1;
	BigInt y = 1; BigInt i = 0; BigInt stage = 2;
	while (x.GCD(N, x.abs(x - y)) == 1)
	{
		if (i == stage) {
			y = x;
			stage = stage * 2;
		}
		x = (x * x + 1) % N;
		i = i + 1;
	}
	return x.GCD(N, x.abs(x - y));
}

BigInt CryptAlg::rhoFloyd(BigInt n)
{
	if (naiveFactor(n) != n)
		return naiveFactor(n);
	BigInt x0 = 2, c = 1;
	BigInt x = x0;
	BigInt y = x0;
	BigInt g = 1;
	while (g == 1) {
		x = floydHelper(x, c, n);
		y = floydHelper(y, c, n);
		y = floydHelper(y, c, n);
		g = c.GCD(c.abs(x - y), n);
	}
	return g;
}

BigInt CryptAlg::floydHelper(BigInt x, BigInt c, BigInt m)
{
	return (x * x % m + c) % m;
}

BigInt CryptAlg::smallGiantStep(BigInt a, BigInt b, BigInt m)
{
	a %= m, b %= m;
	BigInt n = a.sqrt(m) + 1;
	map<BigInt, BigInt> vals;
	for (int p = 1; p <= n; ++p)
		vals[binpower(a, p * n, m)] = p;
	for (int q = 0; q <= n; ++q) {
		BigInt cur = (binpower(a, q, m) * 1ll * b) % m;
		if (vals.count(cur)) {
			BigInt ans = vals[cur] * n - q;
			return ans;
		}
	}
	return -1;
}

BigInt CryptAlg::mobious(BigInt n)
{
	BigInt p = 0;
	if (n % 2 == 0)
	{
		n = n / 2;
		p += 1;
		if (n % 2 == 0)
			return 0;
	}

	// check for all other prime factors 
	for (int i = 3; i <= p.sqrt(n); i = i + 2)
	{
		if (n % i == 0)
		{
			n = n / i;
			p += 1;
			// If i^2 also divides N 
			if (n % i == 0)
				return 0;
		}
	}
	return (p % 2 == 0) ? -1 : 1;
}

BigInt CryptAlg::phi(BigInt n) {
	BigInt result{ n };
	BigInt i{ "2" };

	while (i * i <= n)
	{
		if (n % i == 0) {
			while (n % i == 0)
				n /= i;
			result -= result / i;
		}
		i += 1;
	}
	if (n > 1)
		result -= result / n;
	return result;
}

BigInt CryptAlg::eulerPrimePower(BigInt p, BigInt r)
{
	return p.pow(p, r - 1) * (p - 1);
}

vector<pair<BigInt, BigInt>> CryptAlg::uniqueFactors(BigInt n)
{
	vector<pair<BigInt, BigInt>> uf;
	BigInt p{};
	while (n != 1) {
		if (MillerRabin(n, 2))
			p = n;
		else {
			p = rhoFloyd(n);
			while (p > 1 && !MillerRabin(p, 2))
				p /= rhoFloyd(p);
		}
		bool flag = false;
		for (uint i = 0; i < uf.size(); i++) {
			if (uf[i].first == p) {
				flag = true;
				uf[i].second += 1;
				break;
			}
		}
		if (!flag)
			uf.push_back(make_pair(p, 1));
		n /= p;
	}
	return uf;
}

BigInt CryptAlg::carmichael(BigInt n)
{
	vector<pair<BigInt, BigInt>> uf = uniqueFactors(n);
	vector<BigInt> lambda_pr;
	BigInt pr{};
	for (uint i = 0; i < uf.size(); i++) {
		pr = n.pow(uf[i].first, uf[i].second);
		if (pr == 2 || pr == 4) {
			lambda_pr.push_back(eulerPrimePower(uf[i].first, uf[i].second));
			continue;
		}
		else if (uf[i].first == 2)
			lambda_pr.push_back(eulerPrimePower(uf[i].first, uf[i].second) / 2);
		else
			lambda_pr.push_back(eulerPrimePower(uf[i].first, uf[i].second));
	}
	BigInt res = LCM(lambda_pr);
	return res;
}

BigInt CryptAlg::carmichaelTPC(BigInt p, BigInt q)
{
	return LCM(eulerPrimePower(p, 1), eulerPrimePower(q, 1));
}

BigInt CryptAlg::jacobiSymbol(BigInt a, BigInt n)
{
	BigInt t{ 1 };
	while (a != 0) {
		while (a % 2 == 0) {
			a /= 2;
			BigInt r = n % 8;
			if (r == 3 || r == 5)
				t = -1 * t;
		}
		BigInt tmp{ a };
		a = n;
		n = tmp;
		if (a % 4 == n % 4 == 3)
			t = -1 * t;
		a %= n;
	}
	if (n == 1)
		return t;
	else
		return 0;
}

BigInt CryptAlg::legendreSymbol(BigInt a, BigInt p)
{
	if (a == 1)
	{
		return 1;
	}
	if (a % 2 == 0)
	{
		return legendreSymbol(a / 2, p) * a.pow(-1, (p * p - 1) / 8);
	}
	if ((a % 2 != 0) && (a != 1))
	{
		return legendreSymbol(p % a, a) * a.pow(-1, (a - 1) * (p - 1) / 4);
	}
	return 0;
}

BigInt CryptAlg::binpower(BigInt base, BigInt e, BigInt mod)
{
	BigInt result = 1;
	base %= mod;
	while (e > 0) {
		if (e % 2 != 0)
			result = karatsuba(result, base) % mod;
		base = karatsuba(base, base) % mod;
		e /= 2;
	}
	return result;
}

bool CryptAlg::check_composite(BigInt n, BigInt a, BigInt d, BigInt s)
{
	BigInt x = binpower(a, d, n);
	if (x == 1 || x == n - 1)
		return false;
	for (int r = 1; r < s; r++) {
		x = karatsuba(x, x) % n;
		if (x == n - 1)
			return false;
	}
	return true;
}

bool CryptAlg::MillerRabin(BigInt n, BigInt iter)
{
	if (n < 4)
		return n == 2 || n == 3;

	BigInt s = 0;
	BigInt d = n - 1;
	while (d % 2 == 0) {
		d /= 2;
		s += 1;
	}

	srand((unsigned)time(0));
	for (int i = 0; i < iter; i++) {
		BigInt a = 2 + rand() % (n - 3);
		if (check_composite(n, a, d, s))
			return false;
	}
	return true;
}

bool CryptAlg::Baillie_psw(BigInt n, BigInt k)
{
	if (n <= 1 || n == 4)
		return false;
	if (n <= 3)
		return true;

	//Find r such that n = 2^d * r + 1 for some r >= 1
	BigInt d = n - 1;
	while (d % 2 == 0)
		d /= 2;

	BigInt i = 0;
	while (i < k) {
		if (MillerRabin(n, d) == false)
			return false;
		i += 1;
	}
	return true;
}

bool CryptAlg::StrongLucas(BigInt n)
{
	BigInt dd = 5;
	while (true) {
		BigInt g = dd.GCD(n, dd.abs(dd));
		if (1 < g && g < n)
			return false;
		if (jacobiSymbol(dd, n) == -1)
			break;
		if (dd < 0)
			dd = -1 * dd + 2;
		else
			dd = -1 * dd - 2;
	}
	BigInt p = 1, q = (p * p - dd) / 4;
	BigInt d = n + 1, s = 0;
	while (d % 2 == 0) {
		s += 1;
		d /= 2;
	}
	BigInt u = 1, v = p, u2m = 1, v2m = p, qm = q, qm2 = q * 2, qkd = q;
	for (BigInt mask = 2; mask <= d; mask *= 2) {
		u2m = (u2m * v2m) % n;
		v2m = (v2m * v2m) % n;
		while (v2m < qm2)   v2m += n;
		v2m -= qm2;
		qm = (qm * qm) % n;
		qm2 = qm * 2;
		if (d % mask != 0) {
			BigInt t1 = (u2m * v) % n, t2 = (v2m * u) % n,
				t3 = (v2m * v) % n, t4 = (((u2m * u) % n) * dd) % n;
			u = t1 + t2;
			if (u % 2 != 0)
				u += n;
			u = (u /= 2) % n;
			v = t3 + t4;
			if (v % 2 != 0)
				v += n;
			v = (v /= 2) % n;
			qkd = (qkd * qm) % n;
		}
	}
	if (u == 0 || v == 0)  return true;
	BigInt qkd2 = qkd * 2;
	for (BigInt r = 1; r < s; r += 1) {
		v = (v * v) % n - qkd2;
		if (v < 0)  v += n;
		if (v < 0)  v += n;
		if (v >= n)  v -= n;
		if (v >= n)  v -= n;
		if (v == 0)  return true;
		if (r < s - 1) {
			qkd = (qkd * qkd) % n;
			qkd2 = qkd * 2;
		}
	}
	return false;
}

BigInt CryptAlg::genPrime(BigInt power)
{
	BigInt a{ 0 };

	for (BigInt i = 0; i <= power; i += 1) {
		if (rand() % 2 == 0)
			a += a.pow(2, i);
	}

	while (true) {
		if (MillerRabin(a, 2))
			break;
		a += 1;
	}
	return a;
}

vector<BigInt> CryptAlg::convertToBase(BigInt n, BigInt b)
{
	if (n < 2) {
		vector<BigInt> t;
		t.push_back(n);
		return t;
	}
	BigInt temp{ n };
	vector<BigInt> ans;
	while (temp != 0)
	{
		ans.push_back(temp % b);
		temp /= b;
	}
	reverse(ans.begin(), ans.end());
	return ans;
}

cippair CryptAlg::cipolla(BigInt n, BigInt p)
{
	n %= p;
	if (n == 0 || n == 1)
		return cippair{ n, p - n % p };

	BigInt phi{ p - 1 };

	if (binpower(n, phi / 2, p) != 1)
		return cippair{};

	if (p % 4 == 3) {
		BigInt ans = binpower(n, (p + 1) / 4, p);
		return cippair{ ans, p - ans % p };
	}
	BigInt aa{}, i{ 1 };
	while (i < p)
	{
		BigInt temp{ binpower((i * i - n) % p, phi / 2, p) };
		if (temp == phi) {
			aa = i;
			break;
		}
		i += 1;
	}
	vector<BigInt> exponent = convertToBase((p + 1) / 2, 2);
	cippair x1{ aa, 1 };
	cippair x2 = cipolla_mult(x1, x1, aa * aa - n, p);

	i = 0;
	int expSize = exponent.size();
	while (i < expSize)
	{
		if (*(exponent.end()) == 0) {
			x2 = cipolla_mult(x2, x1, aa * aa - n, p);
			x1 = cipolla_mult(x1, x1, aa * aa - n, p);
		}
		else {
			x1 = cipolla_mult(x1, x2, aa * aa - n, p);
			x2 = cipolla_mult(x2, x2, aa * aa - n, p);
		}
	}
	return cippair{ x1.a, p - x1.a % p };
}

cippair CryptAlg::cipolla_mult(cippair p1, cippair p2, BigInt w, BigInt p)
{
	return cippair{ (p1.a * p2.a + p1.b * p2.b * w) % p, (p1.a * p2.b + p1.b * p2.a) % p };
}

string CryptAlg::textToBinary(string word)
{
	string res = "";
	for (uint i = 0; i < word.size(); i++)
		res += bitset<8>(word.c_str()[i]).to_string();
	return res;
}

BigInt CryptAlg::binaryToBigInt(string binary)
{
	BigInt p{ 1 };
	BigInt res{ 0 };
	for (int i = binary.size() - 1; i > -1; i--) {
		res += (int)binary.c_str()[i] * p;
		p *= 2;
	}
	return res;
}

string hexHelper(const BigInt& num) {
	if (num < 10)
		return num.ToString();
	int nstr = num.ToInt();
	switch (nstr) {
	case 10:
		return "A";
	case 11:
		return "B";
	case 12:
		return "C";
	case 13:
		return "D";
	case 14:
		return "E";
	case 15:
		return "F";
	}
	exit(937);
}

string CryptAlg::convertToHex(BigInt n)
{
	if (n < 2)
		return n.ToString();
	string ans;
	BigInt r{};
	while (n != 0)
	{
		r = n % 16;
		ans += hexHelper(r);
		n /= 16;
	}
	reverse(ans.begin(), ans.end());
	return ans;
}

void CryptAlg::convertToUc(BigInt n, uc* charArr, uint buffSize)
{
	vector<int> ans;
	BigInt r{};
	while (n != 0)
	{
		r = n % 256;
		ans.push_back(r.ToInt());
		n /= 256;
	}
	reverse(ans.begin(), ans.end());
	for (uint i = 0; i < buffSize; i++) {
		if (i >= ans.size())
			charArr[i] = 0;
		else
			charArr[i] = ans[i];
	}
}

BigInt CryptAlg::convertToBigInt(uc* charArr, uint buffSize)
{
	BigInt p{ 1 };
	BigInt res{ 0 };
	for (int i = buffSize - 1; i > -1; i--) {
		res += charArr[i] * p;
		p *= 256;
	}
	return res;
}

byte* CryptAlg::toByteArr(string str)
{
	vector<unsigned char> bytes(str.begin(), str.end());
	bytes.push_back('\0');
	byte* c = &bytes[0];
	return c;
}

string CryptAlg::byteToString(const byte* bts, uint len)
{
	string s(reinterpret_cast<char const*>(bts), len);
	return s;
}

string CryptAlg::aesEncrypt(string plain, string key_s)
{
	byte key[AES::DEFAULT_KEYLENGTH];
	for (int i = 0; i < key_s.size(); i++)
		key[i] = (byte)key_s[i];

	string cipher;
	try
	{
		ECB_Mode< AES >::Encryption e;
		e.SetKey(key, sizeof(key));

		StringSource s(plain, true,
			new StreamTransformationFilter(e,
				new StringSink(cipher)
			) // StreamTransformationFilter
		); // StringSource
	}
	catch (const Exception& e)
	{
		cerr << e.what() << "\n";
		exit(1);
	}
	return cipher;
}

string CryptAlg::aesDecrypt(string cipher, string key_s)
{
	byte key[AES::DEFAULT_KEYLENGTH];
	for (int i = 0; i < key_s.size(); i++)
		key[i] = (byte)key_s[i];
	string recovered;
	try
	{
		ECB_Mode< AES >::Decryption d;
		d.SetKey(key, sizeof(key));

		StringSource s(cipher, true,
			new CryptoPP::HexDecoder(
				new StreamTransformationFilter(d,
					new StringSink(recovered)
				) // StreamTransformationFilter
			) // HexDecoder
		); // StringSource

		cout << "recovered text: " << recovered << "\n";
	}
	catch (const Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	return recovered;
}

string CryptAlg::encrypt_buffer(string input, unsigned char key[AES::DEFAULT_KEYLENGTH], unsigned char iv[AES::DEFAULT_KEYLENGTH])
{
	AES::Encryption aesEncryption(key, AES::DEFAULT_KEYLENGTH);
	CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	string encrypted; encrypted.reserve(input.size() + 16);
	StreamTransformationFilter stfEncryptor(cbcEncryption, new StringSink(encrypted));
	stfEncryptor.Put(reinterpret_cast<const unsigned char*>(input.c_str()), input.length());
	stfEncryptor.MessageEnd();

	string encoded = base64_encode(encrypted, false);
	return encoded;
}

string CryptAlg::decrypt_buffer(string input, unsigned char key[AES::DEFAULT_KEYLENGTH], unsigned char iv[AES::DEFAULT_KEYLENGTH])
{
	string decoded = base64_decode(input, false);
	string decrypted; decrypted.reserve(decoded.size());

	AES::Decryption aesDecryption(key, AES::DEFAULT_KEYLENGTH);
	CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

	StreamTransformationFilter stfDecryptor(cbcDecryption, new StringSink(decrypted));
	stfDecryptor.Put(reinterpret_cast<const unsigned char*>(decoded.c_str()), decoded.size());
	stfDecryptor.MessageEnd();

	return decrypted;
}

string CryptAlg::getDigest(string msg)
{
	HexEncoder encoder(new FileSink(cout));
	string digest;
	SHA3_256 hash;
	StringSource(msg, true, new HashFilter(hash, new StringSink(digest)));

	/*cout << "Message: " << msg << "\n";
	cout << "Digest: ";
	StringSource(digest, true, new Redirector(encoder));
	cout << "\n";*/
	return digest;
}

bool CryptAlg::verifyHash(string msg, string digest) {
	bool result;
	SHA3_256 hash;
	StringSource(digest + msg, true, new HashVerificationFilter(hash,
		new ArraySink((byte*)&result, sizeof(result))));
	return result;
}