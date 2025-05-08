/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:39:20 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/08 14:50:12 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Command.hpp"
#include "Server.hpp"

// SEND MESSAGE
void	Command::handlePrivMsg(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle PRIVMSG failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: PRIVMSG <target> :<message>
	if (!args || args->size() != 2)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " PRIVMSG :Not enough parameters");
		std::cout << "handle PRIVMSG failed => invalid format" << std::endl;
		return ;
	}

	// SEARCH FOR ':' IN FRONT OF THE MESSAGE // ONLY FOR NC
	if (args->at(1)[0] != ':')
	{
		Server::sendClient(client->getFd(), ":localhost 902 " + client->getNick() + " :Need a ':' in front of the message\n");
		std::cout << "handle PRIVMSG failed => no ':' at the start of the message" << std::endl;
		return ;
	}

	// CHECK IF THE TARGET IS A CHANNEL
	if (args->at(0)[0] == '#')
	{
		// TRY TO FIND CHANNEL IN THE SERVER
		Channel	*channel = serv.searchChannel(args->at(0));
		if (!channel)
		{
			Server::sendClient(client->getFd(), ":localhost 401 " + client->getNick() + " :No such nick/channel");
			std::cout << "handle PRIVMSG failed => inexistant channel" << std::endl;
			return ;
		}

		// CHECK IF CLIENT IS IN THIS CHANNEL
		if (!client->isCurrentChannel(channel->getName()))
		{
			Server::sendClient(client->getFd(), NO_CHNL_ASK);
			std::cout << "handle PRIVMSG failed => client isn't in the channel asked" << std::endl;
			return ;
		}

		channel->sendClients(client->getUser(), ":" + client->getNick() + "!" + client->getUser() + " PRIVMSG " + channel->getName() + " " + args->at(1) + "\n");
		if (serv.getBot()->isCurrentChannel(channel->getName()))
			Bot::handleBot(serv, channel, client, args->at(1));
	}
	// CHECK IF THE TARGET IS A CLIENT
	else
	{
		// TRY TO FIND THE TARGET IN THE SERVER
		Client	*target = serv.findClientName(args->at(0));
		if (!target)
		{
			Server::sendClient(client->getFd(), ":localhost 401 " + client->getNick() + " :No such nick/channel");
			std::cout << "handle PRIVMSG failed => inexistant target" << std::endl;
			return ;
		}

		if (target->getNick() == "GameBot")
			Bot::handleBot(serv, NULL, client, args->at(1));
		else
			Server::sendClient(target->getFd(), ":" + client->getNick() + "!" + client->getUser() + " PRIVMSG " + target->getNick() + " " + args->at(1) + "\n");
	}

	std::cout << "handle PRIVMSG successfuly called" << std::endl;
}
