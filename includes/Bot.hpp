/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:48:51 by mreynaud          #+#    #+#             */
/*   Updated: 2025/05/02 15:53:44 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"

class Client;

class Bot : public Client
{
	private:

	public:
		Bot();
		~Bot();

	static void handleBot(Channel *channel, Client *client, std::string &arg);


	void	joinChannel(Channel	*channel);
	// void	kickChanel(Channel	*channel, Client *client, std::vector<std::string> *args);

};
