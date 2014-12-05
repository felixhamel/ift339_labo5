//
//  ensemble.h
//
//  Jean Goulet  14-11-26.
//  Copyleft 2014 UdeS
//

#ifndef Hash_ensemble_h
#define Hash_ensemble_h

#include <iostream>
#include <list>
#include <vector>
#include <cassert>

////////////////////////////////////////////////////////////////
// definition des classes

template <typename TYPE>
class ensemble_sans_ordre{
public:
    class iterator;
    friend class iterator;

    ensemble_sans_ordre();
    ensemble_sans_ordre(const ensemble_sans_ordre&);
    ensemble_sans_ordre& operator=(const ensemble_sans_ordre&);
    ~ensemble_sans_ordre();
    void swap(ensemble_sans_ordre&);

    size_t size()const{return SIZE;}
    bool empty()const{return SIZE==0;}
    void clear();
    iterator find(const TYPE&);

    void insert(const TYPE&);
    void erase(const TYPE&);
    void erase(iterator);

    iterator begin();
    iterator end();

    void afficher()const;
private:
    std::vector<std::list<TYPE*>> REP;
    size_t SIZE;
    float FACTEUR_DE_CHARGE;  //nombre moyen maximal d'elements par alveole

    void rehash();
};

template <typename TYPE>
class ensemble_sans_ordre<TYPE>::iterator{
    friend class ensemble_sans_ordre<TYPE>;
private:
    ensemble_sans_ordre<TYPE>* ENS;
    size_t ALV;
    typename std::list<TYPE*>::iterator POS;
    void avancer();
    void reculer();
public:
    iterator();
    iterator(ensemble_sans_ordre<TYPE>*,size_t,typename std::list<TYPE*>::iterator);
    iterator& operator++(){avancer();return *this;}                         //++i
    iterator operator++(int){iterator copie(*this);avancer();return copie;} //i++
    iterator& operator--(){reculer();return *this;}                         //--i
    iterator operator--(int){iterator copie(*this);reculer();return copie;} //i--
    const TYPE& operator*()const{return **POS;}
    bool operator==(const iterator&)const;
    bool operator!=(const iterator&)const;
};

/*template <typename TYPE>
struct std::hash<TYPE> {
  size_t operator()(const TYPE &object) const{
    return object;
  }
};*/

////////////////////////////////////////////////////////////////
// definition des fonctions de la classe ensemble_sans_ordre

//constructeurs, destructeurs, affectateur

template <typename TYPE>
ensemble_sans_ordre<TYPE>::ensemble_sans_ordre()
{
  SIZE = 0;
  FACTEUR_DE_CHARGE = 1.0;
}

template <typename TYPE>
ensemble_sans_ordre<TYPE>::ensemble_sans_ordre(const ensemble_sans_ordre<TYPE>& source)
{
  REP.resize(source.REP.size());
  SIZE=source.SIZE;
  FACTEUR_DE_CHARGE=source.FACTEUR_DE_CHARGE;
  for(size_t i=0;i<REP.size();++i)
    for(auto j=source.REP[i].begin();j!=source.REP[i].end();++j)
      REP[i].push_back(new TYPE(**j));
}

template <typename TYPE>
ensemble_sans_ordre<TYPE>& ensemble_sans_ordre<TYPE>::operator=(const ensemble_sans_ordre<TYPE>& source)
{
  auto copie(source);
  swap(copie);
  return *this;
}

template <typename TYPE>
ensemble_sans_ordre<TYPE>::~ensemble_sans_ordre()
{
  clear();
}

template <typename TYPE>
void ensemble_sans_ordre<TYPE>::swap(ensemble_sans_ordre<TYPE>& droite)
{
  REP.swap(droite.REP);
  std::swap(SIZE,droite.SIZE);
  std::swap(FACTEUR_DE_CHARGE,droite.FACTEUR_DE_CHARGE);
}


//autres fonctions membres de la classe

template <typename TYPE>
void ensemble_sans_ordre<TYPE>::clear()
{
  //liberer l'espace de tous les elements
  for(auto i = begin(); i != end(); ++i) {
    delete *(i.POS);
  }
  REP.clear();
  SIZE = 0;
  FACTEUR_DE_CHARGE = 1.0;
}

template <typename TYPE>
typename ensemble_sans_ordre<TYPE>::iterator
ensemble_sans_ordre<TYPE>::find(const TYPE& x)
{
  typename ensemble_sans_ordre<TYPE>::iterator retour = end();
  if(empty())return retour;
  auto disperseur = std::hash<TYPE>();
  size_t alv = disperseur(x) % REP.size();
  std::list<TYPE*>& alveole = REP[alv];
  auto i = alveole.begin();
  for(; i != alveole.end(); ++i) {
    if(**i == x) {
      retour.ALV = alv;
      retour.POS = i;
      break;
    }
  }
  return retour;
}

//fonctions generatrices insert et erase

template <typename TYPE>
void ensemble_sans_ordre<TYPE>::insert(const TYPE& x)
{
  rehash(); //agrandir le conteneur au besoin
  auto disperseur = std::hash<TYPE>();
  std::list<TYPE*>& alveole = REP[disperseur(x) % REP.size()];
  auto i = alveole.begin();
  for(; i != alveole.end();++i) {
    if(**i == x)return;
  }
  alveole.push_back(new TYPE(x));
  ++SIZE;
}

template <typename TYPE>
void ensemble_sans_ordre<TYPE>::erase(const TYPE& x)
{
  typename ensemble_sans_ordre<TYPE>::iterator it = find(x);
  if(it != end()) {
    erase(it);
  }
}

template <typename TYPE>
void ensemble_sans_ordre<TYPE>::erase(typename ensemble_sans_ordre<TYPE>::iterator it)
{
  // A IMPLANTER!
  //it.ENS[it.ALV].erase(it);
  //SIZE--;
}


// gestion de l'iteration

template <typename TYPE>
typename ensemble_sans_ordre<TYPE>::iterator ensemble_sans_ordre<TYPE>::begin()
{
  ensemble_sans_ordre<TYPE>::iterator retour;
  retour.ENS = this;
  typename std::list<TYPE*>::iterator P;
  for(retour.ALV=0;retour.ALV<REP.size();++(retour.ALV)) {
    if(!REP[retour.ALV].empty()) {
      retour.POS=REP[retour.ALV].begin();
      return retour;
    }
  }
  return retour;
}

template <typename TYPE>
typename ensemble_sans_ordre<TYPE>::iterator ensemble_sans_ordre<TYPE>::end()
{
  ensemble_sans_ordre<TYPE>::iterator retour;
  retour.ENS=this;
  retour.ALV=REP.size();
  return retour;
}


////////////////////////////////////////////////////////////////
// rehash
// si le facteur de charge est depasse, doubler la dimension
// du tableau d'adressage disperse et replacer tous les
// elements

template <typename TYPE>
void ensemble_sans_ordre<TYPE>::rehash()
{
  if(SIZE < FACTEUR_DE_CHARGE * REP.size()) return;
  if(REP.size() == 0) { REP.resize(1); return; }

  // Calculer le taux de charge moyen de toutes les alvéoles
  float charge_moyenne = 0.f;
  for(auto j = REP.begin(); j != REP.end(); ++j) {
    charge_moyenne += j->size();
  }
  charge_moyenne /= REP.size();

  /*
    Si la charge moyenne de toutes les alvéoles réunies est > que
    le facteur de charge, alors on va créer un nouveau vecteur avec un
    plus grand nombre d'alvéole et mettre les données du vecteur actuel
    dans le nouveau vecteur en utilisant le hashing pour trouver la meilleure
    alvéole placer la donnée.
   */
  if(charge_moyenne > FACTEUR_DE_CHARGE) {

    // Notre nouveau vecteur
    std::vector<std::list<TYPE*>> nouveau_vecteur;
    nouveau_vecteur.resize(FACTEUR_DE_CHARGE * REP.size() + 1);

    std::hash<TYPE*> hasheur;

    // Aller chercher tous les éléments de l'ancien vecteyr REP et
    // aller les mettre dans le nouveau.
    for(auto alveolIt = REP.begin(); alveolIt != REP.end(); ++alveolIt) {
      for(auto it = alveolIt->begin(); it != alveolIt->end(); ++it) {
        size_t indice = hasheur(*it);
        nouveau_vecteur[indice % nouveau_vecteur.size()].push_back(*it);
      }
    }
    REP = nouveau_vecteur;
  }
}


////////////////////////////////////////////////////////////////
// definition des fonctions de la classe iterator

template <typename TYPE>
ensemble_sans_ordre<TYPE>::iterator::iterator(){}

template <typename TYPE>
ensemble_sans_ordre<TYPE>::iterator::iterator(ensemble_sans_ordre<TYPE>* E,size_t A,typename std::list<TYPE*>::iterator P)
    :ENS(E),ALV(A),POS(P){}

template <typename TYPE>
void ensemble_sans_ordre<TYPE>::iterator::avancer()
{
  assert(ALV<ENS->REP.size());
  ++POS;
  if(POS!=ENS->REP[ALV].end())return;
  for(++ALV;ALV<ENS->REP.size();++ALV){
    POS=ENS->REP[ALV].begin();
    if(POS!=ENS->REP[ALV].end())return;
  }
}

template <typename TYPE>
void ensemble_sans_ordre<TYPE>::iterator::reculer()
{
  //A IMPLANTER!
}

template <typename TYPE>
bool ensemble_sans_ordre<TYPE>::iterator::operator==(const iterator& droite)const
{
  assert(ENS==droite.ENS);
  if(ALV==droite.ALV)return true;
  if(ALV==ENS->REP.size())return true;
  return POS==droite.POS;
}

template <typename TYPE>
bool ensemble_sans_ordre<TYPE>::iterator::operator!=(const iterator& droite)const
{
  assert(ENS==droite.ENS);
  if(ALV!=droite.ALV)return true;
  if(ALV==ENS->REP.size())return false;
  return POS!=droite.POS;
}

////////////////////////////////////////////////////////////////
// fonction d'affichage pour mise au point

template <typename TYPE>
void ensemble_sans_ordre<TYPE>::afficher()const
{
  using namespace std;
  cout << "-------------------------------" << endl;
  cout << "ensemble_sans_ordre a l'adresse " << this << endl;
  cout << SIZE << " elements dans " << REP.size() << " alveoles" << endl;
  for(size_t i = 0; i != REP.size(); ++i)
  {
    cout << i << ": ";
    for(auto j=REP[i].begin(); j != REP[i].end(); ++j) {
      cout << (**j) << " ";
    }
    cout<<endl;
  }
  cout << "-------------------------------" << endl;
}

#endif
