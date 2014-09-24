#include <iostream>
#include <iomanip>

#include "StoreNet.h"

#include "Variables.h"
#include "lps.h"

#include "Bounds.h"

#define DEBUG

using namespace CamiProdRead;

void Bounds::ComputePlace(unsigned int i, int &store, vector<unsigned int> &recomp)
{
	bool first = true, redo = false;
	unsigned int cpt = 0;

	while (first || redo)
	{
		first = false;
		lp.mysolve();
#ifdef DEBUG
		std::cout << p[i]->get_name() << " : ";
#endif

		if (lp.is_optimal())
		{
			REAL res = lp.get_obj();

			store = int(res+0.5);
			redo = false;
			cpt = 0;
#ifdef DEBUG
			std::cout << setprecision(20) << res << " (" << store << ")";
#endif
		}
		else if (lp.is_unbounded())
		{
			store = INFINITE;
			redo = false;
			cpt = 0;
#ifdef DEBUG
			std::cout << "oo";
#endif
		}
//		else if (!redo)
		else if (cpt < MAXITER)
		{
//			lp.set_default_basis();
			redo = true;
			++cpt;
#ifdef DEBUG
			std::cout << "on recommence";
#endif

			if ((!lp.is_sub_optimal()) && (!lp.is_time_out()))
				lp.set_default_basis();
			else
			{
				lpconfig = (lpconfig + 1) % NBCONFIG;
				lp.set_config(lpconfig);
#ifdef DEBUG
				std::cout << " (temps dépassé)";
			}
#endif
		}
		else
		{
			store = NOBOUND;
			redo = false;
			recomp.push_back(i);
#ifdef DEBUG
			std::cout << "on n'a pas pu calculer";
#endif
		}

#ifdef DEBUG
		std::cout << std::endl;
#endif
	}
}

Bounds::Bounds(StoreNet &n, bool is_integer): N(n), stEq(), p(n.NbPlaces()), t(n.NbTransitions()), lp(n.NbPlaces(), n.NbPlaces() + n.NbTransitions()),
							minBounds(n.NbPlaces()), maxBounds(n.NbPlaces()), lpconfig(1)
{
	unsigned int cpt = 0;

	/* Création des variables. */

	for (StoreNet::Elements::const_iterator itp = N.Places.begin(); itp != N.Places.end(); ++itp)
	{
		p[cpt] = new Variable((*itp)->GetName());
		p[cpt]->set_id(cpt+1);
		++cpt;
	}

	cpt = 0;

	for (StoreNet::Elements::const_iterator itt = N.Transitions.begin(); itt != N.Transitions.end(); ++itt)
	{
		t[cpt] = (new Variable((*itt)->GetName()));
		(*itt)->SetInfo(cpt);
		t[cpt]->set_id(cpt+N.NbPlaces()+1);
		++cpt;
	}

	/* Création des équations d'état */

	unsigned int cptp = 0;

	for (StoreNet::Elements::const_iterator itp = N.Places.begin(); itp != N.Places.end(); ++itp)
	{
		LinearExpr l = *(p[cptp])*1;

		for (ArcsIterator itt = (*itp)->EnumerateArcs.begin(); itt != (*itp)->EnumerateArcs.end(); ++itt)
		{
			int w = itt->GetIn() - itt->GetOut();

			if (w != 0)
				l = l - w*(*t[itt->GetNode()->GetInfo()]);
		}

		stEq + (l = ((Place *)(*itp))->GetInitial());
		++cptp;
	}

	/* Analyse */

	lp.translate_syst(stEq);

	if (is_integer)
		lp.set_integer();

	lp.set_config(lpconfig);
	lp.set_my_timeout(LP_TIMEOUT);

	vector<unsigned int> sol(N.NbPlaces() + N.NbTransitions());

  for (unsigned int i = 0; i < N.NbPlaces(); ++i)
		sol[p[i]->get_id()-1] = ((Place *)N.Places[i])->GetInitial();

	for (unsigned int i = 0; i < N.NbTransitions(); ++i)
		sol[t[i]->get_id()-1] = 0;

	lp.compute_basis(sol);

	for (unsigned int i = 0; i < N.NbPlaces(); ++i)
	{
		int store;

		lp.translate_obj((*p[i])*1);
		lp.set_min();
		ComputePlace(i, store, minRecomp);
		minBounds[i] = store;

		if (store > 0)
			lp.set_lower_bound((*p[i]), store);

		lp.set_max();
		ComputePlace(i, store, maxRecomp);
		maxBounds[i] = store;

		if (store > 0)
			lp.set_upper_bound((*p[i]), store);
	}

	/* On essaye de recalculer ceux qui ont échoués */

	vector<unsigned int> tmp;
	bool				useful = true;

	while (useful)
	{
		useful = false;
#ifdef DEBUG
		std::cout << "On retente un calcul" << std::endl;
#endif

		for (unsigned int i = 0; i < minRecomp.size(); ++i)
		{
			int store;
			unsigned int pl = minRecomp[i];

			lp.translate_obj((*p[pl])*1);
			lp.set_min();
			ComputePlace(pl, store, tmp);
			minBounds[pl] = store;

			if (store > 0)
				lp.set_lower_bound((*p[pl]), store);
		}

		if (tmp.size() > 0)
		{
			useful = tmp.size() < minRecomp.size();
			minRecomp = tmp;
		}
		else
		{
			useful = (minRecomp.size() > 0);
			minRecomp.clear();
		}

		tmp.clear();

		for (unsigned int i = 0; i < maxRecomp.size(); ++i)
		{
			int store;
			unsigned int pl = maxRecomp[i];

			lp.translate_obj((*p[pl])*1);
			lp.set_max();
			ComputePlace(pl, store, tmp);
			maxBounds[pl] = store;

			if (store > 0)
				lp.set_upper_bound((*p[pl]), store);
		}

		if (tmp.size() > 0)
		{
			useful = (useful) || (tmp.size() < maxRecomp.size());
			maxRecomp = tmp;
		}
		else
		{
			maxRecomp.clear();
			useful = (minRecomp.size() > 0);
		}

		tmp.clear();
	}
}

Bounds::~Bounds()
{
	for (unsigned int i = 0; i < p.size(); ++i)
	{
		delete(p.back());
		p.pop_back();
	}

	for (unsigned int i = 0; i < t.size(); ++i)
	{
		delete(t.back());
		t.pop_back();
	}
}

int Bounds::get_bound_max(unsigned int i) const
{
	return maxBounds[i];
}

int Bounds::get_bound_min(unsigned int i) const
{
	return minBounds[i];
}

StoreNet &Bounds::get_net()
{
	return N;
}


std::ostream &operator<<(std::ostream &os, Bounds &bo)
{
	StoreNet &N = bo.get_net();
	unsigned int nbPl = N.NbPlaces();

	for (unsigned int i = 0; i < nbPl; ++i)
	{
		std::cout << N.Places[i]->GetName() << ": [";

		int b = bo.get_bound_min(i);

		if (b >= 0)
			std::cout << b;
		else if (b == INFINITE)
		{
			std::cout << "Internal error: cannot find an inifinite minimal bound" << std::endl;
			exit(1);
		}
		else if (b == NOBOUND)
			std::cout << "Cannot compute the bound";
		else
		{
			std::cout << "Internal error: unknown bound code" << std::endl;
			exit(1);
		}

		b = bo.get_bound_max(i);
		std::cout << "...";

		if (b >= 0)
			std::cout << b;
		else if (b == INFINITE)
			std::cout << "oo";
		else if (b == NOBOUND)
			std::cout << "Cannot compute the bound";
		else
		{
			std::cout << "Internal error: unknown bound code" << std::endl;
			exit(1);
		}

		std::cout << ']' << std::endl;
	}

	return os;
}
