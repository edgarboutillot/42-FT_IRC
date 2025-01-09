/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbacquet <cbacquet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 15:17:15 by cbacquet          #+#    #+#             */
/*   Updated: 2024/06/04 18:24:15 by cbacquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "channel.hpp"
#include "client.hpp"
#include "include.hpp"

#define MAX_EVENT 5

// void	SignalHandler(int signum);

extern bool Signal;

class Client;
class Channel;

typedef std::map<std::string, int> nickSet;
typedef std::map<int, Client*> MapClient;

class Server
{
	private:

		nickSet _nickUsed;
		std::string _password;
		std::string _passOperator;
		u_int64_t _port;
		int _socket;
		int _epollFd;
		MapClient _clients;
		std::map<int, std::string> _partialCommand;
		std::map<std::string, Channel*> _MapChannels;

	public:

		Server();
		Server(u_int64_t port, const std::string& password);
		~Server();

		// SETTERS
		const nickSet& getNickUsed() const;
		void setNickUsed(const nickSet& nickUsed);
		const std::string& getPassword() const;
		void setPassword(const std::string& password);

		// GETTERS
		u_int64_t getPort() const;
		void setPort(u_int64_t port);
		int getSocket() const;
		void setSocket(int socket);
		int getEpollFd() const;
		void setEpollFd(int epollFd);

		// METHODS
		void serverInit();
		void setServerSocket();
		void receiveNewData(int client_fd);
		void sendBufferedData(int fd);
		bool isBufferEmpty(int fd);
		void removeEpollOutFlag(int client_fd);
		void queueDataForSend(const std::string& data, int client_fd);
		static void reply(std::string response, int fd);
		Client* getClientByFd(int fd);

		// METHOD CHANNEL
		void addChannel(const std::string& channelName);
		void removeChannel(const std::string& channelName);
		Channel* getChannelByName(const std::string& channelName);
		void addUserToChannel(std::string channelName, Client* client);
		void kickUserFromChannel(Channel* channel, Client* client, const std::string& userToKick,
								 const std::string& kickMsg);

		// PARSING
		void parseCommand(std::string& cmd, int client_fd);

		// COMMANDS
		void clientAuthentification(int client_fd, std::string& cmd);
		void clientSetNickname(int client_fd, std::string& cmd);
		void clientSetUsername(int client_fd, std::string& username, std::string& cmd);
		void clientJoinChannel(int client_fd, std::string& cmd);
		void clientJoin0(int client_fd);
		void clientPrivateMsg(int client_fd, const std::string& recipient, const std::string& message);

		// INVITE
		void invite(Client* client, std::string channelName, std::string userToInvite);

		void clientDisconnect(int client_fd);
		void requestConnexion(int clientFd);
		void addClient(int key, Client* clientToAdd);

		void modeL(int client_fd, std::string channelName, std::string& mode, std::string& param);

		void PASS(std::string& cmd, int client_fd);
		void NICK(std::string& cmd, int client_fd);
		void USER(std::string& cmd, int client_fd);
		void JOIN(std::string& cmd, int client_fd);
		void PRIVMSG(std::string& cmd, int client_fd);
		void KICK(std::string& cmd, int client_fd);
		void TOPIC(std::string& cmd, int client_fd);
		void clientKick(int client_fd, std::string userToKick, std::string channelName,
						std::vector<std::string> splited_cmd);
		void joinExistingChannel(Channel* channel, Client* client, int client_fd);
		void PART(std::string cmd, int client_fd);
		void clientPart(int client_fd, const std::string& cmd);
		void extractChannel(std::string& channel_name, std::vector<std::string>& channel_names);

		void modeI(Channel* channel, char option, Client* client);
		void createAndJoinNewChannel(const std::string& channelName, Client* client, int client_fd);
		std::vector<std::string> split_cmd(const std::string& str);
		void MODE(std::vector<std::string> splited_cmd, Client* client, int client_fd);
		void INVITE(std::vector<std::string> splited_cmd, int client_fd);
		void QUIT(std::vector<std::string> splited_cmd, std::string& cmd, int client_fd);
		void hashtagChecker(std::vector<std::string>& channels, std::vector<std::string>& keys, Client* client,
							int client_fd);

		std::map<std::string, Channel*>& getChannels();
		void compareChannelName(std::vector<std::string>& channelsToCheck);

		bool isSameString(const std::string& str1, const std::string& str2);
		std::string toLowerCase(const std::string& input);
		bool isInNicks(std::string& nickToCheck);
		bool compareNickName(std::string& nickToCheck, int client_fd);

		void sendNickToAll(Client* client, const std::string& oldNickname);
		void sendChannel(Client* sender, Channel* channel, const std::string& msg);
		void changeOperNickInChan(std::string& newOp, std::string& oldOp, int client_fd);

		// multimode
		void multimode(Channel* channel, Client* client, std::string multimode);

		void fillMultiModeVector2(std::vector<std::pair<std::string, std::string> >& multiModeContainer2,
								  std::string& multimode, Client* client);
		void extractMultiMode(std::string splited_cmd, Client* client, Channel* channel);
		void extractArgMultiMode(std::vector<std::string>& splited_cmd, size_t startIndex, Client* client,
								 Channel* channel);
		void modeT(char option, std::vector<std::string> splited_cmd, Client* client, int client_fd, Channel* channel);
		void modeK(char option, std::vector<std::string>& splited_cmd, Client* client, int client_fd, Channel* channel);
		void modeO(char option, std::string target, std::vector<std::string> splited_cmd, Channel* channel,
				   Client* client, int client_fd);
		bool isNumber(std::string& basicString);
		std::string trim(const std::string& str);
		void modeChangeMsgforAll ( Client* client, std::string channelName, std::string modeArgs, int client_fd,
								   Channel* channel, std::string mode );
};

#endif