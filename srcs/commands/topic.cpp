/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:40:14 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/28 18:26:31 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Command.hpp"
#include "Server.hpp"

// VIEW OR SET TOPIC OF THE CURRENT CHANNEL
void	Command::handleTopic(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle TOPIC failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: TOPIC <channel> [:<new_topic>]
	if (!args || args->empty() || args->size() > 2)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " TOPIC :Not enough parameters\n");
		std::cout << "handle TOPIC failed => no args" << std::endl;
		return ;
	}

	// IF ARG ISNT A CHANNEL
	if (args->at(0).find("#") == std::string::npos)
	{
		Server::sendClient(client->getFd(), ":localhost 403 " + client->getNick() + " " + args->at(0) + " :Not a channel\n");
		std::cout << "handle TOPIC failed => Not a channel" << std::endl;
		return ;
	}

	// FIND CHANNEL IN SERVER
	Channel	*channel = serv.searchChannel(args->at(0));
	if (!channel)
	{
		Server::sendClient(client->getFd(), ":localhost 403 " + client->getNick() + " " + channel->getName() + " :No such channel\n");
		std::cout << "handle TOPIC failed => channel don't exist" << std::endl;
		return ;
	}

	// VIEW TOPIC
	if (args->size() == 1)
	{
		if (channel->getTopic().empty())
			Server::sendClient(client->getFd(), ":localhost 331 " + client->getNick() + " " + channel->getName() + " :" + NO_TPC);
		else
		{
			Server::sendClient(client->getFd(), ":localhost 332 " + client->getNick() + " " + channel->getName() + " " + channel->getTopic() + "\n");
			Server::sendClient(client->getFd(), ":localhost 333 " + client->getNick() + " " + channel->getName() + " " + channel->getTopicSetter()+ " " + timeToString(channel->getTopicSetTimestamp()) + "\n");
		}
	}
	// SET TOPIC
	else
	{
		// FIND CLIENT IN THE CHANNEL
		if (!client->isCurrentChannel(channel->getName()))
		{
			Server::sendClient(client->getFd(), ":localhost 442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
			std::cout << "handle set TOPIC failed => client isn't in the channel asked" << std::endl;
			return ;
		}

		// CHECK IF CLIENT IS OP
		if (channel->getLockTopic() && !channel->isOpClient(client->getFd()))
		{
			Server::sendClient(client->getFd(), ":localhost 482 " + client->getNick() + " " + channel->getName() + " :You're not channel operator\n");
			std::cout << "handle set TOPIC failed => client isn't moderator" << std::endl;
			return ;
		}

		// CHECK IF THERE IS A ':' IN FRONT OF THE NEW TOPIC // ONLY FOR NC
		if (args->at(1)[0] != ':')
		{
			Server::sendClient(client->getFd(), ":localhost 902 " + client->getNick() + " " + channel->getName() + " :Need a ':' in front of the topic\n");
			std::cout << "handle set TOPIC failed => there is no ':' in front of the topic" << std::endl;
			return ;
		}

		// CHECK IF ALL CHARACTERS ARE VALID
		if (!isValidString(args->at(1), true, true))
		{
			Server::sendClient(client->getFd(), ":localhost 900 " + client->getNick() + " " + channel->getName() + " :invalid characters in topic\n");
			std::cout << "handle set TOPIC failed => there is invalid character" << std::endl;
			return ;
		}

		// CHECK IF THE TOPIC IS ALREADY SET
		if (channel->getTopic() == args->at(1))
		{
			Server::sendClient(client->getFd(), ":localhost 901 " + client->getNick() + " " + channel->getName() + " :Topic already set\n");
			std::cout << "handle set TOPIC failed => topic already set" << std::endl;
			return ;
		}

		channel->setTopic(args->at(1));
		channel->setTopicSetter(client->getNick());
		channel->setTopicSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost TOPIC " + channel->getName() + " " + channel->getTopic() + "\n");
	}

	std::cout << "handle TOPIC successfully called" << std::endl;
}
