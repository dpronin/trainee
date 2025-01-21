#include <boost/filesystem.hpp>
#include <iostream>

using namespace boost::filesystem;

int main()
{
  path p{"C:\\Windows\\System"};

#ifdef BOOST_WINDOWS_API
  std::wcout << p.native() << '\n';
#else
  std::cout << p.native() << '\n';
#endif

  std::cout << p.string() << '\n';
  std::wcout << p.wstring() << '\n';

  std::cout << p.generic_string() << '\n';
  std::wcout << p.generic_wstring() << '\n';

  path p1{"/home/dpronin/Development/private/project.txt"};
  std::cout << p1.root_name() << '\n';
  std::cout << p1.root_directory() << '\n';
  std::cout << p1.root_path() << '\n';
  std::cout << p1.relative_path() << '\n';
  std::cout << p1.parent_path() << '\n';
  std::cout << p1.filename() << '\n';
}
