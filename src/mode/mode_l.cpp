//
// Created by dupuy on 15/05/24.
//

#include "client.hpp"
#include "server.hpp"

void Server::modeL(int client_fd, std::string channelName, std::string& mode, std::string& param)
{
	Client* client	 = getClientByFd(client_fd);
	Channel* channel = getChannelByName(channelName);

	if ( channel->isOperator(client) )
	{
		if ( mode == "+l" )
		{
			if ( param.empty() )
			{
				queueDataForSend(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE"), client_fd);
				return;
			}
			if ( channel->alreadySet("l") )
			{
				std::string msg = "MODE " + channel->getName() + " +l" + " :Channel mode is already set" + CRLF;
				queueDataForSend(msg, client->getFd());
				return;
			}
			if ( !isNumber(param) )
			{
				std::string msg = "MODE " + channel->getName() + " +l" + " :The string is not a number" + CRLF;
				queueDataForSend(msg, client->getFd());
				return;
			}
			channel->setLimit(param, true);
			channel->addArgToModeVector("l");
//			queueDataForSend(RPL_CHANNELMODEIS(client->getNickname(), channelName, channel->getModeArgs()), client_fd);
			sendChannel(client, channel, RPL_CHANNELMODEIS(client->getNickname(), channelName, channel->getModeArgs()));
			modeChangeMsgforAll (client, channel->getName(), param, client_fd, channel, "+l");
			return;
		}
		else if ( mode == "-l" )
		{
			if ( !channel->alreadySet("l") )
			{
				std::string msg = "MODE " + channel->getName() + " +l" + " :Channel mode is not set" + CRLF;
				queueDataForSend(msg, client->getFd());
				return;
			}
			channel->setLimit("0", false);
			channel->removeArgToModeVector("l");
			queueDataForSend(RPL_CHANNELMODEIS(client->getNickname(), channelName, channel->getModeArgs()), client_fd);
			sendChannel(client, channel, RPL_CHANNELMODEIS(client->getNickname(), channelName, channel->getModeArgs()));
			modeChangeMsgforAll (client, channel->getName(), param, client_fd, channel, "-l");
			return;
		}
		else
		{
			queueDataForSend(ERR_UMODEUNKNOWNFLAG(client->getNickname()), client_fd);
			return;
		}
	}
	else
	{
		queueDataForSend(ERR_CHANOPRIVSNEEDED(client->getNickname(), channelName), client_fd);
		return;
	}
}
