////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2010 - 2018, Göteborg Bit Factory.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// http://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
#include <iostream>
#include <stdlib.h>
#include <taskd.h>
#include <format.h>

////////////////////////////////////////////////////////////////////////////////
void command_init (Database& db)
{
  // Verify that root exists.
  auto root = db._config->get ("root");
  if (root == "")
    throw std::string ("ERROR: The '--data' option is required.");

  Directory root_dir (root);
  if (!root_dir.exists ())
    throw std::string ("ERROR: The '--data' path does not exist.");

  if (!root_dir.is_directory ())
    throw std::string ("ERROR: The '--data' path is not a directory.");

  if (!root_dir.readable ())
    throw std::string ("ERROR: The '--data' directory is not readable.");

  if (!root_dir.writable ())
    throw std::string ("ERROR: The '--data' directory is not writable.");

  if (!root_dir.executable ())
    throw std::string ("ERROR: The '--data' directory is not executable.");

  // Provide some defaults and overrides.
  db._config->set ("extensions", TASKD_EXTDIR);
  db._config->setIfBlank ("log",           "/tmp/taskd.log");
  db._config->setIfBlank ("queue.size",    "10");
  db._config->setIfBlank ("pid.file",      "/tmp/taskd.pid");
  db._config->setIfBlank ("ip.log",        "on");
  db._config->setIfBlank ("request.limit", "1048576");
  db._config->setIfBlank ("confirmation",  "1");
  db._config->setIfBlank ("verbose",       "1");
  db._config->setIfBlank ("trust",         "strict");

  // Suggestions for missing items.
  if (db._config->get ("server") == "")
    std::cout << "You must specify the 'server' variable before attempting a server start, for example:\n"
              << "  taskd config server localhost:53589\n"
              << '\n';

  // Create the data structure.
  Directory sub (root_dir);
  sub.cd ();
  sub += "orgs";
  if (!sub.exists ())
    if (!sub.create (0700))
      throw format ("ERROR: Could not create '{1}'.", sub._data);

  // Dump the config file?
  db._config->_original_file = root_dir._data + "/config";

  if (! db._config->_original_file.exists ())
    db._config->_original_file.create (0600);

  if (db._config->dirty ())
  {
    db._config->save ();
    if (db._config->getBoolean ("verbose"))
      std::cout << format ("Created {1}\n", db._config->_original_file._data);
  }
}

////////////////////////////////////////////////////////////////////////////////
