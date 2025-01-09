//
// Created by dupuy on 29/03/24.
//

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "channel.hpp"
#include "include.hpp"
#include "server.hpp"

class Channel;

class Client
{
	private:


		bool _logged;
		bool _operator;
		bool _registered;
		int _fd;
		std::map<std::string, Channel*> _MapChannels;
		std::string _nickname;
		std::string _realname;
		std::string _username;
		struct epoll_event _clientEpollevent;

	public:

		std::string buffer;
		std::string sendBuffer;
		// CONSTRUCTORS
		Client();
		Client(int fd);
		~Client();

		// GETTERS
		bool isLogged() const;
		bool isOperator() const;
		bool isRegistered() const;
		const std::string& getNickname() const;
		const std::string& getRealname() const;
		const std::string& getUsername() const;
		int getFd() const;
		std::map<std::string, Channel*>& getChannels();

		// SETTERS
		void setLogged(bool logged);
		void setNickname(const std::string& nickname);
		void setOperator(bool isOperator);
		void setRealname(const std::string& realname);
		void setRegistered(bool registered);
		void setUsername(const std::string& username);

		// METHODS
		void updateClientStatus(const int& epollFd, uint32_t events);
		void removeChannelClient(const std::string & channelName);
		int& getFd();
};

#endif // CLIENT_HPP
