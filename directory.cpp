
// Author: Sean Davis
#include <cstring>
#include <iostream>
#include "directory.h"
#include "permissions.h"
#include "list.h"

using namespace std;

Directory::Directory(const char *nam, Directory *paren, const char *owner)
  : File(nam, Permissions(0777, owner)), subDirectoryCount(0), parent(paren) 
 {

 }  // Directory())


Directory::Directory(Directory const &rhs) :
  File(rhs.name, Permissions(rhs.permissions)), 
  subDirectoryCount(rhs.subDirectoryCount),  
  parent(rhs.parent)
{
  for (int i = 0; i < subDirectoryCount; i++)
  {
    const Directory *dir =
 dynamic_cast<const Directory*>(rhs.subDirectories[i]);

    if (dir != NULL)
    {
      Directory *temp = new Directory(*dir);
      temp->parent = this;
      subDirectories += temp;
    } //if it's a directory
    else //it's a file
    {
      File *fil = new File(*rhs.subDirectories[i]);
      subDirectories += fil;
      //subDirectories += rhs.subDirectories[i];
    } //not a directory
  } //for each subdirectory
}   // Directory copy constructor


Directory::~Directory()
{
  // delete [] name;
}  // ~Directory()

Directory* Directory::cdReduce()
{
  if (parent) //if not root directory
    return parent;
  return this;
} //cdReduce()

Directory* Directory::cd(int argCount, const char *arguments[], 
                         const char *user)
{
  if (argCount != 2)
  {
    cout << "usage: cd directoryName\n";
    return this;
  }  // if two many arguments 

  if (strcmp(arguments[1], "..") == 0)
  {
    return cdReduce();
  }  // if cd ..

  for (int i = 0; i < subDirectoryCount; i++)
  {
    if (*subDirectories[i] == Directory(arguments[1]))
    {
      if (Directory *dir = dynamic_cast<Directory *>(subDirectories[i]))   
        if (subDirectories[i]->getPermissions()
            .isPermitted(EXECUTE_PERMISSIONS, user))
          return dir;
        else  // subdirectory doesn't have execute permissions
        {
          cout << arguments[1] << ": Permission denied.\n";
          return this;
        } //no execute permissions
      else //else it's a file
      {
        cout << arguments[1] << ": Not a directory.\n";
        return this;
      } //not a directory
    }  // if found the named directory 
  }  // for each subdirectory
 
  cout << "cd: " << arguments[1] << ": No such file or directory\n";
  return this;
}  // cd()


bool Directory:: iscpCorrectFormat(int argCount, const char *arguments[])
{
  if (argCount != 3)
  {
    if (argCount == 1)
      cout << "cp: missing operand\n";
    else   // more than one argument, but not 3
      if (argCount == 2)
        cout << "cp: missing destination file operand after ‘" << arguments[1]
             << "’\n";
      else  // more than 3 arguments
        cout << "Too many operands\n";

    cout << "Try 'cp --help' for more information.\n";
    return false;
  }  // if incorrect number of operands

  if (strcmp(arguments[1], arguments[2]) == 0)
  {
    cout << "cp: ‘" << arguments[1] << "’ and ‘" << arguments[2]
      << "’ are the same file\n";
    return false;
  }  // if source and destination have the same name

  for (int i = 0; i < subDirectoryCount; i++)
    if (*subDirectories[i] == Directory(arguments[2]))
    {
      cout << "cp: '" << arguments[2] << "' already exists.\n";
      return false;
    }  // if the destination directory already exists

  return true;
}  //  iscpCorrectFormat()

void Directory::printMod(int argCount, const char *arguments[])
{
  if (argCount == 1)
    cout << "chmod: missing operand\n";
  else   // argCount == 2
    cout << "chmod: missing operand after ‘" << arguments[1] << "’\n";

    cout << "Try 'chmod --help' for more information.\n"; 
} //printMod()


void Directory::chmod(int argCount, const char *arguments[], const char *user)
 // changes the permissions of the arguments
{
  if (argCount < 3) 
  {
    printMod(argCount, arguments);
    return; 
  }// if wrong number of arguments

  short newPermissions = Permissions::checkOctals(arguments[1]);
  int j;
  
  if (newPermissions < 0)
  {
    cout << "chmod: invalid mode: ‘" << arguments[1] << "’\n";
    cout << "Try 'chmod --help' for more information.\n";
    return;
  }  // if invalid permissions
  
  for (int i = 2; i < argCount; i++)
  {
    for (j = 0; j < subDirectoryCount; j++)
      if (Directory(arguments[i]) == *subDirectories[j])
      {
        Directory *dir = dynamic_cast<Directory *>(subDirectories[j]);

        if (dir != NULL);
        {
          dir->permissions.chmod(newPermissions, user);
          break;
        } //if it's a directory
      }  // if matched name of directory with argument
    
    if (j == subDirectoryCount)
      cout << "chmod: cannot access ‘" << arguments[i] 
           << "’: No such file or directory\n";
  }  // for each filename
}  // chmod()

void Directory::chown(int argCount, const char *arguments[])
{
  int j;
  
  if (argCount != 3)
  {
    if (argCount == 1)
      cout << "chown: missing operand\n";
    else  // 2 or more than 3 arguments
      if (argCount == 2)
        cout << "chown: missing operand after ‘" << arguments[1] << "’\n";
      else  // more than 3 arguments
        cout << "chown: Too many arguments.\n";

    cout << "Try 'chown --help' for more information.\n";
    return;
  }  // if wrong number of arguments
  
  for (j = 0; j < subDirectoryCount; j++)
     if (Directory(arguments[2]) == *subDirectories[j])
      {
        Directory *dir = dynamic_cast<Directory *>(subDirectories[j]);

        if (dir != NULL)
        {
          dir->permissions.chown(arguments[1]);
          break;
        } //if it's a directory
      }  // if matched name of directory with argument
    
  if (j == subDirectoryCount)
    cout << "chown: cannot access ‘" << arguments[2] 
         << "’: No such file or directory\n";
}  // chown()

void Directory::printCpPerm(const char* arguments[])
{
  cout << "cp: cannot open ‘" << arguments[1] 
       << "’ for reading: Permission denied\n";
} //printCpPerm()


void Directory::printCpMiss(const char* arguments[])
{
  cout << "cp: cannot stat ‘" << arguments[1] 
       << "’: No such file or directory\n";
  cout << "Try 'cp --help' for more information.\n";
}//printCpMiss()


void Directory::cp(int argCount, const char *arguments[], const char *user)
{
  if (!  iscpCorrectFormat(argCount, arguments))
    return;

  for (int i = 0; i < subDirectoryCount; i++)
  {    
    if (*subDirectories[i] == Directory(arguments[1]))
    {
      if (!subDirectories[i]->getPermissions()
          .isPermitted(READ_PERMISSIONS, user ))
      {
        printCpPerm(arguments);
        return;
      }  // if not allowed to read

      if (Directory *dir = dynamic_cast<Directory *>(subDirectories[i]))
      {
        Directory *destinationDirectory = new Directory(*dir);
        delete [] destinationDirectory->name;
        destinationDirectory->name = new char[strlen(arguments[2]) + 1];
        strcpy(destinationDirectory->name, arguments[2]);
        subDirectories += destinationDirectory;
      } //if it's a directory
      else //else it's a file
      {
        File *fil = new File(arguments[2], subDirectories[i]->getPermissions());
        subDirectories += fil;
      } //not a directory

      subDirectoryCount++;
      return;
    } //if subdirectory exists
  } //for each subdirectory

  printCpMiss(arguments);
}  // cp())


void Directory::ls(int argCount, const char *arguments[], const char *user)const
{
  if (argCount > 2 || (argCount == 2 && strcmp(arguments[1], "-l") != 0))
    cout << "usage: ls [-l]\n";
  else  // correct number of arguments
  {
    if (getPermissions().isPermitted(READ_PERMISSIONS, user))
    {
      if (argCount == 1)  // simple ls
      {

          for (int i = 0; i < subDirectoryCount; i++)
            subDirectories[i]->ls(0);
  
        cout << "\n";
      }  // if simple ls
      else  // must be ls -l
      {
        for (int i = 0; i < subDirectoryCount; i++)
        { 
          subDirectories[i]->ls(1);
        }  // for each subdirectory
      }  // else is ls -l
    }  // if have read permissions
    else // don't have read permissions
      cout << "ls: cannot open directory .: Permission denied\n";
  }  // else correct arguments
}  // ls()

void Directory::touchMiss()
{
  cout << "touch: missing operand\n";
  cout << "Try 'touch --help' for more information.\n";
} //touchMiss()

void Directory::touchUp(const char* arguments[], int argPos)
{
  cout << "touch: cannot update file ‘" << arguments[argPos] 
       <<  "’: Permission denied\n";
} //touchUp()

void Directory::touchPerm(const char* arguments[], int argPos)
{
  cout << "touch: cannot create file ‘" << arguments[argPos] 
       <<  "’: Permission denied\n";
} //touchPerm()

void Directory::touch(int argCount, const char *arguments[], const char *user)
{
  int i;
  
  if (argCount == 1)
  {
    touchMiss();
    return;
  } //if 1 argument (invalid)
  
  for (int argPos = 1; argPos < argCount; argPos++)
  {
    for (i = 0; i < subDirectoryCount; i++)
      if (*subDirectories[i] == Directory(arguments[argPos]))
      {

        if (getPermissions().isPermitted(WRITE_PERMISSIONS, user))
        {
          subDirectories[i]->touch();
          break;
        } //if user has write permissions

        touchUp(arguments, argPos);
      } //for each subdirectory

    if (i == subDirectoryCount)
    {
      if (getPermissions().isPermitted(WRITE_PERMISSIONS, user))
      {
        subDirectories += new File(arguments[argPos], Permissions(0666, user));
        subDirectoryCount++;
      } //if user has write permissions
      else //else no write permissions
        touchPerm(arguments, argPos);
    } //file wasn't found
  } //for each argument
} //touch()

void Directory::mkdir(int argCount, const char *arguments[], const char *user)
{
  int i;
  
  if (argCount == 1)
  {
    cout << "mkdir: missing operand\n";
    cout << "Try 'mkdir --help' for more information.\n";
    return;
  }  // if too many arguments
   
  for (int argPos = 1; argPos < argCount; argPos++)
  {
    for (i = 0; i < subDirectoryCount; i++)
      if (*subDirectories[i] == Directory(arguments[argPos]))
      {
        cout << "mkdir: cannot create directory ‘" << arguments[argPos] 
             << "’: File exists\n"; 
        break;
      }  // if subdirectory already exists.
    
    if (i == subDirectoryCount)
    {
      if (permissions.isPermitted(WRITE_PERMISSIONS, user))
      {
        subDirectories += new Directory(arguments[argPos], this, user);
        updateTime();
        subDirectoryCount++;
      }  // if have write permissions
      else // no write permissions
        cout << "mkdir: cannot create directory ‘" << arguments[argPos] 
             <<  "’: Permission denied\n";
    }  // if name not found
  }  // for each filename
}  // mkdir()


void Directory::showPath() const
{
  if (parent == NULL)
  {
    cout << "/";
    return;
  }  // at root
  
  parent->showPath();
  cout << name << "/";
}  // showPath())


void Directory::write(ostream &os) const
{
  os << 'D';
  File::write(os);
  os <<  subDirectoryCount << "\n";

  for (int i = 0; i < subDirectoryCount; i++)
  {
    subDirectories[i]->write(os);
  } //for each subdirectory
} //write ()

void Directory::read(istream &is, File &rhs)
{
  File::read(is, rhs);
  is.ignore(10, '\n');
  is >> subDirectoryCount;
  is.ignore(10, '\n');

  for (int i = 0; i < subDirectoryCount; i++)
  {
    char a;
    is >> a;

    if (a == 'F')
    {
      is.ignore(3, ' ');
      File *fil = new File(rhs);
      File::read(is, *fil);
      subDirectories += fil;
      is.ignore(10, '\n');
    } //if it's a file
    else //else it's a directory
    {
      is.ignore(3, ' '); 
      Directory *dir = new Directory("Dummy", this);
      is >> *dir;
      subDirectories += dir;
    } //it's a directory
  } //for each subdirectory
} //read ()
