/*--------------------------------------------------------------------------------
    ooo      L   attice-based  |
  o\.|./o    e   xtensible     | LeMonADE: An Open Source Implementation of the
 o\.\|/./o   Mon te-Carlo      |           Bond-Fluctuation-Model for Polymers
oo---0---oo  A   lgorithm and  |
 o/./|\.\o   D   evelopment    | Copyright (C) 2013-2015 by 
  o/.|.\o    E   nvironment    | LeMonADE Principal Developers (see AUTHORS)
    ooo                        | 
----------------------------------------------------------------------------------

This file is part of LeMonADE.

LeMonADE is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LeMonADE is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LeMonADE.  If not, see <http://www.gnu.org/licenses/>.

--------------------------------------------------------------------------------*/


#ifndef FEATURE_CONTACT_INTERACTION_H
#define FEATURE_CONTACT_INTERACTION_H


/// @file
/// @date 2016/06/18
/// @author Hauke Rabbel
/// @brief Definition and implementation of class template FeatureContactInteractionSc


#include <LeMonADE/feature/Feature.h>
#include <LeMonADE/updater/moves/MoveBase.h>
#include <LeMonADE/updater/moves/MoveLocalSc.h>
#include <LeMonADE/feature/FeatureExcludedVolumeSc.h>
#include <LeMonADE/feature/FeatureBoltzmann.h>
#include <LeMonADE/feature/FeatureAttributes.h>
#include <LeMonADE/feature/NNInteractionReadWrite.h>


/// @class FeatureContactInteractionSc
/// @brief Provides interaction of monomers on distances d<=sqrt(6) for standard BFM
///
/// @tparam FeatureLatticeType Underlying lattice feature, e.g. FeatureLattice or 
/// FeatureLatticePowerOfTwo (template template parameter)
///
/// @details
/// The interaction energy can be set for pairs of monomer-types A,B, where
/// the type can be any integer between 1 and 255. 
/// The feature automatically adds FeatureExcludedVolumeSc<FeatureLatticeType<uint8_t>
/// to the system. Given an energy E in kT between  two types, the interaction potential 
/// as a function of the distance d is:
/// - inf d<2 (implicitly through excluded volume)
/// - 4*E d=2
/// - 2*E d=sqrt(5)
/// - 1*E d=sqrt(6)
/// - 0   d>sqrt(6)
/// .
/// Usage: In the feature list defining Ingredients use this feature as
/// FeatureContactInteractionSc<FeatureLattice> (arbitrary lattices), or as
/// FeatureContactInteractionSc<FeatureLatticePowerOfTwo> (2**n lattices)
/// The feature adds the bfm-file command !contact_interaction A B E
/// for monomers of types A B with interaction energy of E in kT.


template<template<typename> class FeatureLatticeType>
class FeatureContactInteractionSc:public Feature
{

private:
  //! Type for the underlying lattice, used as template parameter for FeatureLatticeType<...>
  typedef uint8_t lattice_value_type;

  //! Interaction energies between monomer types. Max. type=255 given by max(uint8_t)=255 
  double interactionTable[256][256];
  
  //! Lookup table for exp(-interactionTable[a][b])
  double probabilityLookup[256][256];

  //! Returns this feature's factor for the acceptance probability for the given Monte Carlo move
  template<class IngredientsType>
  double calculateAcceptanceProbability(const IngredientsType& ingredients, 
					const MoveLocalSc& move) const;

  //! Occupies the lattice with the attribute tags of all monomers
  template<class IngredientsType>
  void fillLattice(IngredientsType& ingredients);

  //! Access to array probabilityLookup with extra checks in Debug mode
  double getProbabilityFactor(int32_t typeA,int32_t typeB) const;


public:

  FeatureContactInteractionSc();
  ~FeatureContactInteractionSc(){}

    
  //This feature adds interaction energies, so it requires FeatureBoltzmann
  typedef LOKI_TYPELIST_1(FeatureBoltzmann) required_features_back;

  //FeatureExcludedVolumeSc needs to be in front, because FeatureContactInteractionSc
  //re-initializes the lattice and overwrites what FeatureExcludedVolumeSc has written.
  //FeatureAttributes needs to be in front, because when a monomer is added to the system
  //by a MoveAddScMonomer, its attribute has to be set before it is written to the lattice.
  typedef LOKI_TYPELIST_2(
      FeatureAttributes,
      FeatureExcludedVolumeSc<FeatureLatticeType<lattice_value_type> >) 
    required_features_front;


  //! check for all Monte Carlo moves without special check functions (always true)
  template<class IngredientsType>
    bool checkMove(const IngredientsType& ingredients,const MoveBase& move) const;

  //! check for standard sc-BFM local move
  template<class IngredientsType>
    bool checkMove(const IngredientsType& ingredients,MoveLocalSc& move) const;

  //! check move for bcc-BFM local move. always throws std::runtime_error
  template<class IngredientsType>
    bool checkMove(const IngredientsType& ingredients,const MoveLocalBcc& move) const;

  //! apply function for all Monte Carlo moves without special apply functions (does nothing)
  template<class IngredientsType>
    void applyMove(const IngredientsType& ing, const MoveBase& move){}

  //! apply function for bcc-BFM local move (always throws std::runtime_error)
  template<class IngredientsType>
    void applyMove(const IngredientsType& ing, const MoveLocalBcc& move);

  //! apply function for adding a monomer in sc-BFM
  template<class IngredientsType>
    void applyMove(IngredientsType& ing, const MoveAddScMonomer& move);

  //note: apply function for sc-BFM local move is not necessary, because 
  //job of moving lattice entries is done by the underlying FeatureLatticeType

  //! guarantees that the lattice is properly occupied with monomer attributes
  template<class IngredientsType>
  void synchronize(IngredientsType& ingredients);

  //!adds interaction energy between two types of monomers
  void setContactInteraction(int32_t typeA,int32_t typeB,double energy);

  //!returns the interaction energy between two types of monomers
  double getContactInteraction(int32_t typeA,int32_t typeB) const;

  //!export bfm-file read command !contact_interaction
  template <class IngredientsType>
  void exportRead(FileImport <IngredientsType>& fileReader);

  //!export bfm-file write command !contact_interaction
  template <class IngredientsType>
  void exportWrite(AnalyzerWriteBfmFile <IngredientsType>& fileWriter) const;

};


//////////////////  IMPLEMENTATION OF MEMBERS //////////////////////////////////


template<template<typename> class LatticeClassType>
FeatureContactInteractionSc<LatticeClassType>::FeatureContactInteractionSc()
{
  //initialize the energy and probability lookups with default values
  for(size_t n=0;n<256;n++)
    {
      for(size_t m=0;m<256;m++)
        {
	  interactionTable[m][n]=0.0;
	  probabilityLookup[m][n]=1.0;
        }
    }
}



template<template<typename> class LatticeClassType>
template<class IngredientsType>
bool FeatureContactInteractionSc<LatticeClassType>::checkMove(const IngredientsType& ingredients, 
							 const MoveBase& move) const
{
    return true;
}


template<template<typename> class LatticeClassType>
template<class IngredientsType>
bool FeatureContactInteractionSc<LatticeClassType>::checkMove(const IngredientsType& ingredients, 
							 MoveLocalSc& move) const
{
  //add the probability factor coming from this feature, then return true,
  //because the total probability is evaluated by FeatureBoltzmann at the end
  double prob=calculateAcceptanceProbability(ingredients,move);
  move.multiplyProbability(prob);
  return true;
}

template<template<typename> class LatticeClassType>
template<class IngredientsType>
bool FeatureContactInteractionSc<LatticeClassType>::checkMove(const IngredientsType& ingredients, 
							 const MoveLocalBcc& move) const
{
  //throw exception in case someone accidentaly uses a bcc-BFM move with this feature
  std::stringstream errormessage;
  errormessage<<"FeatureContactInteractionSc::checkMove(...):\n";
  errormessage<<"attempting to use bcc-BFM move, which is not allowed\n";
  throw std::runtime_error(errormessage.str());

  return false;
}



template<template<typename> class LatticeClassType>
template<class IngredientsType>
void FeatureContactInteractionSc<LatticeClassType>::applyMove(IngredientsType& ing, 
							 const MoveAddScMonomer& move)
{
    //get the position and attribute tag of the monomer to be inserted
    VectorInt3 pos=move.getPosition();
    VectorInt3 dx(1,0,0);
    VectorInt3 dy(0,1,0);
    VectorInt3 dz(0,0,1);
    int32_t type=move.getType();

    //update lattice
    ing.setLatticeEntry(pos,type);
    ing.setLatticeEntry(pos+dx,type);
    ing.setLatticeEntry(pos+dy,type);
    ing.setLatticeEntry(pos+dx+dy,type);
    ing.setLatticeEntry(pos+dz,type);
    ing.setLatticeEntry(pos+dz+dx,type);
    ing.setLatticeEntry(pos+dz+dy,type);
    ing.setLatticeEntry(pos+dz+dx+dy,type);
}


template<template<typename> class LatticeClassType>
template<class IngredientsType>
void FeatureContactInteractionSc<LatticeClassType>::applyMove(const IngredientsType& ing, 
							 const MoveLocalBcc& move)
{
  //throw exception in case someone accidentaly uses a bcc-BFM move with this feature
  std::stringstream errormessage;
  errormessage<<"FeatureContactInteractionSc::applyMove(...):\n";
  errormessage<<"attempting to use bcc-BFM move, which is not allowed\n";
  throw std::runtime_error(errormessage.str());

}

template<template<typename> class LatticeClassType>
template<class IngredientsType>
void FeatureContactInteractionSc<LatticeClassType>::synchronize(IngredientsType& ingredients)
{

    //refill the lattice with attribute tags
    //caution: this overwrites, what is currently written on the lattice
    fillLattice(ingredients);

}


template<template<typename> class LatticeClassType>
inline double FeatureContactInteractionSc<LatticeClassType>::getProbabilityFactor(int32_t typeA,
									     int32_t typeB) const
{
#ifdef DEBUG
  //extra checks only in debug mode, because this is very frequently called
  //and this costs performance
  if(typeA<0 || typeA>255 || typeB<0 || typeB>255){
    std::stringstream errormessage;
    errormessage<<"***FeatureNaNInteractionSc::getInteraction(typeA,typeB)***\n";
    errormessage<<"probability undefined between types "<<typeA<<" and "<<typeB<<std::endl;
    errormessage<<"types are out of the allowed range";
    throw std::runtime_error(errormessage.str());
  }
#endif /*DEBUG*/

  return probabilityLookup[typeA][typeB];

}


template<template<typename> class LatticeClassType>
template<class IngredientsType>
void FeatureContactInteractionSc<LatticeClassType>::exportRead(FileImport< IngredientsType >& fileReader)
{
  typedef FeatureContactInteractionSc<LatticeClassType> my_type;
  fileReader.registerRead("!contact_interaction",new ReadContactInteraction<my_type>(*this));
}


template<template<typename> class LatticeClassType>
template<class IngredientsType>
void FeatureContactInteractionSc<LatticeClassType>::exportWrite(AnalyzerWriteBfmFile< IngredientsType >& fileWriter) const
{
  typedef FeatureContactInteractionSc<LatticeClassType> my_type;
  fileWriter.registerWrite("!contact_interaction",new WriteContactInteraction<my_type>(*this));
}


template<template<typename> class LatticeClassType>
template<class IngredientsType>
void FeatureContactInteractionSc<LatticeClassType>::fillLattice(IngredientsType& ingredients)
{
    const typename IngredientsType::molecules_type& molecules=ingredients.getMolecules();

    for(size_t n=0;n<molecules.size();n++)
    {
        VectorInt3 pos=molecules[n];
	lattice_value_type attribute=lattice_value_type(molecules[n].getAttributeTag());

	if(int32_t(attribute)!=molecules[n].getAttributeTag()){
	  std::stringstream errormessage;
	  errormessage<<"***FeatureContactInteractionSc::fillLattice()***\n";
	  errormessage<<"type "<<attribute<<" is out of the allowed range";

	  throw std::runtime_error(errormessage.str());
	}

        ingredients.setLatticeEntry(pos,attribute);
        ingredients.setLatticeEntry(pos+VectorInt3(1,0,0),attribute);
        ingredients.setLatticeEntry(pos+VectorInt3(0,1,0),attribute);
        ingredients.setLatticeEntry(pos+VectorInt3(1,1,0),attribute);
        ingredients.setLatticeEntry(pos+VectorInt3(0,0,1),attribute);
        ingredients.setLatticeEntry(pos+VectorInt3(1,0,1),attribute);
        ingredients.setLatticeEntry(pos+VectorInt3(0,1,1),attribute);
        ingredients.setLatticeEntry(pos+VectorInt3(1,1,1),attribute);

    }

}


template<template<typename> class LatticeClassType>
template<class IngredientsType>
double FeatureContactInteractionSc<LatticeClassType>::calculateAcceptanceProbability(
    const IngredientsType& ingredients,
    const MoveLocalSc& move) const
{
    
    VectorInt3 oldPos=ingredients.getMolecules()[move.getIndex()];
    VectorInt3 direction=move.getDir();

    double prob=1.0;
    int32_t monoType=ingredients.getMolecules()[move.getIndex()].getAttributeTag();

    /*get two directions perpendicular to vector directon of the move*/
    VectorInt3 perp1,perp2;
    /* first perpendicular direction is either (0 1 0) or (1 0 0)*/
    int32_t x1=((direction.getX()==0) ? 1 : 0);
    int32_t y1=((direction.getX()!=0) ? 1 : 0);
    perp1.setX(x1);
    perp1.setY(y1);
    perp1.setZ(0);

    /* second perpendicular direction is either (0 0 1) or (0 1 0)*/
    int32_t y2=((direction.getZ()==0) ? 0 : 1);
    int32_t z2=((direction.getZ()!=0) ? 0 : 1);
    perp2.setX(0);
    perp2.setY(y2);
    perp2.setZ(z2);

    //the probability is calculated by going through all possible lattice sites
    //at which the contacts may have changed. At every site the type of the
    //monomer sitting there is retrieved from the lattice. the additional
    //factor for the probability (exp(-deltaE/kT)) is retrieved from the
    //lookup using getProbabilityFactor. For new contacts this factor is multiplied
    //with the probability, for contacts taken away the probability is devided.
    VectorInt3 actual=oldPos;

    //first check front,i.e newly acquired contacts
    if(direction.getX()>0 || direction.getY()>0 || direction.getZ()>0) actual+=direction;
    actual+=direction;

    actual-=perp1;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual+=perp2;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual=actual+perp2+perp1;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual+=perp1;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual=actual+perp1-perp2;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual-=perp2;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual=actual-perp1-perp2;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual-=perp1;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual=actual+perp2+direction;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual+=perp2;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual+=perp1;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual-=perp2;
    prob*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));

    //now check back side (contacts taken away)
    double prob_div=1.0;
    actual=oldPos;
    if(direction.getX()<0 || direction.getY()<0 || direction.getZ()<0) actual-=direction;
    actual-=perp1;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual+=perp2;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual=actual+perp2+perp1;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual+=perp1;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual=actual+perp1-perp2;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual-=perp2;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual=actual-perp1-perp2;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual-=perp1;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual=actual+perp2-direction;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual+=perp2;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual+=perp1;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));
    actual-=perp2;
    prob_div*=getProbabilityFactor(monoType,int32_t(ingredients.getLatticeEntry(actual)));

    prob/=prob_div;
    return prob;

}


template<template<typename> class LatticeClassType>
void FeatureContactInteractionSc<LatticeClassType>::setContactInteraction(int32_t typeA,
								     int32_t typeB,
								     double energy)
{
    if(0<typeA && typeA<=255 && 0<typeB && typeB<=255)
      {
        interactionTable[typeA][typeB]=energy;
        interactionTable[typeB][typeA]=energy;
        probabilityLookup[typeA][typeB]=exp(-energy);
        probabilityLookup[typeB][typeA]=exp(-energy);
        std::cout<<"set interation between types ";
	std::cout<<typeA<<" and "<<typeB<<" to "<<energy<<"kT\n";
      }
    else
      {
	std::stringstream errormessage;
	errormessage<<"FeatureContactInteractionSc::setContactInteraction(typeA,typeB,energy).\n";
	errormessage<<"typeA "<<typeA<<" typeB "<<typeB<<": Types out of range\n";
	throw std::runtime_error(errormessage.str());
      }
}


template<template<typename> class LatticeClassType>
double FeatureContactInteractionSc<LatticeClassType>::getContactInteraction(int32_t typeA,
								       int32_t typeB) const
{

    if(0<typeA && typeA<=255 && 0<typeB && typeB<=255)
        return interactionTable[typeA][typeB];
    else
    {
      std::stringstream errormessage;
      errormessage<<"FeatureContactInteractionSc::getContactInteraction(typeA,typeB).\n";
      errormessage<<"typeA "<<typeA<<" typeB "<<typeB<<": Types out of range\n";
      throw std::runtime_error(errormessage.str());
    }

}

//TODO: rename files, change IO, tests, comments for doxygen

#endif /*FEATURE_CONTACT_INTERACTION_H*/
