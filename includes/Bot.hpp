/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 17:48:51 by mreynaud          #+#    #+#             */
/*   Updated: 2025/04/30 18:17:27 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

class Bot : public Client
{
	private:

	public:
		Bot();
		~Bot();

	static void handleBot(Channel *channel, Client *client, std::string &arg);


	void	joinChanel(Channel	*channel);
	void	kickChanel(Channel	*channel, std::vector<std::string> *args);

};
