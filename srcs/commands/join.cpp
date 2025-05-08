/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:39:41 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/08 16:37:04 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Command.hpp"
#include "Server.hpp"

// JOIN CHANNEL
void	Command::handleJoin(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle JOIN failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: JOIN <channel>{,<channel>} [<key>{,<key>}]
	if (!args || args->size() < 1 || 2 < args->size())
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " JOIN :Not enough parameters");
		std::cout << "handle JOIN failed => invalid format" << std::endl;
		return ;
	}

	// TRY TO MAKE PAIR WITH EACH CHANNELS AND PASSWORDS
	try
	{
		std::string	passwords;
		if (args->size() == 2)
			passwords = args->at(1);

		std::map<std::string, std::string> 				channelsAsk = splitChannelsPasswords(args->at(0), passwords, ',');
		std::map<std::string, std::string>::iterator	it;
		for (it = channelsAsk.begin(); it != channelsAsk.end(); it++)
		{
			// CHECK IF CLIENT ALREADY IN THE CHANNEL
			if (client->isCurrentChannel(it->first))
			{
				Server::sendClient(client->getFd(), ":localhost 443 " + client->getNick() + " " + it->first + " :is already on channel\n");
				std::cout << "handle JOIN failed => client already in this channel" << std::endl;
				continue ;
			}

			// CHECK IF THE NAME IS VALID
			if (it->first.size() < 2 || 50 < it->first.size() || it->first[0] != '#' || !isValidString(it->first, true))
			{
				Server::sendClient(client->getFd(), ":localhost 476 " + client->getNick() + " " + it->first + " :Bad Channel Mask\n");
				std::cout << "handle JOIN failed => invalid name" << std::endl;
				continue ;
			}

			// FIND THE CHANNEL IN THE SERVER
			Channel	*channel = serv.searchChannel(it->first);
			if (!channel)
			{
				// CREATE CHANNEL
				Channel *newChannel = new Channel(it->first, client);

				serv.addChannel(newChannel);
				client->getCurrentsChannels().push_back(newChannel);
				Server::sendClient(client->getFd(), ":" + client->getNick() + "!" + client->getUser() + "@localhost JOIN :" + newChannel->getName() + "\n");
				Server::sendClient(client->getFd(), ":localhost 331 " + client->getNick() + " " + newChannel->getName() + " :No topic is set\n");
				Server::sendClient(client->getFd(), ":localhost 353 " + client->getNick() + " " + newChannel->getName() + " :" + newChannel->listClients() + "\n");
				Server::sendClient(client->getFd(), ":localhost 366 " + client->getNick() + " " + newChannel->getName() + " :End of NAMES list\n");
				continue ;
			}

			// CHECK IF CLIENT CAN ACCESS
			if (client->getNick() != "GameBot" && channel->getInvOnly() && !(client->isJoinableChannel(channel) || channel->isOpName(client->getUser())))
			{
				Server::sendClient(client->getFd(), ":localhost 473 " + client->getNick() + " " + channel->getName() + " :Cannot join channel (+i)");
				std::cout << "handle JOIN failed => client not allowed in this channel" << std::endl;
				continue ;
			}

			// CHECK PASSWORD
			if (client->getNick() != "GameBot" && !channel->getPwd().empty() && !client->isJoinableChannel(channel))
			{
				// CHECK IF PASSWORD IS SENDED AND TRY IT
				if (it->second.empty() || channel->getPwd() != it->second)
				{
					Server::sendClient(client->getFd(), ":localhost 475 " + client->getNick() + " " + channel->getName() + " :Cannot join channel (+k)");
					std::cout << "handle JOIN failed => wrong password" << std::endl;
					continue ;
				}
			}

			// CHECK IF CHANNEL IS FULL
			if (channel->getMaxUsers() != -1 && channel->getClientsList().size() >= (size_t)channel->getMaxUsers())
			{
				Server::sendClient(client->getFd(), ":localhost 471 " + client->getNick() + " " + channel->getName() + " :Cannot join channel (+l)");
				std::cout << "handle JOIN failed => channel full" << std::endl;
				continue ;
			}

			// ADD CLIENT IN CHANNEL AND SEND MESSAGES
			client->getCurrentsChannels().push_back(channel);
			channel->getClientsList().push_back(client);
			channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost JOIN :" + channel->getName() + "\n");
			if (channel->getTopic().empty())
				Server::sendClient(client->getFd(), ":localhost 331 " + client->getNick() + " " + channel->getName() + " :No topic is set\n");
			else
				Server::sendClient(client->getFd(), ":localhost 332 " + client->getNick() + " " + channel->getName() + " " + channel->getTopic() + "\n");
			Server::sendClient(client->getFd(), ":localhost 353 " + client->getNick() + " " + channel->getName() + " :" + channel->listClients() + "\n");
			Server::sendClient(client->getFd(), ":localhost 366 " + client->getNick() + " " + channel->getName() + " :End of NAMES list\n");
			if (client->getNick() == "GameBot")
				channel->getOpList().push_back(client);
		}
	}
	// CATCH IF WRONG INPUT IN CHANNELS
	catch (splitFailed &)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " JOIN :Not enough parameters");
		std::cout << "handle JOIN failed => invalid format" << std::endl;
	}

	std::cout << "handle JOIN successfully called" << std::endl;
}
