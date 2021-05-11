#pragma once
#include "Member.h"
#include <vector>

using namespace std;

class ChatRoom
{
public:
	BigInt dum{};
	//const BigInt p_keyGen{ (dum.pow(2, 128) - 3) / 76439 };
	const BigInt p_keyGen{ 10000000019 };
	const BigInt g = 17;
	ChatRoom();
	~ChatRoom();
	void inputMessages();
	void outputMessages();
	vector<Message> getAllMessages();
	vector<Message> getMessages(uint tag);
	vector<Message> getMessages(vector<uint> tags);
	vector<string> getMessagesAndKeys(string name);
	string getMessagesAndKeys(string name, int dum);
	bool addMessage(json message);
	bool initPrivateKeyGen();
	bool addMember(Member newMem);
	bool removeMember(string name);
	const uint getCurrentTag() { return currentTag; };
	Member getMemberByName(string name);
private:
	vector<Member> members;
	vector<Message> messages;
	uint currentTag;
	string messagesFilename = "messages.json";
	string currentTagFilename = "current_tag.json";

	CryptAlg cra{};
	jsonTranslator jt;
	fieldParams fp;
};