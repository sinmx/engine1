/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "substance/SESubstance.h"
#include "substance/SESubstanceCompound.h"
PROTO_PUSH
#include "bind/cdm/Substance.pb.h"
PROTO_POP

class CDM_DECL SESubstanceManager : public Loggable
{
public:
  SESubstanceManager(Logger* logger);
  virtual ~SESubstanceManager();

  virtual void                                     Clear();  
  virtual void                                     Reset();
  virtual bool                                     LoadSubstanceDirectory();

  virtual const std::vector<SESubstance*>&         GetSubstances() const;
  virtual SESubstance*                             GetSubstance(const std::string& name) const;  
  virtual void                                     AddSubstance(SESubstance& substance);

  virtual bool                                     IsActive(const SESubstance& substance) const;
  virtual const std::vector<SESubstance*>&         GetActiveSubstances() const;
  virtual void                                     AddActiveSubstance(SESubstance& substance);
  virtual void                                     RemoveActiveSubstance(const SESubstance& substance);
  virtual void                                     RemoveActiveSubstances(const std::vector<SESubstance*>& substances);
  virtual void                                     RemoveActiveSubstances();
  
  virtual const std::vector<SESubstance*>&         GetActiveGases() const;
  virtual const std::vector<SESubstance*>&         GetActiveLiquids() const;

  virtual const std::vector<SESubstanceCompound*>& GetCompounds() const;
  virtual SESubstanceCompound*                     GetCompound(const std::string& name) const;
  virtual void                                     AddCompound(SESubstanceCompound& compound);

  virtual bool                                     IsActive(const SESubstanceCompound& compound) const;
  virtual const std::vector<SESubstanceCompound*>& GetActiveCompounds() const;
  virtual void                                     AddActiveCompound(SESubstanceCompound& compound);
  virtual void                                     RemoveActiveCompound(SESubstanceCompound& compound);
  virtual void                                     RemoveActiveCompounds(const std::vector<SESubstanceCompound*>& compounds);

protected:

  // I am making all these vectors of SESubstances,
  // Usually these are passed to methods that take a 
  // vector of substances, and a vector of drugs won't be accepted
  // I could do some crazy template methods that will auto convert
  // these to substance vectors, but I think that would be too 
  // slow for the amount of times we call stuff
  // So for now, these will just be substance vectors
  // If we want drug vectors, I can rename methods to 
  // std::vector<SESubstance*>* GetActiveDrugSubstances 
  // std::vector<SEDrug*>* GetActiveDrugs()
  // But I don't see a need for that yet...
  // So will just leave it as it now

  std::vector<SESubstance*>  m_Substances;
  std::vector<SESubstance*>  m_ActiveSubstances;
  std::vector<SESubstance*>  m_ActiveGases;
  std::vector<SESubstance*>  m_ActiveLiquids;

  std::vector<SESubstanceCompound*>  m_Compounds;
  std::vector<SESubstanceCompound*>  m_ActiveCompounds;

private:
  std::map<SESubstance*, const cdm::SubstanceData*> m_OriginalSubstanceData;  
  std::map<SESubstanceCompound*, const cdm::SubstanceData_CompoundData*> m_OriginalCompoundData;
};