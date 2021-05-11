#include <iostream>
#include <fstream>
#include "ChatRoom.h"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;
typedef unsigned int uint;
int main()
{
	srand(time(NULL));
	Member m1{"Alice"}, m2{"Bob"};
	ChatRoom cr{};
	cr.addMember(m1);
	cr.addMember(m2);
	json secretMessage = cr.getMemberByName("Alice").compoundSecretMessage("Hello!It's Alice again", cr.getCurrentTag());
	bool flag = cr.addMessage(secretMessage);
	jsonTranslator jt;
	Message msg{};
	jt.from_json(secretMessage, msg);
	string recovered = cr.getMemberByName("Bob").decryptSecretMessage(msg, flag);
	cout << recovered << "\n";
	cout << flag;

	auto Amessages = cr.getMessagesAndKeys("Alice");
	for (const auto& mes : Amessages) {
		cout << mes << "\n\n";
	}
	cout << "\n\n";
	cout << cr.getMessagesAndKeys("Alice", 1);
	cout << "\n" << cr.removeMember("Alice");


	/*CryptAlg cra{};
	string digest = cra.getDigest("Hello!!");
	cout << base64_encode(digest, false) << "\n\n";
	cout << cra.verifyHash("Hello!!", digest);*/

	/*CryptAlg cra{};
	AutoSeededRandomPool prng;
	unsigned char key[AES::DEFAULT_KEYLENGTH];
	unsigned char iv[AES::BLOCKSIZE];
	prng.GenerateBlock(key, AES::DEFAULT_KEYLENGTH);
	prng.GenerateBlock(iv, AES::BLOCKSIZE);
	BigInt k = cra.convertToBigInt(key, AES::DEFAULT_KEYLENGTH);
	cout << "char to BigInt :" << k << "\n\n";
	cra.convertToUc(k, key, AES::DEFAULT_KEYLENGTH);


	string key_base64 = base64_encode(key, AES::DEFAULT_KEYLENGTH, false);
	string key_base64_decode = base64_decode(key_base64, false);
	byte* key_byte = cra.toByteArr(key_base64_decode);

	string plain = "Hello!wewe!!dghdfhgdrhgedhtrethetredh";
	string cipher;
	string recovered;
	cipher = cra.encrypt_buffer(plain, key, iv);
	cout << "Encr: " << cipher << "\n\n";
	recovered = cra.decrypt_buffer(cipher, key, iv);
	cout << "Decr: " << recovered << "\n\n";*/
	return 0;
}