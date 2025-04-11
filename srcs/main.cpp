/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibjean-b <ibjean-b@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-03-26 15:16:26 by ibjean-b          #+#    #+#             */
/*   Updated: 2025-03-26 15:16:26 by ibjean-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"

void	setupSignals();

int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (0);
	}
	try
	{
		setupSignals();
		Server	ircserv = Server(argv[1], argv[2]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
