#pragma once
#include "Member.h"
#include <vector>



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
	std::vector<Message> getAllMessages();
	std::string getAllMessages(int num);
	std::vector<Message> getMessages(uint tag);
	std::vector<Message> getMessages(std::vector<uint> tags);
	std::vector<std::string> getMessagesAndKeys(std::string name);
	std::string getMessagesAndKeys(std::string name, int dum);
	bool addMessage(json message);
	bool initPrivateKeyGen();
	bool addMember(Member newMem);
	bool removeMember(std::string name);
	bool removeMember(uint idx);
	const uint getCurrentTag() { return currentTag; };
	Member getMemberByName(std::string name);
	std::string getMemberNameByIdx(uint idx);
	uint getMemberCount() { return members.size(); };

private:
	std::vector<Member> members;
	std::vector<Message> messages;
	uint currentTag;
	std::string messagesFilename = "messages.json";
	std::string currentTagFilename = "current_tag.json";

	CryptAlg cra{};
	jsonTranslator jt;
	fieldParams fp;
};