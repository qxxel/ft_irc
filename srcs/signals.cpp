/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibjean-b <ibjean-b@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-04-09 16:46:28 by ibjean-b          #+#    #+#             */
/*   Updated: 2025-04-09 16:46:28 by ibjean-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <csignal>

void	handleSignals(int signal)
{
	(void)signal;
	Server::exit();
}

void	setupSignals(void)
{
	signal(SIGINT, handleSignals);
	signal(SIGTERM, handleSignals);
	signal(SIGQUIT, handleSignals);
}