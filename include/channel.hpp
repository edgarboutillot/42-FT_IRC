//
// Created by dupuy on 02/04/24.
//

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "client.hpp"
#include "include.hpp"
#include "server.hpp"
#include <ctime>

typedef std::map<Client*, time_t> OperTime;

class Channel
{
	private:

		bool _isByKey;
		bool _isOnInvitation;
		bool _isTopic;
		bool _isTopicProtected;
		bool _limit;
		int _limitValue;
		std::map<std::string, Client*> _MapClient;
		std::map<Client*, time_t> _ClientTime;
		std::string _name;
		std::string _password;
		std::string _topic;
		OperTime _ChannelOperator;
		std::vector<Client*> _inviteList;
		std::vector<std::string> _modeArgs;

		std::vector<char> _multiModeVector;
		std::vector<std::string> _ArgsForModeVector;

		std::vector<std::pair<char, std::string> > _multiModeContainer;
		std::vector<std::pair<std::string, std::string> > _multiModeContainer2;


	public:

		Channel(const std::string& name);
		~Channel();

		// METHODS
		bool isGoodKey(std::string& key);
		bool isInChanByFd(int client_fd);
		bool isInChannel(std::string nickname);
		void addClient(const std::string& clientName, Client* client);
		void setChannelOperator(Client* client);

		// GETTERS
		bool isLimited() const;
		bool isOnInvitation() const;
		bool isProtected();
		bool isTopic();
		bool isTopicProtected();
		const std::string& getName() const;
		int getClientLimit();
		int getNumberOfClients();
		std::map<std::string, Client*>& getMapClient();
		std::string getAllNames();
		std::string getModeArgs();
		std::string getTopic();
		OperTime& getOperator();
		void setOlderClientAsOperator();
		std::vector<std::pair<std::string, std::string> >& getMultiModeContainer2();

		// SETTERS
		bool alreadySet(std::string mode);
		std::string addArgToModeVector(std::string argToAdd);
		std::string removeArgToModeVector(std::string argToRemove);
		void removeClient(Client* client);
		void removeInviteList(std::string nickname);
		void removeOperator(Client* client);
		void setLimit(std::string value, bool isLimit);
		void setOnInvitation(bool isOnInvitation);
		void setPassword(const std::string& password, bool isByKey);
		void setTopic(const std::string& basicString, bool isTopic);
		void setTopicProtected(bool b);
		void setClientTime(Client* client);
		void removeClientTime(Client* pClient);
		bool isOperator(Client* client);
		void addClientToInviteList(Client* client);
		bool isInInviteList(Client* client);
};

#endif // CHANNEL_HPP
