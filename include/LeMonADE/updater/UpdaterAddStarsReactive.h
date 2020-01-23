/*--------------------------------------------------------------------------------
    ooo      L   attice-based  |
  o\.|./o    e   xtensible     | LeMonADE: An Open Source Implementation of the
 o\.\|/./o   Mon te-Carlo      |           Bond-Fluctuation-Model for Polymers
oo---0---oo  A   lgorithm and  |
 o/./|\.\o   D   evelopment    | Copyright (C) 2013-2015 by
  o/.|.\o    E   nvironment    | LeMonADE Principal Developers
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

#ifndef LEMONADE_UPDATER_SETUP_STARS
#define LEMONADE_UPDATER_SETUP_STARS
/**
 * @file
 *
 * @class UpdaterAddStars
 *
 * @brief Updater to create a solution of monodisperse branched stars.
 *
 * @details This is a simple implementation of a system setup starting from an empty ingredients
 * or a system with some monomers inside. This updater requires FeatureAttributes.
 * Two tags are added to the monomers in alternating manner, usually needed for GPU computing.  // not realized in this first attempt
 *                                                                                              // RS, 05 Nov 2019
 *
 * @tparam IngredientsType
 *
 * @param ingredients_ The system, holding either an empty simulation box for system setup
 * or a prefilled ingredients where the stars shall be added
 * @param NStar_ number of stars that are added to ingredients
 * @param NMonoPerStar_ number of monomers in each star
 * @param NBranchPerStar_ number of branches in each star
 * @param NMonoPerBranch_ number of monomers in each branch (excluding central monomer)
 * @param type1_ attribute tag of "even" monomers
 * @param type2_ attribute tag of "odd" monomers
 **/

#include <LeMonADE/updater/UpdaterAbstractCreate.h>
#include <LeMonADE/utility/Vector3D.h>
#include <cmath>

template<class IngredientsType>
class UpdaterAddStars: public UpdaterAbstractCreate<IngredientsType>
{
  typedef UpdaterAbstractCreate<IngredientsType> BaseClass;

public:
  UpdaterAddStars(IngredientsType& ingredients_, uint32_t NStar_, uint32_t NMonoPerStar_, 
                  uint32_t NBranchPerStar_, uint32_t NMonoPerBranch_, 
                  int32_t type1_=1, int32_t type2_=2, bool IsSolvent=false, bool IsReactive=false);

  virtual void initialize();
  virtual bool execute();
  virtual void cleanup();

  //! getter function for write compressed solvent bool
  const bool getIsSolvent() const {return IsSolvent;}

  //! getter function for number of stars
  const int32_t getNStar() const {return NStar;}
  
  //! getter function for number of monomers in stars
  const int32_t getNMonoPerStar() const {return NMonoPerStar;}

   //! getter function for number of branches in stars
  const int32_t getNBranchPerStar() const {return NBranchPerStar;}

  //! getter function for number of monomers in branch
  const int32_t getNMonoPerBranch() const {return NMonoPerBranch;}

  //! getter function for calculated density
  const double getDensity() const {return density;}

private:
  // provide access to functions of UpdaterAbstractCreate used in this updater
  using BaseClass::ingredients;
  using BaseClass::addMonomerToParent;
  using BaseClass::addSingleMonomer;
  using BaseClass::linearizeSystem;

  //! number of stars in the box
  uint32_t NStar;
  
  //! number of monomers in a star
  uint32_t NMonoPerStar;

  //! number of branches in a star
  uint32_t NBranchPerStar;

  //! number of monomers in a branch
  uint32_t NMonoPerBranch;  

  //! lattice occupation density
  double density;

  //! bool for execution
  bool wasExecuted;

  //! attribute tag of even monomers
  int32_t type1;

  //! getAttributeTag of odd monomers
  int32_t type2;

  //! bool to check if chains of size 1 should be compressed to solvent
  bool IsSolvent;

   //! bool to check if monomers are reactive (intended: only branch ends shall be reactive)
  bool IsReactive;
 
};

/**
* @brief Constructor handling the new systems parameters
* 
* @param ingredients_ a reference to the IngredientsType - mainly the system
* @param NStar_ number of stars to be added in the system instead of solvent
* @param NMonoPerStar_ number of monomers in each star
* @param NBranchPerStar_ number of branches in each star
* @param NMonoPerBranch_ number of monomers in each branch (excluding central monomer)
*/


template < class IngredientsType >
UpdaterAddStars<IngredientsType>::UpdaterAddStars(IngredientsType& ingredients_, uint32_t NStar_, 
                                                  uint32_t NMonoPerStar_, uint32_t NBranchPerStar_, uint32_t NMonoPerBranch_, 
                                                  int32_t type1_, int32_t type2_, bool IsSolvent_, bool IsReactive_):
BaseClass(ingredients_), NStar(NStar_), 
NMonoPerStar(NMonoPerStar_), NBranchPerStar(NBranchPerStar_), NMonoPerBranch(NMonoPerBranch_), 
density(0.0), wasExecuted(false), type1(type1_), type2(type2_), IsSolvent(IsSolvent_), IsReactive(IsReactive_)
{}

/**
* @brief initialise function, calculate the target density to compare with at the end.
*
* @tparam IngredientsType Features used in the system. See Ingredients.
*/
template < class IngredientsType >
void UpdaterAddStars<IngredientsType>::initialize(){
  std::cout << "initialize UpdaterAddStars" << std::endl;

  // get the target density from the sum of existing monomers and the new added chains
  density=(double)( ingredients.getMolecules().size() + NMonoPerStar*NStar ) * 8  /(double)( ingredients.getBoxX()*ingredients.getBoxY()*ingredients.getBoxZ() );

  std::cout << "add "<<NStar*NMonoPerStar<<" monomers to the box"<<std::endl;

  execute();
}

/**
* @brief Execution of the system creation
*
* @tparam IngredientsType Features used in the system. See Ingredients.
*/
template < class IngredientsType >
bool UpdaterAddStars<IngredientsType>::execute(){
  if(wasExecuted)
    return true;

  std::cout << "execute UpdaterAddStars" << std::endl;

  //loop over stars and star monomers and build it up
  for(uint32_t i=0;i<(NStar);i++){
    for(uint32_t j=0;j<(NMonoPerStar);j++){
      if(j==0)
	addSingleMonomer(type1);  // central monomer of star
      else{
    if(j<=NBranchPerStar) {                                             // first generation of each branch
      if(ingredients.getMolecules()[ingredients.getMolecules().size()-j].getAttributeTag() == type1)
        addMonomerToParent(ingredients.getMolecules().size()-j,type1);  // size()-j shall correspond to center of star
    }
    if(j>NBranchPerStar) {                                              // second and higher generation of each branch
      if(ingredients.getMolecules()[ingredients.getMolecules().size()-NBranchPerStar].getAttributeTag() == type1)
        addMonomerToParent(ingredients.getMolecules().size()-NBranchPerStar,type1);  // size()-NBranchPerStar shall correspond to
    } // previous generation in each branch                                                                            
    //      
//	if(ingredients.getMolecules()[ingredients.getMolecules().size()-1].getAttributeTag() == type1)
//	  addMonomerToParent(ingredients.getMolecules().size()-1,type2);
//	else
//	  addMonomerToParent(ingredients.getMolecules().size()-1,type1);
//          
      }
    IsReactive == false;
    if(j>=NMonoPerStar-NBranchPerStar-1) IsReactive == true;  
    }
  }

  ingredients.synchronize();
  double lattice_volume(ingredients.getBoxX()*ingredients.getBoxY()*ingredients.getBoxZ());
  if(std::abs(density - ( (double)(ingredients.getMolecules().size()*8) / lattice_volume )) > 0.0000000001 ){
    std::cout << density << " " <<( (ingredients.getMolecules().size()*8) / lattice_volume)<<std::endl;
    throw std::runtime_error("UpdaterAddStars: number of monomers in molecules does not match the calculated number of monomers!");
  }else{
    std::cout << "real lattice occupation density =" << (8*ingredients.getMolecules().size()) / lattice_volume<<std::endl;
    wasExecuted=true;
    // if we allow for solvent compression AND added single monomers (solvent): compress solvent
    if(IsSolvent && NMonoPerStar==1){
      ingredients.setCompressedOutputIndices(ingredients.getMolecules().size()-(NMonoPerStar*NStar),ingredients.getMolecules().size()-1);
    }else{
      linearizeSystem();
    }
    return true;
  }
}

/**
* @brief Standard clean up.
*
* @tparam IngredientsType Features used in the system. See Ingredients.
*/
template < class IngredientsType >
void UpdaterAddStars<IngredientsType>::cleanup(){

}


#endif /* LEMONADE_UPDATER_SETUP_LINEAR_CHAINS */
