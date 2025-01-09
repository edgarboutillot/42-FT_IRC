#include "include.hpp"
#include "server.hpp"

void Server::sendChannel(Client* sender, Channel* channel, const std::string& msg)
{
	if ( !sender || !channel )
		return;

	std::map<std::string, Client*> clientOfChan = channel->getMapClient();

	for ( std::map<std::string, Client*>::iterator it = clientOfChan.begin(); it != clientOfChan.end(); ++it )
	{
		if ( it->second )
		{
			if ( it->second->getNickname() != sender->getNickname() )
			{
				int recipient_fd = it->second->getFd();
				send(recipient_fd, msg.c_str(), msg.length(), 0);
			}
		}
		else
		{
			return;
		}
	}
}

// void Server::sendChannel(Client* sender, Channel* channel, const std::string& msg)
// {
// 	// (void) msg;
// 	if ( !sender || !channel )
// 	{
// 		return;
// 	}
// 	// std::cout << "msg ='" << msg << "'" << std::endl;

// 	std::map<std::string, Client*> &clientOfChan = channel->getMapClient();

// 	for ( std::map<std::string, Client*>::iterator it = clientOfChan.begin(); it != clientOfChan.end(); ++it )
// 	{
// 		int recipient_fd = it->second->getFd();
// 		send(recipient_fd, msg.c_str(), msg.length(), 0);
		// queueDataForSend(RPL_NAMREPLY(channel->getName(), it->first, channel->getAllNames()), recipient_fd);
		// queueDataForSend(RPL_ENDOFNAMES(it->second->getNickname(), channel->getName()), recipient_fd);
// 		// if ( it->second )
// 		// {
// 		// 	if ( it->second->getNickname() != sender->getNickname() )
// 		// 	{
// 				// int recipient_fd = it->second->getFd();
// 				// send(recipient_fd, msg.c_str(), msg.length(), 0);
// 		// 	}
// 		// }
// 		// else
// 		// {
// 		// 	return;
// 		// }
// 	}
// }

void Server::PRIVMSG(std::string& cmd, int client_fd)
{
	Client* client						 = getClientByFd(client_fd);
	std::vector<std::string> splited_cmd = split_cmd(cmd);
	if ( !client->isRegistered() || !client->isLogged() )
	{
		queueDataForSend(ERR_NOTREGISTERED(std::string("*")), client_fd);
		return;
	}
	if ( splited_cmd.size() < 3 )
	{
		if ( splited_cmd.size() == 2 )
		{
			queueDataForSend(ERR_NOTEXTTOSEND(client->getUsername(), splited_cmd[0]), client_fd);
			return;
		}
		if ( splited_cmd.size() == 1 )
		{
			queueDataForSend(ERR_NORECIPIENT(client->getUsername()), client_fd);
			return;
		}
	}
	else
	{
		std::string msg;
		for ( size_t i = 2; i < splited_cmd.size(); ++i )
		{
			msg += splited_cmd[i];
			if ( i != splited_cmd.size() - 1 )
				msg += " ";
		}
		clientPrivateMsg(client_fd, splited_cmd[1], msg);
	}
}

void Server::clientPrivateMsg(int client_fd, const std::string& recipient, const std::string& msg)
{
	Client* sender = getClientByFd(client_fd);
	if ( recipient[0] == '#' )
	{
		Channel* channel = getChannelByName(recipient);
		if ( channel )
		{
			std::string channelMsg = ":" + sender->getNickname() + " PRIVMSG " + recipient + " " + msg + CRLF;
			sendChannel(sender, channel, channelMsg);
		}
		else
		{
			queueDataForSend(ERR_NOSUCHCHANNEL(sender->getNickname(), recipient), client_fd);
			return;
		}
	}
	else
	{
		std::map<int, Client*>::iterator it;
		for ( it = _clients.begin(); it != _clients.end(); ++it )
		{
			Client* client = it->second;
			if ( client->getNickname() == recipient )
			{
				std::string response;
				response = ":" + sender->getNickname() + " PRIVMSG " + recipient + " " + msg + CRLF;
				send(client->getFd(), response.c_str(), response.length(), 0);
				return;
			}
		}
		queueDataForSend(ERR_NOSUCHNICK(sender->getNickname()), client_fd);
	}
}
