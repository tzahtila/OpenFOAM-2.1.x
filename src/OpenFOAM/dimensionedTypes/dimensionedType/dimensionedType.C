/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2012 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "dimensionedType.H"
#include "pTraits.H"
#include "dictionary.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

template <class Type>
dimensioned<Type> dimensioned<Type>::lookupOrDefault
(
    const word& name,
    const dictionary& dict,
    const Type& defaultValue,
    const dimensionSet& dims
)
{
    Type value = dict.lookupOrDefault<Type>(name, defaultValue);
    return dimensioned<Type>(name, dims, value);
}


template <class Type>
dimensioned<Type> dimensioned<Type>::lookupOrAddToDict
(
    const word& name,
    dictionary& dict,
    const Type& defaultValue,
    const dimensionSet& dims
)
{
    Type value = dict.lookupOrAddDefault<Type>(name, defaultValue);
    return dimensioned<Type>(name, dims, value);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <class Type>
dimensioned<Type>::dimensioned
(
    const word& name,
    const dimensionSet& dimSet,
    const Type t
)
:
    name_(name),
    dimensions_(dimSet),
    value_(t)
{}


template <class Type>
dimensioned<Type>::dimensioned
(
    const word& name,
    const dimensioned<Type>& dt
)
:
    name_(name),
    dimensions_(dt.dimensions_),
    value_(dt.value_)
{}


template <class Type>
dimensioned<Type>::dimensioned
(
    Istream& is
)
:
    name_(is),
    dimensions_(is),
    value_(pTraits<Type>(is))
{}


template <class Type>
dimensioned<Type>::dimensioned
(
    const word& name,
    Istream& is
)
:
    name_(name),
    dimensions_(is),
    value_(pTraits<Type>(is))
{}


template <class Type>
dimensioned<Type>::dimensioned
(
    const word& name,
    const dimensionSet& dimSet,
    Istream& is
)
:
    name_(name),
    dimensions_(dimSet),
    value_(pTraits<Type>::zero)
{
    token nextToken(is);
    is.putBack(nextToken);

    // Check if the original format is used in which the name is provided
    // and reset the name to that read
    if (nextToken.isWord())
    {
        is >> name_;
        is >> nextToken;
        is.putBack(nextToken);
    }

    // If the dimensions are provided compare with the argument
    if (nextToken == token::BEGIN_SQR)
    {
        dimensionSet dims(is);

        if (dims != dimensions_)
        {
            FatalErrorIn
            (
                "dimensioned<Type>::dimensioned"
                "(const word&, const dimensionSet&, Istream&)"
            ) << "The dimensions " << dims
              << " provided do not match the required dimensions "
              << dimensions_
              << abort(FatalError);
        }
    }

    is >> value_;
}


template <class Type>
dimensioned<Type>::dimensioned
()
:
    name_("undefined"),
    dimensions_(dimless),
    value_(pTraits<Type>::zero)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class Type>
const word& dimensioned<Type>::name() const
{
    return name_;
}

template <class Type>
word& dimensioned<Type>::name()
{
    return name_;
}


template <class Type>
const dimensionSet& dimensioned<Type>::dimensions() const
{
    return dimensions_;
}

template <class Type>
dimensionSet& dimensioned<Type>::dimensions()
{
    return dimensions_;
}


template <class Type>
const Type& dimensioned<Type>::value() const
{
    return value_;
}

template <class Type>
Type& dimensioned<Type>::value()
{
    return value_;
}


template <class Type>
dimensioned<typename dimensioned<Type>::cmptType> dimensioned<Type>::component
(
    const direction d
) const
{
    return dimensioned<cmptType>
    (
        name_ + ".component(" + Foam::name(d) + ')',
        dimensions_,
        value_.component(d)
    );
}


template <class Type>
void dimensioned<Type>::replace
(
    const direction d,
    const dimensioned<typename dimensioned<Type>::cmptType>& dc
)
{
    dimensions_ = dc.dimensions();
    value_.replace(d, dc.value());
}


template <class Type>
bool dimensioned<Type>::readIfPresent(const dictionary& dict)
{
    return dict.readIfPresent(name_, value_);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template <class Type>
dimensioned<typename dimensioned<Type>::cmptType> dimensioned<Type>::operator[]
(
    const direction d
) const
{
    return component(d);
}


template <class Type>
void dimensioned<Type>::operator+=
(
    const dimensioned<Type>& dt
)
{
    dimensions_ += dt.dimensions_;
    value_ += dt.value_;
}


template <class Type>
void dimensioned<Type>::operator-=
(
    const dimensioned<Type>& dt
)
{
    dimensions_ -= dt.dimensions_;
    value_ -= dt.value_;
}


template <class Type>
void dimensioned<Type>::operator*=
(
    const scalar s
)
{
    value_ *= s;
}


template <class Type>
void dimensioned<Type>::operator/=
(
    const scalar s
)
{
    value_ /= s;
}


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //

template<class Type, int r>
dimensioned<typename powProduct<Type, r>::type>
pow(const dimensioned<Type>& dt, typename powProduct<Type, r>::type)
{
    return dimensioned<typename powProduct<Type, r>::type>
    (
        "pow(" + dt.name() + ',' + name(r) + ')',
        pow(dt.dimensions(), r),
        pow(dt.value(), 2)
    );
}

template<class Type>
dimensioned<typename outerProduct<Type, Type>::type>
sqr(const dimensioned<Type>& dt)
{
    return dimensioned<typename outerProduct<Type, Type>::type>
    (
        "sqr(" + dt.name() + ')',
        sqr(dt.dimensions()),
        sqr(dt.value())
    );
}

template<class Type>
dimensioned<scalar> magSqr(const dimensioned<Type>& dt)
{
    return dimensioned<scalar>
    (
        "magSqr(" + dt.name() + ')',
        magSqr(dt.dimensions()),
        magSqr(dt.value())
    );
}

template<class Type>
dimensioned<scalar> mag(const dimensioned<Type>& dt)
{
    return dimensioned<scalar>
    (
        "mag(" + dt.name() + ')',
        dt.dimensions(),
        mag(dt.value())
    );
}


template <class Type>
dimensioned<Type> max
(
    const dimensioned<Type>& dt1,
    const dimensioned<Type>& dt2
)
{
    if (dt1.dimensions() != dt2.dimensions())
    {
        FatalErrorIn("max(const dimensioned<Type>&, const dimensioned<Type>&)")
            << "dimensions of arguments are not equal"
            << abort(FatalError);
    }

    return dimensioned<Type>
    (
        "max(" + dt1.name() + ',' + dt2.name() + ')',
        dt1.dimensions(),
        max(dt1.value(), dt2.value())
    );
}


template <class Type>
dimensioned<Type> min
(
    const dimensioned<Type>& dt1,
    const dimensioned<Type>& dt2
)
{
    if (dt1.dimensions() != dt2.dimensions())
    {
        FatalErrorIn("min(const dimensioned<Type>&, const dimensioned<Type>&)")
            << "dimensions of arguments are not equal"
            << abort(FatalError);
    }

    return dimensioned<Type>
    (
        "min(" + dt1.name() + ',' + dt2.name() + ')',
        dt1.dimensions(),
        min(dt1.value(), dt2.value())
    );
}


// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

template <class Type>
Istream& operator>>(Istream& is, dimensioned<Type>& dt)
{
    token nextToken(is);
    is.putBack(nextToken);

    // Check if the original format is used in which the name is provided
    // and reset the name to that read
    if (nextToken.isWord())
    {
        is >> dt.name_;
        is >> nextToken;
        is.putBack(nextToken);
    }

    // If the dimensions are provided reset the dimensions to those read
    if (nextToken == token::BEGIN_SQR)
    {
        is >> dt.dimensions_;
    }

    // Read the value
    is >> dt.value_;

    // Check state of Istream
    is.check("Istream& operator>>(Istream&, dimensioned<Type>&)");

    return is;
}


template <class Type>
Ostream& operator<<(Ostream& os, const dimensioned<Type>& dt)
{
    // do a stream write op for a dimensions()et
    os  << dt.name() << token::SPACE
        << dt.dimensions() << token::SPACE
        << dt.value();

    // Check state of Ostream
    os.check("Ostream& operator<<(Ostream&, const dimensioned<Type>&)");

    return os;
}


// * * * * * * * * * * * * * * * Global Operators  * * * * * * * * * * * * * //

template <class Type>
bool operator>
(
    const dimensioned<Type>& dt1,
    const dimensioned<Type>& dt2
)
{
    return dt1.value() > dt2.value();
}


template <class Type>
bool operator<
(
    const dimensioned<Type>& dt1,
    const dimensioned<Type>& dt2
)
{
    return dt1.value() < dt2.value();
}


template <class Type>
dimensioned<Type> operator+
(
    const dimensioned<Type>& dt1,
    const dimensioned<Type>& dt2
)
{
    return dimensioned<Type>
    (
        '(' + dt1.name() + '+' + dt2.name() + ')',
        dt1.dimensions() + dt2.dimensions(),
        dt1.value() + dt2.value()
    );
}


template <class Type>
dimensioned<Type> operator-(const dimensioned<Type>& dt)
{
    return dimensioned<Type>
    (
        '-' + dt.name(),
        dt.dimensions(),
        -dt.value()
    );
}


template <class Type>
dimensioned<Type> operator-
(
    const dimensioned<Type>& dt1,
    const dimensioned<Type>& dt2
)
{
    return dimensioned<Type>
    (
        '(' + dt1.name() + '-' + dt2.name() + ')',
        dt1.dimensions() - dt2.dimensions(),
        dt1.value() - dt2.value()
    );
}


template <class Type>
dimensioned<Type> operator*
(
    const dimensioned<scalar>& ds,
    const dimensioned<Type>& dt
)
{
    return dimensioned<Type>
    (
        '(' + ds.name() + '*' + dt.name() + ')',
        ds.dimensions() * dt.dimensions(),
        ds.value() * dt.value()
    );
}


template <class Type>
dimensioned<Type> operator/
(
    const dimensioned<Type>& dt,
    const dimensioned<scalar>& ds
)
{
    return dimensioned<Type>
    (
        '(' + dt.name() + '|' + ds.name() + ')',
        dt.dimensions()/ds.dimensions(),
        dt.value()/ds.value()
    );
}


// Products
// ~~~~~~~~

#define PRODUCT_OPERATOR(product, op, opFunc)                                 \
                                                                              \
template<class Type1, class Type2>                                            \
dimensioned<typename product<Type1, Type2>::type>                             \
operator op(const dimensioned<Type1>& dt1, const dimensioned<Type2>& dt2)     \
{                                                                             \
    return dimensioned<typename product<Type1, Type2>::type>                  \
    (                                                                         \
        '(' + dt1.name() + #op + dt2.name() + ')',                            \
        dt1.dimensions() op dt2.dimensions(),                                 \
        dt1.value() op dt2.value()                                            \
    );                                                                        \
}                                                                             \
                                                                              \
template<class Type, class Form, class Cmpt, int nCmpt>                       \
dimensioned<typename product<Type, Form>::type>                               \
operator op                                                                   \
(                                                                             \
    const dimensioned<Type>& dt1,                                             \
    const VectorSpace<Form,Cmpt,nCmpt>& t2                                    \
)                                                                             \
{                                                                             \
    return dimensioned<typename product<Type, Form>::type>                    \
    (                                                                         \
        '(' + dt1.name() + #op + name(t2) + ')',                              \
        dt1.dimensions(),                                                     \
        dt1.value() op static_cast<const Form&>(t2)                           \
    );                                                                        \
}                                                                             \
                                                                              \
template<class Type, class Form, class Cmpt, int nCmpt>                       \
dimensioned<typename product<Form, Type>::type>                               \
operator op                                                                   \
(                                                                             \
    const VectorSpace<Form,Cmpt,nCmpt>& t1,                                   \
    const dimensioned<Type>& dt2                                              \
)                                                                             \
{                                                                             \
    return dimensioned<typename product<Form, Type>::type>                    \
    (                                                                         \
        '(' + name(t1) + #op + dt2.name() + ')',                              \
        dt2.dimensions(),                                                     \
        static_cast<const Form&>(t1) op dt2.value()                           \
    );                                                                        \
}


PRODUCT_OPERATOR(outerProduct, *, outer)
PRODUCT_OPERATOR(crossProduct, ^, cross)
PRODUCT_OPERATOR(innerProduct, &, dot)
PRODUCT_OPERATOR(scalarProduct, &&, dotdot)

#undef PRODUCT_OPERATOR


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
