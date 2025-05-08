/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 19:40:36 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/08 15:15:56 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Command.hpp"
#include "Server.hpp"

// VIEW OR SET A MODE IN A CHANNEL
void	Command::handleMode(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle MODE failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: MODE <channel> [<+/-modes>] [<arguments>]
	if (!args || args->size() < 1)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " MODE :Not enough parameters");
		std::cout << "handle MODE failed => invalid format" << std::endl;
		return ;
	}

	// FIND CHANNEL IN SERVER
	Channel	*channel = serv.searchChannel(args->at(0));
	if (!channel)
	{
		Server::sendClient(client->getFd(), NO_CHNL);
		std::cout << "handle TOPIC failed => channel don't exist" << std::endl;
		return ;
	}

	// FIND CLIENT IN THE CHANNEL
	if (!client->isCurrentChannel(channel->getName()))
	{
		Server::sendClient(client->getFd(), NO_CHNL_ASK);
		std::cout << "handle TOPIC failed => client isn't in the channel asked" << std::endl;
		return ;
	}

	// SEND ACTIVE MODES
	if (args->size() == 1)
	{
		std::string	modes = " +";
		std::string	modesArgs;

		if (channel->getInvOnly())
			modes += "i";
		else if (!channel->getTopic().empty())
		{
			modes += "t";
			modesArgs += " " + channel->getTopic();
		}
		else if (!channel->getPwd().empty())
			modes += "k";
		else if (channel->getMaxUsers())
		{
			modes += "l";
			modesArgs += " " + intToString(channel->getMaxUsers());
		}
		else
			modes = "";

		Server::sendClient(client->getFd(), client->getNick() + " " + channel->getName() + modes + modesArgs + "\n");
		Server::sendClient(client->getFd(), client->getNick() + " " + channel->getName() + " " + timeToString(channel->getModeSetTimestamp()) + "\n");
		std::cout << "handle ask MODE successfuly called\n";
		return ;
	}

	// CHECK IF CLIENT IS OP
	if (!channel->isOpName(client->getUser()))
	{
		Server::sendClient(client->getFd(), NO_PERM);
		std::cout << "handle modify MODE failed => client isn't moderator" << std::endl;
		return ;
	}

	// ADD INVITE ONLY MODE
	if (args->at(1) == "+i")
	{
		// USAGE: MODE <channel> +i
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> +i\n");
			std::cout << "handle modify MODE +i failed => invalid format" << std::endl;
			return ;
		}

		channel->setInvOnly(true);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " +i\n");

		// ADD CHANNEL IN CURRENT CLIENTS' JOINABLE CHANNELS
		for (std::vector<Client*>::iterator it = channel->getClientsList().begin(); it != channel->getClientsList().end(); it++)
			(*it)->getJoinableChannels().push_back(channel);
	}
	// SUPP INVITE ONLY MODE
	if (args->at(1) == "-i")
	{
		// USAGE: MODE <channel> -i
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> -i\n");
			std::cout << "handle modify MODE -i failed => invalid format" << std::endl;
			return ;
		}

		channel->setInvOnly(false);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " -i\n");

		// ERASE CHANNEL IN ALL CLIENTS' JOINABLE CHANNELS
		for (std::vector<Client*>::iterator it1 = serv.getClients().begin(); it1 != serv.getClients().end(); it1++)
		{
			for (std::vector<Channel*>::iterator it2 = (*it1)->getJoinableChannels().begin(); it2 != (*it1)->getJoinableChannels().end(); it2++)
			{
				if ((*it2) == channel)
				{
					(*it1)->getJoinableChannels().erase(it2);
					break ;
				}
			}
		}
	}

	// ADD LOCKED TOPIC MODE
	if (args->at(1) == "+t")
	{
		// USAGE: MODE <channel> +t
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> +t\n");
			std::cout << "handle modify MODE +t failed => invalid format" << std::endl;
			return ;
		}

		channel->setLockTopic(true);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " +t\n");
	}
	// SUPP LOCKED TOPIC MODE
	else if (args->at(1) == "-t")
	{
		// USAGE: MODE <channel> -t
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> -t\n");
			std::cout << "handle modify MODE -t failed => invalid format" << std::endl;
			return ;
		}

		channel->setLockTopic(false);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " -t\n");
	}

	// ADD PASSWORD MODE
	else if (args->at(1) == "+k")
	{
		// USAGE: MODE <channel> +k <new_password>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> +k <new_password>\n");
			std::cout << "handle modify MODE +k failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF ALL CHARACTERS ARE VALID
		if (isValidString(args->at(2), true))
		{
				Server::sendClient(client->getFd(), INV_PASS_FRMT);
				std::cout << "handle modify MODE +k failed => invalid password format" << std::endl;
				return ;
		}

		channel->setPwd(args->at(2));
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " +k\n");
	}
	// SUPP PASSWORD MODE
	else if (args->at(1) == "-k")
	{
		// USAGE: MODE <channel> -k <actual_password>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> -k <actual_password>\n");
			std::cout << "handle modify MODE -k failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF THE PASSWORD IS GOOD
		if (args->at(2) != channel->getPwd())
		{
			Server::sendClient(client->getFd(), WRNG_PASS);
			std::cout << "handle modify MODE -k failed => wrong password" << std::endl;
			return ;
		}

		channel->setPwd("");
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " -k\n");
	}

	// SET OP TARGET
	else if (args->at(1) == "+o")
	{
		// USAGE: MODE <channel> +o <target_username>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> +o <target_username>\n");
			std::cout << "handle modify MODE +o failed => invalid format" << std::endl;
			return ;
		}

		// TRY TO FIND TARGET IN CHANNEL
		Client	*target = channel->findClientName(args->at(2));
		if (!target)
		{
			Server::sendClient(client->getFd(), TRGT_NOT_FOUND);
			std::cout << "handle modify MODE +o failed => inexistant target" << std::endl;
			return ;
		}

		// CHECK IF TARGET IS OP
		if (channel->isOpName(target->getUser()))
		{
			Server::sendClient(client->getFd(), ALRD_OP);
			std::cout << "handle modify MODE +o failed => target already op" << std::endl;
			return ;
		}

		channel->getOpList().push_back(target);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " +o " + target->getUser() + "\n");
	}
	// UNSET OP TARGET
	else if (args->at(1) == "-o")
	{
		// USAGE: MODE <channel> -o <target_username>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> -o <target_username>\n");
			std::cout << "handle modify MODE -o failed => invalid format" << std::endl;
			return ;
		}

		// TRY TO FIND TARGET IN CHANNEL
		Client	*target = channel->findClientName(args->at(2));
		if (!target)
		{
			Server::sendClient(client->getFd(), TRGT_NOT_FOUND);
			std::cout << "handle modify MODE -o failed => inexistant target" << std::endl;
			return ;
		}

		// CHECK IF TARGET ISNT OP
		if (!channel->isOpName(target->getUser()))
		{
			Server::sendClient(client->getFd(), ISNT_OP);
			std::cout << "handle modify MODE -o failed => target isn't op" << std::endl;
			return ;
		}

		channel->delOpName(target->getUser());
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " -o " + target->getUser() + "\n");
	}

	// ADD USER LIMIT MODE
	else if (args->at(1) == "+l")
	{
		// USAGE: MODE <channel> +l <new_user_limit>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> +t <new_user_limit>\n");
			std::cout << "handle modify MODE +l failed => invalid format" << std::endl;
			return ;
		}

		// TRY TO SET THE NEW LIMIT
		try
		{
			channel->setMaxUsers(stringToInt(args->at(2)));
			channel->setModeSetTimestamp(time(NULL));
			channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " +l " + args->at(2) + "\n");
		}
		// IF THE INPUT ISNT AN INT
		catch (notIntNumber &e)
		{
			Server::sendClient(client->getFd(), std::string(e.what()) + "\n");
			std::cout << "handle modify MODE +l failed => " << e.what() << std::endl;
			return ;
		}
	}
	// SUPP USER LIMIT MODE
	else if (args->at(1) == "-l")
	{
		// USAGE: MODE <channel> -l
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> -l\n");
			std::cout << "handle modify MODE -l failed => invalid format" << std::endl;
			return ;
		}

		channel->setMaxUsers(0);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " -l\n");
	}

	// OTHERS USAGES
	else
	{
		Server::sendClient(client->getFd(), LST_MODES);
		std::cout << "handle modify MODE failed => not an existant flag" << std::endl;
		return ;
	}

	std::cout << "handle modify MODE " << args->at(1) << " successfuly called\n";
}
