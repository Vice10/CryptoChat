#define _CRT_SECURE_NO_WARNINGS
#include "Member.h"

Member::Member(string name)
{
	if (name.empty()) throw invalid_argument("Member(): new member name is empty\n");
	uniqueName = name;
}

BigInt Member::genPublicKey()
{
	//auto pBitRep = cra.convertToBase(p, 2);
	a = cra.genPrime(bitLen);
	return cra.binpower(fp.g_key, a, fp.p_key);
}

BigInt Member::genSecretKey(BigInt publicKey, uint tag)
{
	BigInt secretKey = cra.binpower(publicKey, a, fp.p_key);
	secretKeys.insert(make_pair(tag, secretKey.ToString())); // decimal
	//return cra.binpower(secretKey, checkSecretKey, fp.p_mes);
	return 1;
}

json Member::compoundSecretMessage(string plain, uint currentTag)
{
	auto it = secretKeys.find(currentTag);
	if (it == secretKeys.end())
		return nullptr;
	unsigned char iv[AES::BLOCKSIZE];
	AutoSeededRandomPool prng;
	prng.GenerateBlock(iv, AES::BLOCKSIZE);

	BigInt key{ secretKeys[currentTag] };
	uc key_uc[AES::DEFAULT_KEYLENGTH];
	cra.convertToUc(key, key_uc, AES::DEFAULT_KEYLENGTH);
	string encrypted = cra.encrypt_buffer(plain, key_uc, iv);

	string digest = base64_encode(cra.getDigest(plain), false);
	string curTime = getCurrentTime();
	json outMes;
	BigInt iv_num = cra.convertToBigInt(iv, AES::BLOCKSIZE);
	outMes["str"] = encrypted;
	outMes["sender"] = uniqueName;
	outMes["tag"] = currentTag;
	outMes["date"] = curTime;
	outMes["sign"] = digest;
	outMes["iv"] = cra.convertToBigInt(iv, AES::BLOCKSIZE).ToString();
	return outMes;
}

string Member::decryptSecretMessage(Message msg, bool& verified)
{
	auto it = secretKeys.find(msg.tag);
	if (it == secretKeys.end())
		return nullptr;
	BigInt key{ secretKeys[msg.tag] };
	uc key_uc[AES::DEFAULT_KEYLENGTH];
	cra.convertToUc(key, key_uc, AES::DEFAULT_KEYLENGTH);

	unsigned char iv[AES::BLOCKSIZE];
	cra.convertToUc(BigInt{ msg.iv }, iv, AES::BLOCKSIZE);
	string recovered = cra.decrypt_buffer(msg.str, key_uc, iv);
	verified = cra.verifyHash(recovered, base64_decode(msg.sign, false));

	return recovered;
}

string Member::getCurrentTime()
{
	time_t rawtime;
	struct tm* timeInfo;

	time(&rawtime);

	timeInfo = localtime(&rawtime);
	string time = getCurTimeHelper(timeInfo);
	return time;
}

bool Member::addSecretKey(json secretKey) {
	Message msg{};
	jt.from_json(secretKey, msg);
	secretKeys.insert(make_pair(msg.tag, msg.str));
	return true;
}

bool Member::setUniqueName(string name)
{
	if (name.empty()) return false;
	uniqueName = name;
	return true;
}

bool Member::setSecretKeys(map<uint, string> newSecretKeys)
{
	if (newSecretKeys.size() == 0) return false;
	secretKeys = newSecretKeys;
	return true;
}

char* Member::getCurTimeHelper(const tm* timeptr)
{
	static const char wday_name[][4] = {
   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	static const char mon_name[][4] = {
	  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	static char result[26];
	sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d",
		wday_name[timeptr->tm_wday],
		mon_name[timeptr->tm_mon],
		timeptr->tm_mday, timeptr->tm_hour,
		timeptr->tm_min, timeptr->tm_sec,
		1900 + timeptr->tm_year);
	return result;
}

vector<Message> Member::transformMes(char* messages) { /// messages[0] == '0'
	string allMes = messages;
	vector<Message> res;
	json jMessages = allMes;
	Message curMes{};
	for (const auto& mes : jMessages) {
		jt.from_json(mes, curMes);
		res.push_back(curMes);
	}
	return res;
}

uint Member::getLatestTag(){
	if (secretKeys.size() == 0) return 0;
	map<uint, string>::iterator itr;
	itr = secretKeys.end();
	itr--;
	return itr->first;
}

BigInt fieldParams::getHash(BigInt key)
{
	return key % M;
}

BigInt fieldParams::getSmallHash(BigInt key)
{
	return key % M2;
}
