#include "ensemble.h"

#include <unordered_set>

using namespace std;

int main()
{
  ensemble_sans_ordre<int> e;

  for(int i = 0; i < 100; ++i) {
    e.insert(i);
  }

  //e.erase(50);

  e.afficher();

  /*std::unordered_set<int> list;
  list.max_load_factor(5);
  for(int i = 0; i < 500; ++i) {
    list.insert(i);
  }

  list.rehash(15);
  for(std::unordered_set<int>::iterator it = list.begin(); it != list.end(); ++it) {
    cout << *it << endl;
  }

  std::cout << "size = " << list.size() << std::endl;
  std::cout << "bucket_count = " << list.bucket_count() << std::endl;
  std::cout << "load_factor = " << list.load_factor() << std::endl;
  std::cout << "max_load_factor = " << list.max_load_factor() << std::endl;

  unsigned nbuckets = list.bucket_count();

  std::cout << "myset has " << nbuckets << " buckets:\n";

  for (unsigned i=0; i<nbuckets; ++i) {
    std::cout << "bucket #" << i << " has " << list.bucket_size(i) << " elements.\n";
  }*/


  return 0;
}
