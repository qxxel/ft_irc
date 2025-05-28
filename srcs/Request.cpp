/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 14:50:48 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/05/28 18:13:14 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Request.hpp"

Request::~Request() { }

Request::Request(): _line(), _arr() { }

Request::Request(char *raw): _line(raw), _arr()
{
	this->split_Request();
}

void Request::split_Request()
{
	std::string	cmd_raw;
	std::size_t	del;

	while (!_line.empty() && (del = _line.find('\n')) != std::string::npos)
	{
		cmd_raw = _line.substr(0, del);
		Command	tp = Command(cmd_raw);
		std::cout << tp;
		_arr.push_back(tp);
		_line = _line.substr(del + 1, _line.size() - del);
	}
}

std::string	&Request::append(const char* s, size_t n)
{
	return _line.append(s, n);
}

void		Request::clear()
{
	_line.clear();
	_arr.clear();
}

std::string	Request::getLine()
{
	return _line;
}

std::vector<Command>	&Request::getArr()
{
	return (_arr);
}

void	Request::setArr(std::vector<Command> arr)
{
	_arr = arr;
}
