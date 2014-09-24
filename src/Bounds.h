#ifndef BOUNDS_H_INCLUDED
#define BOUNDS_H_INCLUDED

#include <vector>
#include <iostream>

#include "StoreNet.h"
#include "LinearSyst.h"
#include "lps.h"

#define INFINITE -1
#define NOBOUND -2

// Le nombre de seconde que peut durer un calcul pour une borne

#define LP_TIMEOUT 1

// Le nombre de fois qu'un caclul de bornes peut etre recommencé

#define MAXITER 7

class Bounds
{
private:

/*! \var StoreNet &N
	\brief Le réseau pour lequel on veut calculer les bornes. */

	StoreNet &N;

/*! \var LinearSyst stEq
	\brief Les équations d'état correspondant au réseau \a N. */

	LinearSyst stEq;

/*! \var vector<Variable *> p
	\brief Contient les variables représentant les places. */

	vector<Variable *> p;

/*! \var vector<Variable *> t
	\brief Contient les variables représentant les transitions. */

	vector<Variable *> t;

/*! \var lps lp
	\brief Le système de contraintes encapsulé. */

	lps lp;

/*! \var vector<int > minBounds
	\brief Stockage des bornes minimales. */

	vector<int > minBounds;

/*! \var vector<int > maxBounds
	\brief Stockage des bornes maximales. */

	vector<int > maxBounds;

/*! \var vector<unsigned int> minRecomp
	\brief Stockage des bornes minimales à recalculer. */

	vector<unsigned int> minRecomp;

/*! \var vector<unsigned int> maxRecomp
	\brief Stockage des bornes maximales à recalculer. */

	vector<unsigned int> maxRecomp;

/*! \var unsigned int lpconfig
	\brief La configuration actuellement utilisée par le solveur. */

	unsigned int lpconfig;

/*! \brief Effectue le calcul d'une borne (min ou max) pour une place. */

	void ComputePlace(unsigned int i, int &store, vector<unsigned int> &recomp);

public:
	Bounds(StoreNet &n, bool is_integer = true);
	~Bounds();

/*! \brief Optention de la borne maximale de la place de rang \a i.
	\param i Rang de la place dont on veut la borne dans la liste des places du réseau servant à l'initialisation de \ Bounds.
	\return La borne de la place. */

	int get_bound_max(unsigned int i) const;

/*! \brief Optention de la borne minimale de la place de rang \a i.
	\param i Rang de la place dont on veut la borne dans la liste des places du réseau servant à l'initialisation de \ Bounds.
	\return La borne de la place. */

	int get_bound_min(unsigned int i) const;

/*! \brief Optention du modèle étudié.
	\return Une référence sur le modèle étudié. */

	StoreNet &get_net();
};

std::ostream &operator<<(std::ostream &os, Bounds &bo);

#endif // BOUNDS_H_INCLUDED
