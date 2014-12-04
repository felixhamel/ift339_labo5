#include "ensemble.h"

using namespace std;

int main()
{
  ensemble_sans_ordre<int> e;

  for(int i = 0; i < 90; ++i) {
    e.insert(i);
  }

  e.erase(50);

  e.afficher();

  return 0;
}
