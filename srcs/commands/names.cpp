/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:40:21 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/07 20:35:35 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Command.hpp"
#include "Server.hpp"

// VIEW CLIENTS LIST OF A CHANNEL
void	Command::handleNames(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle NAMES failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: NAMES [<channel>{,<channel>}]
	if (!args || args->size() > 1)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " NAMES :Not enough parameters");
		std::cout << "handle NAMES failed => no args" << std::endl;
		return ;
	}

	// IF CLIENT WANT TO SEE THE CLIENT LIST OF ALL CHANNELS
	if (args->size() == 0)
	{
		std::vector<Channel*>::iterator	it_chnl;
		for (it_chnl = serv.getChannels().begin(); it_chnl != serv.getChannels().end(); it_chnl++)
		{
			Server::sendClient(client->getFd(), ":localhost 353 " + client->getNick() + " = " + (*it_chnl)->getName() + " :" + (*it_chnl)->listClients() + "\n");
			Server::sendClient(client->getFd(), ":localhost 366 " + client->getNick() + " " + (*it_chnl)->getName() + " :End of NAMES list\n");
		}

		std::cout << "handle NAMES all successfully called" << std::endl;
		return ;
	}

	// IF CLIENT WANT TO SEE THE CLIENT LIST OF SPECIFIC CHANNEL
	try
	{
		std::vector<std::string>			channels = splitChannels(args->at(0), ',');
		std::vector<std::string>::iterator	it;
		for (it = channels.begin(); it != channels.end(); it++)
		{
			// FIND CHANNEL IN SERVER
			Channel	*channel = serv.searchChannel(*it);
			if (channel)
				Server::sendClient(client->getFd(), ":localhost 353 " + client->getNick() + " = " + channel->getName() + " :" + channel->listClients() + "\n");
			Server::sendClient(client->getFd(), ":localhost 366 " + client->getNick() + " " + args->at(0) + " :End of NAMES list\n");
		}
	}
	catch (splitFailed &)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " NAMES :Not enough parameters");
		std::cout << "handle NAMES failed => wrong format" << std::endl;
		return ;
	}

	std::cout << "handle NAMES successfully called\n";
}
