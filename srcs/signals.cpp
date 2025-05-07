/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:46:28 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/05/07 20:13:02 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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
