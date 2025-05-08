/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:40:07 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/08 15:50:34 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Command.hpp"
#include "Server.hpp"

// INVITE USER IN CURRENT CHANNEL
void	Command::handleInvite(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle INVITE failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: INVITE <user> <channel>
	if (!args || args->empty() || args->size() != 2)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " INVITE :Not enough parameters");
		std::cout << "handle INVITE failed => invalid format" << std::endl;
		return ;
	}

	// FIND TARGET IN SERVER
	Client	*target = serv.findClientName(args->at(0));
	if (!target)
	{
		Server::sendClient(client->getFd(), ":localhost 401 " + client->getNick() + " " + target->getNick() + " :No such nick/channel\n");
		std::cout << "handle INVITE failed => target don't exist" << std::endl;
		return ;
	}

	// FIND CHANNEL IN SERVER
	Channel	*channel = serv.searchChannel(args->at(1));
	if (!channel)
	{
		Server::sendClient(client->getFd(), ":localhost 403 " + client->getNick() + " " + args->at(1) + " :No such channel\n");
		std::cout << "handle INVITE failed => inexistant channel" << std::endl;
		return ;
	}

	// FIND CLIENT IN THE CHANNEL
	if (!client->isCurrentChannel(channel->getName()))
	{
		Server::sendClient(client->getFd(), ":localhost 442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
		std::cout << "handle INVITE failed => client isn't in the channel" << std::endl;
		return ;
	}

	// FIND TARGET IN THE CHANNEL
	if (target->isCurrentChannel(channel->getName()))
	{
		Server::sendClient(client->getFd(), ":localhost 443 " + client->getNick() + " " + target->getNick() + " " + channel->getName() + " :is already on channel\n");
		std::cout << "handle INVITE failed => target already in the channel" << std::endl;
		return ;
	}

	// CHECK INVITE ONLY CHANNEL
	if (channel->getInvOnly())
	{
		// CHECK IF CLIENT IS OP
		if (!channel->isOpName(target->getUser()))
		{
			Server::sendClient(client->getFd(), ":localhost 482 " + client->getNick() + " " + channel->getName() + " :You're not channel operator\n");
			std::cout << "handle INVITE failed => client isn't moderator" << std::endl;
			return ;
		}

		// ADD TARGET TO ACCESS LIST
		target->getJoinableChannels().push_back(channel);
	}

	if (target->getUser() == "GameBot")
		serv.getBot()->joinChannel(serv, channel);
	else
	{
		Server::sendClient(target->getFd(), ":" + client->getNick() + "!" + client->getUser() + "@localhost INVITE " + target->getNick() + " :" + channel->getName() + "\n");
		Server::sendClient(client->getFd(), ":localhost 341 " + client->getNick() + " " + target->getNick() + " " + channel->getName() + "\n");
	}
	std::cout << "handle INVITE successfuly called" << std::endl;
}
