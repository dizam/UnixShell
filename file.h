#ifndef FILE_H
  #define FILE_H

#include "Time.h"
#include "permissions.h"

class File
{
private:
  Time time;
protected:
  char *name;
  Permissions permissions;
  void updateTime();
public:
  File();
  File(File const &rhs);
  File(const char *nam, Permissions perm);
  virtual ~File();
  bool operator== (const File &rhs) const;
  bool operator< (const File &rhs) const;
  void ls(bool isLongFormat) const;
  const Permissions getPermissions() const;
  void touch();
  friend ostream& operator<< (ostream &os, File &rhs);
  friend istream& operator>> (istream &is, File &rhs);
  virtual void write(ostream &os) const;
  virtual void read(istream &is, File &rhs);
}; //class File()

#endif  
