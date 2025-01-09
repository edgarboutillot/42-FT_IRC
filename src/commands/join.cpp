#include "include.hpp"
#include "server.hpp"

void Server::extractChannel ( std::string& channel_name, std::vector< std::string >& channel_names )
{
	std::string token;
	std::istringstream tokenStream ( channel_name );

	while ( std::getline ( tokenStream, token, ',' ) )
		channel_names.push_back ( token );
}

void extractKey ( std::string& keyString, std::vector< std::string >& keys )
{
	std::string token;
	std::istringstream tokenStream ( keyString );

	while ( std::getline ( tokenStream, token, ',' ) )
		keys.push_back ( token );
}

void Server::JOIN ( std::string& cmd, int client_fd )
{
	std::vector< std::string > splited_cmd = split_cmd ( cmd );
	if ( splited_cmd.size () < 2 )
	{
		queueDataForSend ( ERR_NEEDMOREPARAMS ( std::string ( "*" ), std::string ( " JOIN" ) ), client_fd );
		return;
	}
	if ( splited_cmd.size () == 2 && splited_cmd[1] == "0" )
	{
		clientJoin0 ( client_fd );
		return;
	}
	clientJoinChannel ( client_fd, cmd );
}

// void Server::clientJoin0 ( int client_fd )
//{
//	Client* client								   = getClientByFd ( client_fd );
//	std::map< std::string, Channel* >& MapChannels = client->getChannels ();
//
//	if ( !client->isRegistered () || !client->isLogged () )
//	{
//		queueDataForSend ( ERR_NOTREGISTERED ( std::string ( "*" ) ), client_fd );
//		return;
//	}
//	if ( MapChannels.empty () )
//		queueDataForSend ( ERR_NOSUCHCHANNEL ( client->getNickname (), "no channel" ), client_fd );
//	else
//	{
//		for ( std::map< std::string, Channel* >::iterator it = MapChannels.begin (); it != MapChannels.end (); ++it )
//		{
//			PART ( "PART " + it->first + ": Leaving", client_fd );
//		}
//		return;
//	}
// }

void Server::clientJoin0 ( int client_fd )
{
	Client* client								   = getClientByFd ( client_fd );
	std::map< std::string, Channel* >& MapChannels = client->getChannels ();

	if ( !client->isRegistered () || !client->isLogged () )
	{
		queueDataForSend ( ERR_NOTREGISTERED ( std::string ( "*" ) ), client_fd );
		return;
	}

	if ( MapChannels.empty () )
	{
		queueDataForSend ( ERR_NOSUCHCHANNEL ( client->getNickname (), "no channel" ), client_fd );
	}
	else
	{
		std::vector< std::string > channelNames;
		for ( std::map< std::string, Channel* >::iterator it = MapChannels.begin (); it != MapChannels.end (); ++it )
			channelNames.push_back ( it->first );

		for ( std::vector< std::string >::iterator it = channelNames.begin (); it != channelNames.end (); ++it )
			PART ( "PART " + *it + ": Leaving", client_fd );
	}
}

void Server::hashtagChecker ( std::vector< std::string >& channels, std::vector< std::string >& keys, Client* client,
							  int client_fd )
{
	for ( size_t i = 0; i < channels.size (); ++i )
		if ( channels[i][0] != '#' )
		{
			queueDataForSend ( ERR_NOSUCHCHANNEL ( client->getNickname (), channels[i] ), client_fd );
			channels.erase ( channels.begin () + i );
			if ( i < keys.size () )
				keys.erase ( keys.begin () + i );
			i--;
		}
}

std::string Server::toLowerCase ( const std::string& input )
{
	std::string result = input; // Crée une copie de la chaîne d'entrée
	for ( std::size_t i = 0; i < result.size (); ++i )
		result[i] = std::tolower ( result[i] ); // Convertit chaque caractère en minuscule
	return result;
}

bool Server::isSameString ( const std::string& str1, const std::string& str2 )
{
	return toLowerCase ( str1 ) == toLowerCase ( str2 );
}

void Server::compareChannelName ( std::vector< std::string >& channelsToCheck )
{
	std::map< std::string, Channel* > channels = getChannels ();
	if ( channels.empty () || channelsToCheck.empty () )
	{
		// std::cout << "No channel to compare" << std::endl;
		return;
	}
	for ( std::map< std::string, Channel* >::iterator it = channels.begin (); it != channels.end (); ++it )
	{
		for ( std::size_t i = 0; i < channelsToCheck.size (); ++i )
			if ( isSameString ( it->first, channelsToCheck[i] ) )
				channelsToCheck[i] = it->first;
	}
}

std::string removeSpacesAfterComma ( const std::string& input )
{
	std::string result;
	bool lastWasComma = false;

	for ( std::string::const_iterator it = input.begin (); it != input.end (); ++it )
	{
		if ( *it == ',' )
		{
			result += *it;
			lastWasComma = true;
		}
		else if ( *it == ' ' && lastWasComma )
		{
			continue;
		}
		else
		{
			result += *it;
			lastWasComma = false;
		}
	}

	return result;
}

std::string removeSpace ( const std::string& str )
{
	std::string::const_iterator start = str.begin ();
	std::string::const_iterator end	  = str.end ();

	while ( start != end && std::isspace ( *start ) )
		++start;

	if ( start != end )
	{
		do
		{
			--end;
		} while ( std::isspace ( *end ) );
		++end;
	}

	return std::string ( start, end );
}

void Server::clientJoinChannel ( int client_fd, std::string& cmd )
{
	Client* client = NULL;

	client = getClientByFd ( client_fd );

	if ( !client->isRegistered () || !client->isLogged () )
	{
		queueDataForSend ( ERR_NOTREGISTERED ( std::string ( "*" ) ), client_fd );
		return;
	}

	std::string newCmd		  = cmd.substr ( cmd.find_first_of ( ' ' ) + 1 );
	newCmd					  = removeSpacesAfterComma ( newCmd );
	std::string channelString = newCmd.substr ( 0, newCmd.find_first_of ( ' ' ) );
	std::vector< std::string > keyValues;

	if ( newCmd.find_first_of ( ' ' ) != std::string::npos )
	{
		std::string keyString = newCmd.substr ( newCmd.find_first_of ( ' ' ) + 1 );
		keyString			  = removeSpace ( keyString );
		extractKey ( keyString, keyValues );
	}

	std::vector< std::string > channelNames;
	extractChannel ( channelString, channelNames );
	hashtagChecker ( channelNames, keyValues, client, client_fd );
	compareChannelName ( channelNames );

	if ( channelNames.empty () )
	{
		queueDataForSend ( ERR_NEEDMOREPARAMS ( std::string ( "*" ), cmd ), client_fd );
		return;
	}

	for ( std::size_t i = 0; i < channelNames.size (); ++i )
	{
		std::map< std::string, Channel* >::iterator it = _MapChannels.find ( channelNames[i] );
		if ( it != _MapChannels.end () )
		{
			Channel* channel = it->second;
			if ( channel->isInChanByFd ( client_fd ) )
				continue;
			if ( channel->isLimited () && channel->getNumberOfClients () >= channel->getClientLimit () )
			{
				queueDataForSend ( ERR_CHANNELISFULL ( client->getNickname (), channel->getName () ), client_fd );
				return;
			}
			if ( !channel->isOnInvitation () && !channel->isProtected () )
			{
				joinExistingChannel ( channel, client, client_fd );
			}
			else if ( channel->isOnInvitation () && !channel->isProtected () )
			{
				if ( channel->isInInviteList ( client ) )
					joinExistingChannel ( channel, client, client_fd );
				else
					queueDataForSend ( ERR_INVITEONLYCHAN ( client->getNickname (), channel->getName () ), client_fd );
			}
			else if ( !channel->isOnInvitation () && channel->isProtected () )
			{
				if ( keyValues.size () == 0 )
				{
					queueDataForSend ( ERR_BADCHANNELKEY ( client->getNickname (), channelNames[i] ), client_fd );
					return;
				}
				if ( channel->isGoodKey ( keyValues[i] ) )
					joinExistingChannel ( channel, client, client_fd );
				else
					queueDataForSend ( ERR_BADCHANNELKEY ( client->getNickname (), channel->getName () ), client_fd );
			}
			else if ( channel->isOnInvitation () && channel->isProtected () )
			{
				if ( keyValues.size () == 0 )
				{
					queueDataForSend ( ERR_BADCHANNELKEY ( client->getNickname (), channelNames[i] ), client_fd );
					return;
				}
				if ( channel->isInInviteList ( client ) && channel->isGoodKey ( keyValues[i] ) )
					joinExistingChannel ( channel, client, client_fd );
				else
					queueDataForSend ( ERR_INVITEONLYCHAN ( client->getNickname (), channel->getName () ), client_fd );
			}
		}
		else
		{
			createAndJoinNewChannel ( channelNames[i], client, client_fd );
			if ( i < keyValues.size () )
			{
				std::map< std::string, Channel* >::iterator it = _MapChannels.find ( channelNames[i] );
				Channel* channel							   = it->second;
				channel->setPassword ( keyValues[i], true );
				channel->addArgToModeVector ( "k" );
			}
		}
	}
}

void Server::joinExistingChannel ( Channel* channel, Client* client, int client_fd )
{
	channel->addClient ( client->getNickname (), client );
	channel->setClientTime ( client );

	std::map< std::string, Channel* >& channelOfClient = client->getChannels ();
	channelOfClient[channel->getName ()]			   = channel;

	queueDataForSend ( JOIN_CHANNEL ( channel->getName (), client->getNickname () ), client_fd );

	queueDataForSend ( RPL_CHANNELMODEIS ( client->getNickname (), channel->getName (), channel->getModeArgs () ),
					   client_fd );

	sendChannel ( client, channel,
				  RPL_NAMREPLY ( channel->getName (), client->getNickname (), channel->getAllNames () ) );
	queueDataForSend(RPL_NAMREPLY(channel->getName(), client->getNickname(), channel->getAllNames()), client_fd);
	queueDataForSend(RPL_ENDOFNAMES(client->getNickname(), channel->getName()), client_fd);

	if ( channel->isTopic () )
		queueDataForSend ( RPL_TOPIC ( client->getNickname (), channel->getName (), channel->getTopic () ), client_fd );
	else
		queueDataForSend ( RPL_NOTOPIC ( client->getNickname (), channel->getName () ), client_fd );


	queueDataForSend ( RPL_NAMREPLY ( channel->getName (), client->getNickname (), channel->getAllNames () ),
					   client_fd );

	queueDataForSend ( RPL_ENDOFNAMES ( client->getNickname (), channel->getName () ), client_fd );
}

void Server::createAndJoinNewChannel ( const std::string& channelName, Client* client, int client_fd )
{
	addChannel ( channelName );
	addUserToChannel ( channelName, client );
	Channel* channel = getChannelByName ( channelName );
	channel->setClientTime ( client );
	channel->setChannelOperator ( client );
	channel->addArgToModeVector ( "+" );

	queueDataForSend ( JOIN_CHANNEL ( channel->getName (), client->getNickname () ), client_fd );

	queueDataForSend ( RPL_CHANNELMODEIS ( client->getNickname (), channel->getName (), channel->getModeArgs () ),
					   client_fd );
	sendChannel ( client, channel,
				  RPL_NAMREPLY ( channel->getName (), client->getNickname (), channel->getAllNames () ) );
	queueDataForSend(RPL_NAMREPLY(channel->getName(), client->getNickname(), channel->getAllNames()), client_fd);
	queueDataForSend(RPL_ENDOFNAMES(client->getNickname(), channel->getName()), client_fd);
	if ( channel->isTopic () )
		queueDataForSend ( RPL_TOPIC ( client->getNickname (), channel->getName (), channel->getTopic () ), client_fd );
	else
		queueDataForSend ( RPL_NOTOPIC ( client->getNickname (), channel->getName () ), client_fd );

	queueDataForSend ( RPL_NAMREPLY ( channel->getName (), client->getNickname (), channel->getAllNames () ),
					   client_fd );
	queueDataForSend ( RPL_ENDOFNAMES ( client->getNickname (), channel->getName () ), client_fd );
}
