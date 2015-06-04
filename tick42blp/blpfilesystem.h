/*
* BlpBridge: The Bloomberg v3 API Bridge for OpenMama
* Copyright (C) 2012 Tick42 Ltd.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
* 02110-1301 USA

*Distributed under the Boost Software License, Version 1.0.
*    (See accompanying file LICENSE_1_0.txt or copy at
*         http://www.boost.org/LICENSE_1_0.txt)

*/

#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__
/*
 * File system is just a thin wrapper above common libraries like boost/filesystem
 */

#include <string>


/**
 * @param: any relative path
 * @return: absolute path
 */
std::string absolute_path(std::string path);

/**
 * @param path: any path to file or directory
 * @return: if path exists
 */
bool path_exists(std::string path);


/**
 * @param path: any path
 * @return: if path is relative
 */
bool has_relative_path(std::string path);
/**
 * @param path: any path
 * @return: if path is complete
 */
bool has_complete_path(std::string path);
/**
 * @param path_prefix: left side of the path (e.g. the branch)
 * @param path_suffix: right side of the path (e.g. file name)
 * @return: complete path
 */
std::string complete_path(const std::string &path_prefix, const std::string &path_suffix);
#endif