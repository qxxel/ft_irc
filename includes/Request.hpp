/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 14:50:56 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/05/07 20:21:28 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <vector>
# include "defines.hpp"

class Command;

class Request
{
private:
	std::string				_line;
	std::vector<Command>	_arr;
public:
	~Request();
	Request();
	Request(char *raw);

	std::string				&append(const char* s, size_t n);
	void					clear();
	std::string				getLine();
	void 					split_Request();

	void					setArr(std::vector<Command> arr);
	std::vector<Command>	&getArr();
};
