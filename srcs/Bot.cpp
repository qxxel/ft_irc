/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 14:55:23 by mreynaud          #+#    #+#             */
/*   Updated: 2025/04/29 16:35:36 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include "Server.hpp"
#include "Bot.hpp"

Bot::Bot(int fd) : Client(fd)
{
	srand(time(NULL));
}

Bot::~Bot() {}

static void explain_rule(Client *client)
{
	Server::sendClient(client->getFd(), "\nGAME RULE:\n");
	Server::sendClient(client->getFd(), "Rock-Paper-Scissors is a game where two players each choose one of three shapes (Rock, Paper and Scissors):\n\n");
	Server::sendClient(client->getFd(), "The rules are simple:\n");
    Server::sendClient(client->getFd(), " - Rock beats scissors\n");
    Server::sendClient(client->getFd(), " - Scissors beats paper\n");
    Server::sendClient(client->getFd(), " - Paper beats rock\n");
	Server::sendClient(client->getFd(), "If both players choose the same shape, it's a tie.\n\n");
	Server::sendClient(client->getFd(), "To try :\t!GAME [rock|paper|scissors]\n");
}

static void you_lose(std::string my_choice, Client *client)
{
	Server::sendClient(client->getFd(), "I'm choice " + my_choice + "\n");
	Server::sendClient(client->getFd(), "You lose!\n");
	// and try to kick
}

static void no_winner(std::string my_choice, Client *client)
{
	Server::sendClient(client->getFd(), "I'm choice " + my_choice + "\n");
	Server::sendClient(client->getFd(), "It’s a tie! Go again?\n");
}

static void you_win(std::string my_choice, Client *client)
{
	Server::sendClient(client->getFd(), "I'm choice " + my_choice + "\n");
	Server::sendClient(client->getFd(), "You win! 🏆\n");
	// and try to op
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

static void	game_rock_paper_scissors(std::string adverse_choice, Client *client)
{
	std::string my_choice = rand_rock_paper_scissors();
	if (adverse_choice == "ROCK")
	{
		if (my_choice == "ROCK")
			no_winner(my_choice, client);
		else if (my_choice == "PAPER")
			you_lose(my_choice, client);
		else
			you_win(my_choice, client);
	}
	else if (adverse_choice == "PAPER")
	{
		if (my_choice == "ROCK")
			you_win(my_choice, client);
		else if (my_choice == "PAPER")
			no_winner(my_choice, client);
		else
			you_lose(my_choice, client);
	}
	else if (adverse_choice == "SCISSORS")
	{
		if (my_choice == "ROCK")
			you_lose(my_choice, client);
		else if (my_choice == "PAPER")
			you_win(my_choice, client);
		else
			no_winner(my_choice, client);
	}
	else if (adverse_choice == "WELL")
		you_lose("PAPER", client);
	else
		explain_rule(client);
	return ;
}

void Bot::handleBot(Server &serv, Client *client, std::vector<std::string> *args)
{
	if (args->size() != 1)
		explain_rule(client);
	else
		game_rock_paper_scissors(Server::str_toupper(args->at(0)), client);

	(void)serv;
	// (void)client;
}
