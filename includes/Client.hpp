/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibjean-b <ibjean-b@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-03-27 16:03:22 by ibjean-b          #+#    #+#             */
/*   Updated: 2025-03-27 16:03:22 by ibjean-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <map>

class Client
{
private:
	int			_fd;
	bool		_auth;
	bool		_pwd;
	std::string	_user;
	std::string	_nick;
public:
	~Client();
	Client(int fd);

	//SETTERS AND GETTERS
	void	setFd(int fd);
	void	setUser(std::string user);
	void	setNick(std::string nick);
	void	setAuth(bool auth);
	void	setPwd(bool pwd);

	bool		getAuth(void);
	bool		getPwd(void);
	int			getFd(void);
	std::string	getUser(void);
	std::string	getNick(void);
};

std::ostream &	operator<<(std::ostream &o, Client &client);

#endif