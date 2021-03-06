/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::solidMixtureProperties

Description
    A mixture of solids.

Note
    The dictionary constructor searches for the entry \c solidComponents,
    which is a wordList. The solid properties of each component can either
    be contained within a \c solidProperties sub-dictionary or (for legacy
    purposes) can be found directly in the dictionary.
    The \c solidProperties sub-dictionary entry should be used when possible
    to avoid conflicts with identically named gas-phase entries.

SeeAlso
    Foam::solidMixtureProperties

\*---------------------------------------------------------------------------*/

#ifndef solidMixtureProperties_H
#define solidMixtureProperties_H

#include "scalarField.H"
#include "PtrList.H"
#include "solidProperties.H"
#include "autoPtr.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class solidMixtureProperties Declaration
\*---------------------------------------------------------------------------*/

class solidMixtureProperties
{
    // Private data

        //- The names of the solids
        List<word> components_;

        //- The solidProperties properties
        PtrList<solidProperties> properties_;


public:

    // Constructors

        //- Construct from dictionary
        solidMixtureProperties(const dictionary&);

        //- Construct copy
        solidMixtureProperties(const solidMixtureProperties& lm);

        //- Construct and return a clone
        virtual autoPtr<solidMixtureProperties> clone() const
        {
            return autoPtr<solidMixtureProperties>
            (
                new solidMixtureProperties(*this)
            );
        }


    //- Destructor
    virtual ~solidMixtureProperties()
    {}


    // Selectors

        //- Select construct from dictionary
        static autoPtr<solidMixtureProperties> New(const dictionary&);


    // Member Functions

        //- Return the solidProperties names
        inline const List<word>& components() const
        {
            return components_;
        }

        //- Return the solidProperties properties
        inline const PtrList<solidProperties>& properties() const
        {
            return properties_;
        }

        //- Return the number of solids in the mixture
        inline label size() const
        {
            return components_.size();
        }


        //- Returns the mass fractions, given mole fractions
        scalarField Y(const scalarField& X) const;

        //- Returns the mole fractions, given mass fractions
        scalarField X(const scalarField& Y) const;

        //- Calculate the mixture density [kg/m^3] as a function of
        //  volume fractions
        scalar rho(const scalarField& X) const;

        //- Calculate the mixture heat capacity [J/(kg K)] as a function
        //  of mass fractions
        scalar Cp(const scalarField& Y) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
