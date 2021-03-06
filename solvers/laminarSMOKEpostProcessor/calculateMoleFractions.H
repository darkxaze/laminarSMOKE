/*-----------------------------------------------------------------------*\
|                                                                         |
|                    ╔═══╦═╗╔═╦═══╦╗╔═╦═══╗                               |
|                    ║╔═╗║║╚╝║║╔═╗║║║╔╣╔══╝                               | 
|   ╔╗╔══╦╗╔╦╦═╗╔══╦═╣╚══╣╔╗╔╗║║ ║║╚╝╝║╚══╗                               |
|   ║║║╔╗║╚╝╠╣╔╗╣╔╗║╔╩══╗║║║║║║║ ║║╔╗║║╔══╝                               |
|   ║╚╣╔╗║║║║║║║║╔╗║║║╚═╝║║║║║║╚═╝║║║╚╣╚══╗                               |
|   ╚═╩╝╚╩╩╩╩╩╝╚╩╝╚╩╝╚═══╩╝╚╝╚╩═══╩╝╚═╩═══╝                               |
|                                                                         |
|                                                                         |
|   Authors: A. Cuoci                                                     |
|                                                                         |
|   Contacts: Alberto Cuoci                                               |
|   email: alberto.cuoci@polimi.it                                        |
|   Department of Chemistry, Materials and Chemical Engineering           |
|   Politecnico di Milano                                                 |
|   P.zza Leonardo da Vinci 32, 20133 Milano (Italy)                      |
|                                                                         |
|-------------------------------------------------------------------------|
|                                                                         |
|   This file is part of laminarSMOKE solver.                             |
|                                                                         |
|   License                                                               |
|                                                                         |
|   Copyright(C) 2016, 2015, 2014 A. Cuoci                                |
|   laminarSMOKE is free software: you can redistribute it and/or modify  |
|   it under the terms of the GNU General Public License as published by  |
|   the Free Software Foundation, either version 3 of the License, or     |
|   (at your option) any later version.                                   |
|                                                                         |
|   laminarSMOKE is distributed in the hope that it will be useful,       |
|   but WITHOUT ANY WARRANTY; without even the implied warranty of        |
|   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         |
|   GNU General Public License for more details.                          |
|                                                                         |
|   You should have received a copy of the GNU General Public License     |
|   along with laminarSMOKE. If not, see <http://www.gnu.org/licenses/>.  |
|                                                                         |
\*-----------------------------------------------------------------------*/

// Post processing: mole fractions
if (calculateMoleFractions == true)
{
	label ns = Y.size();

	OpenSMOKE::OpenSMOKEVectorDouble y(ns);
	OpenSMOKE::OpenSMOKEVectorDouble x(ns);

	const scalarField& TCells = T.internalField();
	
	forAll(TCells, celli)
	{	
		double mw;

		// Extract the mean mass fractions
		for(unsigned int i=0;i<ns;i++)
			y[i+1] = Y[i].internalField()[celli];

		// Convert to mole fractions
		thermodynamicsMapXML->MoleFractions_From_MassFractions(x.GetHandle(), mw, y.GetHandle());

		#if OPENFOAM_VERSION >= 40
		for(unsigned int i=0;i<ns;i++)
			X[i].ref()[celli] = x[i+1];
		#else
		for(unsigned int i=0;i<ns;i++)
			X[i].internalField()[celli] = x[i+1];
		#endif
		
	}

	forAll(T.boundaryField(), patchi)
	{
		#if OPENFOAM_VERSION >= 40
		fvPatchScalarField& pT = T.boundaryFieldRef()[patchi];
		#else
		fvPatchScalarField& pT = T.boundaryField()[patchi];
		#endif
		

		forAll(pT, facei)
		{
			double mw;

			// Extract the mass fractions
			for(unsigned int i=1;i<=ns;i++)
				y[i] = Y[i-1].boundaryField()[patchi][facei];

			// Convert to mole fractions
			thermodynamicsMapXML->MoleFractions_From_MassFractions(x.GetHandle(), mw, y.GetHandle());

			#if OPENFOAM_VERSION >= 40
			for(unsigned int i=1;i<=ns;i++)
				X[i-1].boundaryFieldRef()[patchi][facei] = x[i];
			#else
			for(unsigned int i=1;i<=ns;i++)
				X[i-1].boundaryField()[patchi][facei] = x[i];
			#endif	
		}
	}

	for (unsigned int i=0;i<thermodynamicsMapXML->NumberOfSpecies();i++)
		X[i].write();		
}
