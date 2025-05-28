/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:40:01 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/28 17:59:00 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Command.hpp"
#include "Server.hpp"

// KICK USER ON THE CURRENT CHANNEL
void	Command::handleKick(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle KICK failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: KICK <channel> <user> [:<comment>]
	if (!args || args->size() < 2 || 3 < args->size())
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " KICK :Not enough parameters\n");
		std::cout << "handle KICK failed => invalid format" << std::endl;
		return ;
	}

	// CHECK IF CHANNEL EXIST
	Channel	*channel = serv.searchChannel(args->at(0));
	if (!channel)
	{
		Server::sendClient(client->getFd(), ":localhost 403 " + client->getNick() + " " + args->at(0) + " :No such channel\n");
		std::cout << "handle KICK failed => channel don't exist" << std::endl;
		return ;
	}

	// FIND CLIENT IN THE CHANNEL
	if (!client->isCurrentChannel(channel->getName()))
	{
		Server::sendClient(client->getFd(), ":localhost 442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
		std::cout << "handle KICK failed => client not in the channel asked" << std::endl;
		return ;
	}

	// FIND TARGET IN THE CHANNEL
	Client	*target = channel->findClientName(args->at(1));
	if (!target)
	{
		Server::sendClient(client->getFd(), ":localhost 441 " + client->getNick() + " " + args->at(1) + " " + channel->getName() + " :They aren't on that channel\n");
		std::cout << "handle KICK failed => target isn't in the channel" << std::endl;
		return ;
	}

	// CHECK IF CLIENT IS OP
	if (!channel->isOpClient(client->getFd()))
	{
		Server::sendClient(client->getFd(), ":localhost 482 " + client->getNick() + " " + channel->getName() + " :You're not channel operator\n");
		std::cout << "handle KICK failed => client isn't moderator" << std::endl;
		return ;
	}

	// CHECK IF CLIENT IS TRING TO KICK HIMSELF
	if (client == target)
	{
		Server::sendClient(client->getFd(), ":localhost 903 " + client->getNick() + " " + channel->getName() + " :You cannot kick yourself\n");
		std::cout << "handle KICK failed => the client cannot kick himself" << std::endl;
		return ;
	}

	// CHECK IF THERE IS A ':' IN FRONT OF THE COMMENT // ONLY FOR NC
	if (args->size() == 3 && args->at(2)[0] != ':')
	{
		Server::sendClient(client->getFd(), ":localhost 902 " + client->getNick() + " " + channel->getName() + " :Need a ':' in front of the comment\n");
		std::cout << "handle KICK failed => there is no ':' in front of the comment" << std::endl;
		return ;
	}

	// HANDLE FOR INVITATION ONLY
	if (channel->getInvOnly())
		target->delJoinableChannel(channel);

	// HANDLE IF TARGET IS OP
	if (channel->isOpClient(target->getFd()))
		channel->delOpClient(target->getFd());

	channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost KICK " + Command::joinStrings(*args) + "\n");
	target->delCurrentChannel(channel);
	channel->delClient(target->getFd());

	std::cout << "handle KICK successfully called" << std::endl;
}
