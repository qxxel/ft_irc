/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibjean-b <ibjean-b@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-03-27 16:04:15 by ibjean-b          #+#    #+#             */
/*   Updated: 2025-03-27 16:04:15 by ibjean-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


Client::~Client()
{
}

Client::Client(int fd) : _fd(fd), _pwd(false), _user(""), _nick("")
{
}

void	Client::setFd(int fd)
{
	_fd = fd;
}

void	Client::setNick(std::string nick)
{
	_nick = nick;
}

void	Client::setUser(std::string user)
{
	_user = user;
}

void	Client::setPwd(bool trueOrFalse)
{
	_pwd = trueOrFalse;
}

int		Client::getFd(void)
{
	return (_fd);
}

bool	Client::getPwd(void)
{
	return (_pwd);
}

std::string	Client::getNick(void)
{
	return (_nick);
}

std::string	Client::getUser(void)
{
	return (_user);
}

std::ostream &	operator<<(std::ostream &o, Client &client)
{
	o << "-----------Client-----------\n";
	o << "user: " << client.getUser() << std::endl;
	o << "nick: " << client.getNick() << std::endl;
	o << "clfd: " << client.getFd() << std::endl;
	o << "pwd : " << client.getPwd() << std::endl;
	return (o);
}