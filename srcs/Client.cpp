/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 16:04:15 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/10 22:17:41 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


Client::~Client()
{
}

Client::Client(int fd) : _fd(fd), _auth(false),  _pwd(false), _user(""), _nick("")
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

void	Client::setAuth(bool auth)
{
	_auth = auth;
}

void	Client::setPwd(bool pwd)
{
	_pwd = pwd;
}

void	Client::setCurrentChannel(Channel *channel)
{
	if (!channel)
		return ;
	this->_currentChannel = channel;
}

int		Client::getFd()
{
	return (_fd);
}

bool	Client::getAuth()
{
	return (_auth);
}

bool	Client::getPwd()
{
	return (_pwd);
}

std::string	Client::getNick()
{
	return (_nick);
}

std::string	Client::getUser()
{
	return (_user);
}

std::ostream &	operator<<(std::ostream &o, Client &client)
{
	o << "-----------Client-----------\n";
	o << "user: " << client.getUser() << std::endl;
	o << "nick: " << client.getNick() << std::endl;
	o << "clfd: " << client.getFd() << std::endl;
	o << "auth: " << client.getAuth() << std::endl;
	return (o);
}