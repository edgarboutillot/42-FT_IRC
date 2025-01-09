//
// Created by hdupuy on 5/21/24.
//

#include "client.hpp"
#include "server.hpp"

void Server::modeI ( Channel* channel, char option, Client* client )
{
	if ( !channel->isOperator ( client ) )
	{
		queueDataForSend ( ERR_CHANOPRIVSNEEDED ( client->getNickname (), channel->getName () ), client->getFd () );
		return;
	}
	if ( option == '+' )
	{
		if ( channel->alreadySet ( "i" ) )
		{
			std::string msg = "MODE " + channel->getName () + " +i" + " :Channel mode is already set" + CRLF;
			queueDataForSend ( msg, client->getFd () );
			return;
		}
		channel->setOnInvitation ( true );
		std::string modeArgs	  = channel->addArgToModeVector ( "i" );
		std::string channelModeIs = RPL_CHANNELMODEIS ( client->getNickname (), channel->getName (), modeArgs );
		queueDataForSend ( channelModeIs, client->getFd () );
		sendChannel ( client, channel, channelModeIs );
		modeChangeMsgforAll ( client, channel->getName (), "", client->getFd (), channel, "+i" );
		return;
	}
	else if ( option == '-' )
	{
		if ( !channel->alreadySet ( "i" ) )
		{
			std::string msg = "MODE " + channel->getName () + " +i" + " :Channel mode is not set" + CRLF;
			queueDataForSend ( msg, client->getFd () );
			return;
		}
		channel->setOnInvitation ( false );
		std::string modeArgs	  = channel->removeArgToModeVector ( "i" );
		std::string channelModeIs = RPL_CHANNELMODEIS ( client->getNickname (), channel->getName (), modeArgs );
		sendChannel ( client, channel, channelModeIs );
		queueDataForSend ( RPL_CHANNELMODEIS ( client->getNickname (), channel->getName (), modeArgs ),
						   client->getFd () );
		modeChangeMsgforAll ( client, channel->getName (), "", client->getFd (), channel, "-i" );
		return;
	}
}
