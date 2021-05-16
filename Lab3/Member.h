#pragma once
#include <string>
#include <chrono>
#include <nlohmann/json.hpp>
#include <fstream>
#include "CryptAlg.h"
#include <iomanip>
#include <time.h>
#include "base64.h"

using namespace std;
typedef unsigned int uint;
using json = nlohmann::json;


struct fieldParams { /// For message encryption/decryption
	BigInt dum1{};
	const uint KEYLEN = AES::DEFAULT_KEYLENGTH;
	const uint BLOCKSIZE = AES::BLOCKSIZE;
	const BigInt g_key{ 5 }; /// generator for public/private key exchange
	const BigInt p_mes{ dum1.pow(2, 192) - dum1.pow(2, 32) - dum1.pow(2, 12) - dum1.pow(2, 8)
			 - dum1.pow(2, 7) - dum1.pow(2, 6) - dum1.pow(2, 3) - 1 };    /// modulo for messages
	//const BigInt p_key{dum1.pow(2, 128) - dum1.pow(2, 97) - 1};  /// modulo for secret key
	const BigInt p_key{ "10000000019" };  /// modulo for secret key
	const BigInt M{ "1000000019" }; /// modulo for big messages
	const BigInt M2{ "10007" };     /// modulo for small messages
	BigInt getHash(BigInt key);     /// hash func for big messages
	BigInt getSmallHash(BigInt key);/// hash func for small messages
};
struct Message
{
	string str;    // encrypted message
	string sender; // sender name
	int tag;	   // tag to identify secret key
	string date;
	string sign;   // message signature
	string iv;     // used in AES
};

struct jsonTranslator {
	void to_json(json& j, const Message& m) {
		j = json{ {"str", m.str}, {"sender", m.sender}, {"tag", m.tag},
			{"date", m.date}, {"sign", m.sign}, {"iv", m.iv} };
	}

	void from_json(const json& j, Message& m) {
		j.at("str").get_to(m.str);
		j.at("sender").get_to(m.sender);
		j.at("tag").get_to(m.tag);
		j.at("date").get_to(m.date);
		j.at("sign").get_to(m.sign);
		j.at("iv").get_to(m.iv);
	}
};

class Member
{
public:
	Member(string name);
	string getUniqueName() { return uniqueName; };
	BigInt genPublicKey();		    /// g and p are specified by the chat room
	BigInt genSecretKey(BigInt publicKey, uint tag);    /// return secretKey^check (mod p_mes)
	json compoundSecretMessage(string text, uint tag);  /// the current tag is provided by the chat room
	string decryptSecretMessage(Message msg, bool& verified);   /// the current tag is given the encrypted json
	string getCurrentTime();
	bool addSecretKey(json secretKey); // extracts tag and str only
	const map<uint, string> getSecretKeys() { return secretKeys; };
	bool setUniqueName(string name);
	bool setSecretKeys(map<uint, string> newSecretKeys);
	vector<Message> transformMes(char* messages);
	uint getLatestTag();
	uint getSecretKeysCount() { return secretKeys.size(); };
	void setInactive() { isActive = false; };
	bool membIsActive() { return isActive; };

private:
	string uniqueName;
	map<uint, string> secretKeys;
	bool isActive = true;
	BigInt a{}; // the secret number to generate the public key
	const uint bitLen = 20;
	CryptAlg cra{};
	char* getCurTimeHelper(const struct tm* timeptr);
	fieldParams fp;
	jsonTranslator jt;
	BigInt checkSecretKey{ "197346825" };
};
