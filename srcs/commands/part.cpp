/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:39:47 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/07 20:36:08 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Command.hpp"
#include "Server.hpp"

// LEAVE CHANNEL
void	Command::handlePart(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle PART failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: PART <channel>{,<channel>} [:<message>]
	if (!args || args->size() < 1)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " PART :Not enough parameters");
		std::cout << "handle PART failed => wrong format" << std::endl;
		return ;
	}

	// TRY TO SPLIT CHANNELS IN ARGS
	try
	{
		std::vector<std::string>	channels = splitChannels(args->at(0), ',');
		std::vector<std::string>::iterator	it;
		for (it = channels.begin(); it != channels.end(); it++)
		{
			// FIND CHANNEL IN SERVER
			Channel	*channel = serv.searchChannel((*it));
			if (!channel)
			{
				Server::sendClient(client->getFd(), ":localhost 403 " + client->getNick() + " " + *it + " :No such channel\n");
				std::cout << "handle PART failed => inexistant channel" << std::endl;
				continue ;
			}

			// CHECK IF CLIENT IS IN THIS CHANNEL
			if (!client->isCurrentChannel(channel->getName()))
			{
				Server::sendClient(client->getFd(), "localhost 442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
				std::cout << "handle PART failed => client isn't in the channel asked" << std::endl;
				continue ;
			}

			// CHECK IF THERE IS NO OP AFTER CLIENT QUIT
			if (channel->getOpList().size() == 1 && channel->isOpName(client->getUser()))
			{
				// SEARCH FOR THE OLDEST CLIENT NOT OP TO OP HIM
				Client	*oldestClient = channel->getOldestClient();
				if (oldestClient)
					channel->getOpList().push_back(oldestClient);
			}

			// IF CHANNEL IS EMPTY
			if (channel->getClientsList().size() == 1)
			{
				Server::sendClient(client->getFd(), ":" + client->getNick() + "!" + client->getUser() + "@localhost PART " + Command::joinStrings(*args) + "\n");
				this->deleteChannel(serv, channel);
				continue ;
			}

			// HANDLE FOR INVITATION ONLY
			if (channel->getInvOnly())
				client->delJoinableChannel(channel);

			// HANDLE IF TARGET IS OP
			if (channel->isOpName(client->getUser()))
				channel->delOpName(client->getUser());

			channel->delClientName(client->getUser());
			client->delCurrentChannel(channel);
			channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost PART " + Command::joinStrings(*args) + "\n");
		}
	}
	// CATCH IF WRONG INPUT IN CHANNELS
	catch (splitFailed &)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " PART :Not enough parameters");
		std::cout << "handle PART failed => wrong format" << std::endl;
		return ;
	}

	std::cout << "handle PART successfuly called" << std::endl;
}
