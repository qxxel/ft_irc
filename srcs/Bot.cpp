/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:55:23 by mreynaud          #+#    #+#             */
/*   Updated: 2025/05/07 23:42:23 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include "Channel.hpp"
#include "Server.hpp"

Bot::Bot(): Client(-2) {
	_auth = true;
	_pwd = true;
	_user = "GameBot";
	_nick = "GameBot";
	srand(time(NULL));
}

Bot::~Bot() { }

void	Bot::joinChannel(Channel	*channel)
{
	if (channel->getMaxUsers() != 0 && !(channel->getClientsList().size() < (unsigned long)channel->getMaxUsers()))
	{
		channel->sendClients("", "the bot (GameBot) cannot rejoin, because the channel is full\n");
		std::cout << "GameBot JOIN failed => channel full" << std::endl;
		return ;
	}
	this->getCurrentsChannels().push_back(channel);
	channel->getClientsList().push_back(this);
	channel->getOpList().push_back(this);
	channel->sendClients("", ":" + this->getNick() + "!" + this->getUser() + " JOIN :" + channel->getName() + "\n");
}

static void	send_msg_bot(std::string msg, Client *client, Channel *channel)
{
	if (channel)
		channel->sendClients("", ":GameBot!GameBot PRIVMSG " + channel->getName() + " " + msg);
	else
		Server::sendClient(client->getFd(), ":GameBot!GameBot PRIVMSG " + client->getNick() + " " + msg);
}

static void explain_rule(Client *client, Channel *channel)
{
	send_msg_bot("GAME RULE:\n", client, channel);
	send_msg_bot("Rock-Paper-Scissors is a game where two players each choose one of three shapes (Rock, Paper and Scissors):\n", client, channel);
	send_msg_bot("The rules are simple:\n", client, channel);
	send_msg_bot(" - Rock beats scissors\n", client, channel);
	send_msg_bot(" - Scissors beats paper\n", client, channel);
	send_msg_bot(" - Paper beats rock\n", client, channel);
	send_msg_bot("If both players choose the same shape, it's a tie.\n", client, channel);
	send_msg_bot("To try :\t!GAME [rock|paper|scissors]\n", client, channel);
}


static void you_lose(std::string my_choice, Client *client, Channel *channel)
{
	send_msg_bot("I'm choice " + my_choice + "\n", client, channel);
	send_msg_bot("You lose!\n", client, channel);
	// /!\ WIP : try to kick
}

static void no_winner(std::string my_choice, Client *client, Channel *channel)
{
	send_msg_bot("I'm choice " + my_choice + "\n", client, channel);
	send_msg_bot("It's a tie! Go again?\n", client, channel);
}

static void you_win(std::string my_choice, Client *client, Channel *channel)
{
	send_msg_bot("I'm choice " + my_choice + "\n", client, channel);
	send_msg_bot("You win! 🏆\n", client, channel);
	// /!\ WIP : and try to op
}

static std::string	rand_rock_paper_scissors()
{
	int nb = rand() % 3;

	if (nb == 0)
		return "ROCK";
	else if (nb == 1)
		return "PAPER";
	else
		return "SCISSORS";
}

static void	game_rock_paper_scissors(std::string adverse_choice, Client *client, Channel *channel)
{
	std::string my_choice = rand_rock_paper_scissors();
	if (adverse_choice == "ROCK")
	{
		if (my_choice == "ROCK")
			no_winner(my_choice, client, channel);
		else if (my_choice == "PAPER")
			you_lose(my_choice, client, channel);
		else
			you_win(my_choice, client, channel);
	}
	else if (adverse_choice == "PAPER")
	{
		if (my_choice == "ROCK")
			you_win(my_choice, client, channel);
		else if (my_choice == "PAPER")
			no_winner(my_choice, client, channel);
		else
			you_lose(my_choice, client, channel);
	}
	else if (adverse_choice == "SCISSORS")
	{
		if (my_choice == "ROCK")
			you_lose(my_choice, client, channel);
		else if (my_choice == "PAPER")
			you_win(my_choice, client, channel);
		else
			no_winner(my_choice, client, channel);
	}
	else if (adverse_choice == "WELL")
		you_lose("PAPER", client, channel);
	else
		explain_rule(client, channel);
	return ;
}

void Bot::handleBot(Channel *channel, Client *client, std::string &arg)
{
	if (arg.find("!GAME") != 1 || arg.size() <= 7)
	{
		explain_rule(client, channel);
		return ;
	}
	std::string str = arg.substr(7);

	game_rock_paper_scissors(Server::str_toupper(str), client, channel);
}
