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

#ifndef FEATURE_SYSTEM_INFORMATION_STAR_H
#define FEATURE_SYSTEM_INFORMATION_STAR_H

#include <LeMonADE/feature/Feature.h>
#include <LeMonADE/updater/moves/MoveBase.h>
#include <LeMonADE/updater/moves/MoveLocalSc.h>
#include <LeMonADE/io/FileImport.h>
#include <LeMonADE/analyzer/AnalyzerWriteBfmFile.h>


class FeatureSystemInformationStar:public Feature
{
public:

	FeatureSystemInformationStar(){
	  nChains=0;
	  nBranches=0;
	  nChainLength=0;
	  nBranchLength=0;
	};
	virtual ~FeatureSystemInformationStar(){};
	
	//! For all unknown moves: this does nothing
	template<class IngredientsType> 
	bool checkMove(const IngredientsType& ingredients,const MoveBase& move) const {return true; }
	
	//! Overloaded for moves of type MoveScMonomer to check for sinusoidal movement
	template<class IngredientsType> 
	bool checkMove(const IngredientsType& ingredients,MoveLocalSc& move) const {return true; }

	//! Export the relevant functionality for reading bfm-files to the responsible reader object
	template <class IngredientsType>
	void exportRead(FileImport <IngredientsType>& fileReader);
	
	//! Export the relevant functionality for reading bfm-files to the responsible reader object
	template <class IngredientsType>
	void exportWrite(AnalyzerWriteBfmFile <IngredientsType>& filewriter) const;

	//!
	const uint32_t getNumStars() const {return nChains;} 
	//!
	void setNumStars(uint32_t nChains_){nChains=nChains_;}

	//!
	const uint32_t getNumBranches() const { return nBranches;}
	//!
	void setNumBranches(uint32_t nBranches_){nBranches=nBranches_;}
	
	//!
	const uint32_t getNumChainLength() const { return nChainLength;}
	//!
	void setNumChainLength(uint32_t nChainLength_){nChainLength=nChainLength_;}
	
	//!
	const uint32_t getNumBranchLength() const { return nBranchLength;}
	//!
	void setNumBranchLength(uint32_t nBranchLength_){nBranchLength=nBranchLength_;}
	
protected:
	uint32_t nChains;
	uint32_t nBranches;
	uint32_t nChainLength;
	uint32_t nBranchLength;

};

/*****************************************************************/
/**
 * @class ReadNumStars
 *
 * @brief Handles BFM-File-Reads \b #!number_of_stars
 * @tparam IngredientsType Ingredients class storing all system information.
 **/
template < class IngredientsType>
class ReadNumStars: public ReadToDestination<IngredientsType>
{
public:
  ReadNumStars(IngredientsType& i):ReadToDestination<IngredientsType>(i){}
  virtual ~ReadNumStars(){}
  virtual void execute();
};

template<class IngredientsType>
void ReadNumStars<IngredientsType>::execute()
{
	std::cout<<"reading number of stars...";

	uint32_t Chains = 0;
	IngredientsType& ingredients=this->getDestination();
	std::istream& source=this->getInputStream();

	std::string line;
	getline(source,line);
	Chains = atof(line.c_str());
	std::cout << "#!number_of_stars=" << (Chains) << std::endl;

	ingredients.setNumStars(Chains);
}
/*****************************************************************/
/**
 * @class ReadNumBranches
 *
 * @brief Handles BFM-File-Reads \b #!number_of_branches
 * @tparam IngredientsType Ingredients class storing all system information.
 **/
template < class IngredientsType>
class ReadNumBranches: public ReadToDestination<IngredientsType>
{
public:
  ReadNumBranches(IngredientsType& i):ReadToDestination<IngredientsType>(i){}
  virtual ~ReadNumBranches(){}
  virtual void execute();
};

template<class IngredientsType>
void ReadNumBranches<IngredientsType>::execute()
{
	std::cout<<"reading number of branches...";

	uint32_t Branches = 0;
	IngredientsType& ingredients=this->getDestination();
	std::istream& source=this->getInputStream();

	std::string line;
	getline(source,line);
	Branches = atof(line.c_str());
	std::cout << "#!number_of_branches=" << (Branches) << std::endl;

	ingredients.setNumBranches(Branches);
}
/*****************************************************************/
/**
 * @class ReadNumBranchLength
 *
 * @brief Handles BFM-File-Reads \b #!number_of_branch_length
 * @tparam IngredientsType Ingredients class storing all system information.
 **/
template < class IngredientsType>
class ReadNumBranchLength: public ReadToDestination<IngredientsType>
{
public:
  ReadNumBranchLength(IngredientsType& i):ReadToDestination<IngredientsType>(i){}
  virtual ~ReadNumBranchLength(){}
  virtual void execute();
};

template<class IngredientsType>
void ReadNumBranchLength<IngredientsType>::execute()
{
	std::cout<<"reading number of branch length...";

	uint32_t BranchLength = 0;
	IngredientsType& ingredients=this->getDestination();
	std::istream& source=this->getInputStream();

	std::string line;
	getline(source,line);
	BranchLength = atof(line.c_str());
	std::cout << "#!number_of_branch_length=" << (BranchLength) << std::endl;

	ingredients.setNumBranchLength(BranchLength);
}
/*****************************************************************/
/**
 * @class ReadNumChainLength
 *
 * @brief Handles BFM-File-Reads \b #!number_of_chain_length
 * @tparam IngredientsType Ingredients class storing all system information.
 **/
template < class IngredientsType>
class ReadNumChainLength: public ReadToDestination<IngredientsType>
{
public:
  ReadNumChainLength(IngredientsType& i):ReadToDestination<IngredientsType>(i){}
  virtual ~ReadNumChainLength(){}
  virtual void execute();
};

template<class IngredientsType>
void ReadNumChainLength<IngredientsType>::execute()
{
	std::cout<<"reading chain length...";

	uint32_t ChainLength = 0;
	IngredientsType& ingredients=this->getDestination();
	std::istream& source=this->getInputStream();

	std::string line;
	getline(source,line);
	ChainLength = atof(line.c_str());
	std::cout << "#!number_of_chain_length=" << (ChainLength) << std::endl;

	ingredients.setNumChainLength(ChainLength);
}

/**
 * @details The function is called by the Ingredients class when an object of type Ingredients
 * is associated with an object of type FileImport. The export of the Reads is thus
 * taken care automatically when it becomes necessary.\n
 * Registered Read-In Commands:
 * * #!number_of_stars
 * * #!number_of_branches
 * * #!number_of_branch_length
 * * #!number_of_chain_length
 *
 * @param fileReader File importer for the bfm-file
 * @param destination List of Feature to write-in from the read values.
 * @tparam IngredientsType Ingredients class storing all system information.
 **/
template<class IngredientsType>
void FeatureSystemInformationStar::exportRead(FileImport< IngredientsType >& fileReader)
{
    fileReader.registerRead("#!number_of_stars", new ReadNumStars<IngredientsType>(fileReader.getDestination()));
    fileReader.registerRead("#!number_of_branches", new ReadNumBranches<IngredientsType>(fileReader.getDestination()));
    fileReader.registerRead("#!number_of_branch_length", new ReadNumBranchLength<IngredientsType>(fileReader.getDestination()));
    fileReader.registerRead("#!number_of_chain_length", new ReadNumChainLength<IngredientsType>(fileReader.getDestination()));
    
  
}

/*****************************************************************/
/**
 * @class WriterNumStars
 *
 * @brief Handles BFM-File-Write \b #!number_of_stars
 * @tparam IngredientsType Ingredients class storing all system information.
 **/
template <class IngredientsType>
class WriterNumStars:public AbstractWrite<IngredientsType>
{
public:
	WriterNumStars(const IngredientsType& i)
	:AbstractWrite<IngredientsType>(i){this->setHeaderOnly(true);}
	//  WriteBonds(const IngredientsType& src):AbstractWrite<IngredientsType>(src){this->setHeaderOnly(true);}

	virtual ~WriterNumStars(){}

	virtual void writeStream(std::ostream& strm);
};

template<class IngredientsType>
void WriterNumStars<IngredientsType>::writeStream(std::ostream& stream)
{
   //get reference to molecules

	stream<<"#!number_of_stars=" << (this->getSource().getNumStars()) << std::endl<< std::endl;
}
/*****************************************************************/
/**
 * @class WriterNumBranches
 *
 * @brief Handles BFM-File-Write \b #!number_of_branches
 * @tparam IngredientsType Ingredients class storing all system information.
 **/
template <class IngredientsType>
class WriterNumBranches:public AbstractWrite<IngredientsType>
{
public:
	WriterNumBranches(const IngredientsType& i)
	:AbstractWrite<IngredientsType>(i){this->setHeaderOnly(true);}

	virtual ~WriterNumBranches(){}

	virtual void writeStream(std::ostream& strm);
};

template<class IngredientsType>
void WriterNumBranches<IngredientsType>::writeStream(std::ostream& stream)
{
	stream<<"#!number_of_branches=" << (this->getSource().getNumBranches()) << std::endl<< std::endl;
}
/*****************************************************************/
/**
 * @class WriterNumBranchLength
 *
 * @brief Handles BFM-File-Write \b #!number_of_branch_length
 * @tparam IngredientsType Ingredients class storing all system information.
 **/
template <class IngredientsType>
class WriterNumBranchLength:public AbstractWrite<IngredientsType>
{
public:
	WriterNumBranchLength(const IngredientsType& i)
	:AbstractWrite<IngredientsType>(i){this->setHeaderOnly(true);}

	virtual ~WriterNumBranchLength(){}

	virtual void writeStream(std::ostream& strm);
};

template<class IngredientsType>
void WriterNumBranchLength<IngredientsType>::writeStream(std::ostream& stream)
{
	stream<<"#!number_of_branch_length=" << (this->getSource().getNumBranchLength()) << std::endl<< std::endl;
}
/*****************************************************************/
/**
 * @class WriterNumChainLength
 *
 * @brief Handles BFM-File-Write \b #!number_of_chain_length
 * @tparam IngredientsType Ingredients class storing all system information.
 **/
template <class IngredientsType>
class WriterNumChainLength:public AbstractWrite<IngredientsType>
{
public:
	WriterNumChainLength(const IngredientsType& i)
	:AbstractWrite<IngredientsType>(i){this->setHeaderOnly(true);}

	virtual ~WriterNumChainLength(){}

	virtual void writeStream(std::ostream& strm);
};

template<class IngredientsType>
void WriterNumChainLength<IngredientsType>::writeStream(std::ostream& stream)
{
	stream<<"#!number_of_chain_length=" << (this->getSource().getNumChainLength()) << std::endl<< std::endl;
}
/**
 * @details The function is called by the Ingredients class when an object of type Ingredients
 * is associated with an object of type AnalyzerWriteBfmFile. The export of the Writes is thus
 * taken care automatically when it becomes necessary.\n
 * Registered Read-In Commands:
 * * #!number_of_stars
 * * #!number_of_branches
 * * #!number_of_branch_length
 * * #!number_of_chain_length
 *
 * @param fileReader File importer for the bfm-file
 * @param destination List of Feature to write-in from the read values.
 * @tparam IngredientsType Ingredients class storing all system information.
 **/
template<class IngredientsType>
void FeatureSystemInformationStar::exportWrite(AnalyzerWriteBfmFile< IngredientsType >& filewriter) const
{
    const IngredientsType& source=filewriter.getIngredients_();
    filewriter.registerWrite("#!number_of_stars", new WriterNumStars<IngredientsType>(source));
    filewriter.registerWrite("#!number_of_branches", new WriterNumBranches<IngredientsType>(source));
    filewriter.registerWrite("#!number_of_branch_length", new WriterNumBranchLength<IngredientsType>(source));
    filewriter.registerWrite("#!number_of_chain_length", new WriterNumChainLength<IngredientsType>(source));

}

#endif /*FEATURE_SYSTEM_INFORMATION_STAR_H*/
