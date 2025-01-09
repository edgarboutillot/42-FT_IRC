//
// Created by hdupuy on 5/21/24.
//

#include "client.hpp"
#include "server.hpp"

std::string convertTime(std::time_t time)
{
	std::ostringstream oss;
	oss << time;
	std::string numStr = oss.str();

	return numStr;
}

void Server::TOPIC(std::string& cmd, int client_fd)
{
	Client* client = getClientByFd(client_fd);
	if ( !client->isRegistered() || !client->isLogged() )
	{
		queueDataForSend(ERR_NOTREGISTERED(std::string("*")), client_fd);
		return;
	}
	std::vector<std::string> splited_cmd = split_cmd(cmd);

	if ( splited_cmd.size() < 2 )
	{
		queueDataForSend(ERR_NEEDMOREPARAMS(client->getNickname(), "TOPIC"), client_fd);
		return;
	}

	std::string channelName = splited_cmd[1];
	Channel* channel		= getChannelByName(channelName);
	std::string topic		= cmd.substr(cmd.find_first_of(':') + 1);

	if ( !channel )
	{
		queueDataForSend(ERR_NOSUCHCHANNEL(client->getNickname(), channelName), client_fd);
		return;
	}
	if ( !channel->isInChannel(client->getNickname()) )
	{
		queueDataForSend(ERR_NOTONCHANNEL(client->getNickname(), channelName), client_fd);
		return;
	}
	if ( channel->isTopicProtected() )
		if ( channel->isOperator(client) )
		{
			channel->setTopic(topic, true);
			queueDataForSend(RPL_TOPIC(client->getNickname(), channel->getName(), channel->getTopic()), client_fd);
			std::string topicMsg = ":" + client->getNickname() + " TOPIC " + channelName + " :" + topic + CRLF;
			sendChannel(client, channel, topicMsg);
			std::string time = convertTime(std::time(0));
			queueDataForSend(RPL_TOPICWHOTIME(client->getNickname(), channel->getName(), time), client_fd);
			sendChannel(client, channel, RPL_TOPICWHOTIME(client->getNickname(), channel->getName(), time));
		}
		else
		{
			queueDataForSend(RPL_TOPIC(client->getNickname(), channel->getName(), channel->getTopic()), client_fd);
			queueDataForSend(ERR_CHANOPRIVSNEEDED(client->getNickname(), channel->getName()), client_fd);
		}
	else
	{
		channel->setTopic(topic, true);
		std::string time = convertTime(std::time(0));
		queueDataForSend(RPL_TOPIC(client->getNickname(), channel->getName(), channel->getTopic()), client_fd);
		std::string topicMsg = ":" + client->getNickname() + " TOPIC " + channelName + " :" + topic + CRLF;
		sendChannel(client, channel, topicMsg);
		queueDataForSend(RPL_TOPICWHOTIME(client->getNickname(), channel->getName(), time), client_fd);
		sendChannel(client, channel, RPL_TOPICWHOTIME(client->getNickname(), channel->getName(), time));
	}
}

std::map<std::string, Channel*>& Server::getChannels()
{
	return _MapChannels;
}
