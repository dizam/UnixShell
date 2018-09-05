#include "file.h"
#include <cstring>

File::File()
{
  name = NULL;
  permissions.set(0666, NULL);  
} //File()

File::File(File const &rhs) : permissions(rhs.permissions)
{
  if (rhs.name)
  {
    name = new char[strlen(rhs.name) + 1];
    strcpy(name, rhs.name);
  } //if non-null name from source
} //File copy constructor

File::File(const char *nam, Permissions perm) : permissions(perm)
{
  if (nam)
  {
    name = new char[strlen(nam) + 1];
    strcpy(name, nam);
  } //if non-null name from parameter 
} //File standard constructor

File::~File()
{
  delete[] name; 
} //~File()

bool File::operator== (const File &rhs) const
{
  return strcmp(name, rhs.name) == 0;
} // operator==()

bool File::operator< (const File &rhs) const
{
  return strcmp(name, rhs.name) < 0;
} //operator<()

void File::ls(bool isLongFormat) const
{
  if (isLongFormat)
  {
    permissions.print();
    cout << " ";
    time.print();
    cout << name;
    cout << "\n";
  } //if ls -l
  else //just ls
  {
    cout << name << " ";
  } //just ls
} //ls()

void File::updateTime()
{
  time.update();
} //updateTime()

const Permissions File::getPermissions() const
{
  return permissions;
} //getPermissions()

void File::touch()
{
  updateTime();
} //touch()

void File::write(ostream &os) const
{
  os << 'F' << " ";
  os << name << " " << time << " " << permissions << "\n"; 
} //write()

ostream& operator<< (ostream &os, File &rhs)
{
  rhs.write(os);
  return os;
}  // operator<<  

void File::read(istream &is, File &rhs)
{
  is >> rhs.name >> rhs.time >> rhs.permissions;  
} //read()

istream& operator>> (istream &is, File &rhs)
{
  rhs.read(is, rhs);
  return is;
} // operator>>
