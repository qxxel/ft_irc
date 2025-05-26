/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 14:50:56 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/05/26 17:25:03 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

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

#endif
