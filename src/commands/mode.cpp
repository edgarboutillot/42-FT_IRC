//
// Created by dupuy on 22/05/24.
//

#include "client.hpp"
#include "server.hpp"

void Server::MODE ( std::vector< std::string > splited_cmd, Client* client, int client_fd )
{
	if ( !client->isRegistered () || !client->isLogged () )
	{
		queueDataForSend ( ERR_NOTREGISTERED ( std::string ( "*" ) ), client_fd );
		return;
	}
	if ( !client->isLogged () )
	{
		std::cout << "Error: you need to be logged on the server" << std::endl;
		return;
	}
	if ( splited_cmd.size () < 3 )
	{
		if ( splited_cmd.size () == 1 )
		{
			queueDataForSend ( ERR_NEEDMOREPARAMS ( client->getNickname (), "MODE" ), client->getFd () );
			return;
		}
		std::string channelName = splited_cmd[1];
		Channel* channel		= getChannelByName ( channelName );
		if ( splited_cmd.size () == 2 )
		{
			if ( channel )
			{
				queueDataForSend ( RPL_CHANNELMODEIS ( client->getNickname (), channelName, channel->getModeArgs () ),
								   client_fd );
				return;
			}
		}
	}
	std::string channelName = splited_cmd[1];
	Channel* channel		= getChannelByName ( channelName );

	if ( channel )
	{
		extractMultiMode ( splited_cmd[2], client, channel );
		if ( splited_cmd.size () > 3 )
			extractArgMultiMode ( splited_cmd, 3, client, channel );

		std::vector< std::pair< std::string, std::string > >& modes = channel->getMultiModeContainer2 ();

		for ( std::vector< std::pair< std::string, std::string > >::iterator it = modes.begin (); it != modes.end ();
			  ++it )
		{
			if ( it->first[1] == 'i' )
				modeI ( channel, it->first[0], client );
			else if ( it->first[1] == 't' )
				modeT ( it->first[0], splited_cmd, client, client_fd, channel );
			else if ( it->first[1] == 'k' )
				modeK ( it->first[0], splited_cmd, client, client_fd, channel );
			else if ( it->first[1] == 'o' )
				modeO ( it->first[0], it->second, splited_cmd, channel, client, client_fd );
			else if ( it->first[1] == 'l' )
				modeL ( client_fd, channel->getName (), it->first, it->second );
		}
		modes.clear ();
	}
	else
	{
		queueDataForSend ( ERR_NOSUCHCHANNEL ( client->getNickname (), channelName ), client_fd );
	}
}

void Server::extractArgMultiMode ( std::vector< std::string >& splited_cmd, size_t startIndex, Client* client,
								   Channel* channel )
{
	std::vector< std::pair< std::string, std::string > >& multiModeContainer2 = channel->getMultiModeContainer2 ();
	size_t argIndex															  = startIndex;

	std::vector< std::pair< std::string, std::string > > updatedModes;

	for ( std::vector< std::pair< std::string, std::string > >::iterator it = multiModeContainer2.begin ();
		  it != multiModeContainer2.end (); )
	{
		if ( ( it->first[1] == 'k' || it->first[1] == 'o' || it->first[1] == 'l' ) && argIndex < splited_cmd.size () )
		{
			if ( it->first[1] == 'o' && !channel->isInChannel ( splited_cmd[argIndex] ) )
			{
				queueDataForSend ( ERR_USERNOTINCHANNEL ( client->getNickname (), it->second, channel->getName () ),
								   client->getFd () );
				it = multiModeContainer2.erase ( it );
				++argIndex;
				continue;
			}
			it->second = splited_cmd[argIndex];
			++argIndex;
		}
		++it;
	}
}

void Server::extractMultiMode ( std::string splited_cmd, Client* client, Channel* channel )
{
	if ( splited_cmd.find_first_of ( '+' ) != std::string::npos ||
		 splited_cmd.find_first_of ( '-' ) != std::string::npos )
	{
		if ( splited_cmd.length () < 2 )
		{
			queueDataForSend ( ERR_NEEDMOREPARAMS ( client->getNickname (), "MODE" ), client->getFd () );
			return;
		}
		else
		{
			multimode ( channel, client, splited_cmd );
		}
	}
}

void Server::multimode ( Channel* channel, Client* client, std::string multimode )
{
	std::vector< std::pair< std::string, std::string > >& multiModeContainer2 = channel->getMultiModeContainer2 ();
	if ( ( multimode[0] != '+' ) && ( multimode[0] != '-' ) )
	{
		queueDataForSend ( ERR_NEEDMOREPARAMS ( client->getNickname (), "MODE" ), client->getFd () );
		return;
	}
	else
	{
		fillMultiModeVector2 ( multiModeContainer2, multimode, client );
	}
}

void Server::fillMultiModeVector2 ( std::vector< std::pair< std::string, std::string > >& multiModeContainer2,
									std::string& multimode, Client* client )
{
	std::string validModes = "+-kilto";
	bool plus			   = false;
	bool minus			   = false;
	std::string mode	   = "";
	if ( multimode.empty () )
		return;
	for ( size_t i = 0; i < multimode.length (); ++i )
	{
		if ( validModes.find ( multimode[i] ) == std::string::npos )
		{
			std::string msg = client->getNickname () + " the mode " + multimode[i] + " is not considered" + CRLF;
			queueDataForSend ( msg.c_str (), client->getFd () );
			continue;
		}
		else
		{
			if ( multimode[i] == '+' )
			{
				plus  = true;
				minus = false;
				++i;
			}
			else if ( multimode[i] == '-' )
			{
				minus = true;
				plus  = false;
				++i;
			}

			if ( plus )
			{
				mode += "+";
				mode += multimode[i];

				for ( std::vector< std::pair< std::string, std::string > >::iterator it = multiModeContainer2.begin ();
					  it != multiModeContainer2.end (); ++it )
				{
					if ( mode[1] != 'o' && ( mode[1] == it->first[1] ) && ( mode[0] != it->first[0] ) )
					{
						multiModeContainer2.erase ( it );
						break;
					}
				}
			}
			if ( minus )
			{
				mode += "-";
				mode += multimode[i];
				for ( std::vector< std::pair< std::string, std::string > >::iterator it = multiModeContainer2.begin ();
					  it != multiModeContainer2.end (); ++it )
				{
					if ( mode[1] != 'o' && ( mode[1] == it->first[1] ) && mode[0] != it->first[0] )
					{
						multiModeContainer2.erase ( it );
						break;
					}
				}
			}
			multiModeContainer2.push_back ( std::make_pair ( mode, "" ) );
		}
		mode = "";
	}
}