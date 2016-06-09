//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: G4WeightWindowStore.cc,v 1.4 2003/10/22 13:52:10 gcosmo Exp $
// GEANT4 tag $Name: geant4-06-00-patch-01 $
//
// ----------------------------------------------------------------------
// GEANT 4 class source file
//
// G4WeightWindowStore
//
// ----------------------------------------------------------------------


#include "G4WeightWindowStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4GeometryCellStepStream.hh"


G4WeightWindowStore::
G4WeightWindowStore(const G4VPhysicalVolume &worldvolume) :
  fWorldVolume(worldvolume),
  fGeneralUpperEnergyBounds(),
  fCellToUpEnBoundLoWePairsMap(),
  fCurrentIterator(fCellToUpEnBoundLoWePairsMap.end())
{}

G4WeightWindowStore::~G4WeightWindowStore()
{}


G4double G4WeightWindowStore::GetLowerWeitgh(const G4GeometryCell &gCell, 
					    G4double partEnergy) const
{
  SetInternalIterator(gCell);
  if (fCurrentIterator ==  fCellToUpEnBoundLoWePairsMap.end()) {
    Error("GetLowerWitgh: Cell does not exist");
  }
  G4UpperEnergyToLowerWeightMap upEnLoWeiPairs =
    fCurrentIterator->second;
  G4double lowerWeight = -1;
  G4bool found = false;
  for (G4UpperEnergyToLowerWeightMap::iterator it = 
	 upEnLoWeiPairs.begin(); it != upEnLoWeiPairs.end(); it++) {
    if (partEnergy < it->first) {
      lowerWeight = it->second;
      found = true;
      break;
    }
  }
  if (!found) {
    G4cout << "energy: " << partEnergy << G4endl;
    Error("GetLowerWitgh: couldn't find lower weight bound");
  }
  return lowerWeight;


}

void G4WeightWindowStore::
SetInternalIterator(const G4GeometryCell &gCell) const
{
  fCurrentIterator = fCellToUpEnBoundLoWePairsMap.find(gCell);
}

G4bool G4WeightWindowStore::
IsInWorld(const G4VPhysicalVolume &aVolume) const
{
  G4bool isIn(true);
  if (!(aVolume == fWorldVolume)) {
    isIn = fWorldVolume.GetLogicalVolume()->IsAncestor(&aVolume);
  }
  return isIn;
}


G4bool G4WeightWindowStore::IsKnown(const G4GeometryCell &gCell) const
{
  G4bool inWorldKnown(IsInWorld(gCell.GetPhysicalVolume()));
		      
  if ( inWorldKnown ) {
    SetInternalIterator(gCell);
    inWorldKnown = (fCurrentIterator!=fCellToUpEnBoundLoWePairsMap.end());
  }
  return inWorldKnown;
}


const G4VPhysicalVolume &G4WeightWindowStore::GetWorldVolume() const
{
  return fWorldVolume;
}


void G4WeightWindowStore::
AddLowerWeights(const G4GeometryCell & gCell,
                const std::vector<G4double> &lowerWeights)
{
  if (fGeneralUpperEnergyBounds.empty()) {
    Error("AddLowerWeights: no general upper energy limits set");
  }
  if (IsKnown(gCell)) {
    Error("AddLowerWeights: the cell is already in the store");
  }
  if (lowerWeights.size() != fGeneralUpperEnergyBounds.size()) {
    Error("missmatch between number of lower weights and energy bounds");
  }
  G4UpperEnergyToLowerWeightMap m;
  G4int i = 0;
  for (std::set<G4double, std::less<G4double> >::iterator it = 
	 fGeneralUpperEnergyBounds.begin(); 
       it != fGeneralUpperEnergyBounds.end();
       it++) {
    m[*it] = lowerWeights[i];
    i++;
  }
  fCellToUpEnBoundLoWePairsMap[gCell] = m;
}

 
void G4WeightWindowStore::
AddUpperEboundLowerWeightPairs(const G4GeometryCell &gCell,
                               const G4UpperEnergyToLowerWeightMap& enWeMap)
{
  if (IsKnown(gCell)) {
    Error("AddUpperEboundLowerWeightPairs: the cell is already in the store");
  }
  if (IsKnown(gCell)) {
    Error("AddUpperEboundLowerWeightPairs: the cell is already in the store");
  }
  fCellToUpEnBoundLoWePairsMap[gCell] = enWeMap;

}


void G4WeightWindowStore::
SetGeneralUpperEnergyBounds(const std::set<G4double,
                            std::less<G4double> > &enBounds)
{
  if (!fGeneralUpperEnergyBounds.empty()) {
    Error("SetGeneralUpperEnergyBounds: energy bounds already set");
  }
  fGeneralUpperEnergyBounds = enBounds;
}

  
void G4WeightWindowStore::Error(const G4String &m) const
{
  G4cerr << "ERROR - G4WeightWindowStore: " << m << G4endl;
  G4Exception("G4WeightWindowStore::Error()",
              "FatalException", FatalException, m);
}