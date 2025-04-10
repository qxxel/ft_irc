/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibjean-b <ibjean-b@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-04-01 14:50:48 by ibjean-b          #+#    #+#             */
/*   Updated: 2025-04-01 14:50:48 by ibjean-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "defines.hpp"
#include <sys/wait.h>

Request::~Request()
{
}

Request::Request(char *raw) : _arr()
{
	std::string	str_raw(raw);
	std::string	cmd_raw;
	std::size_t	del;

	while (!str_raw.empty() && (del = str_raw.find('\n')) != std::string::npos)
	{
		cmd_raw = str_raw.substr(0, del + 1);
		Command	tp = Command(cmd_raw);
		std::cout << tp;
		_arr.push_back(tp);
		str_raw = str_raw.substr(del + 1, str_raw.size() - del);
	}
}

std::vector<Command>	&Request::getArr()
{
	return (_arr);
}

void	Request::setArr(std::vector<Command> arr)
{
	_arr = arr;
}